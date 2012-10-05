package com.marakana.android.logservice;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Slog;
import java.util.HashMap;
import java.util.Map;
import com.marakana.android.service.log.ILogListener;
import com.marakana.android.service.log.ILogService;
import com.marakana.android.lib.log.LibLog;
import com.marakana.android.lib.log.LibLogException;

class ILogServiceImpl extends ILogService.Stub {
  private static final String TAG = "ILogServiceImpl";
  private static final int INCREMENTAL_TIMEOUT = 2 * 1000;
  private static final boolean DEBUG = false;
  private final Map<IBinder, ListenerTracker> listeners = new HashMap<IBinder, ListenerTracker>();
  private final Context context;
  private LogServiceThread logServiceThread;
  private int nativeHandle;

  ILogServiceImpl(Context context) {
    this.context = context;
    this.nativeHandle = LibLog.init();
  }
  
  protected void finalize() throws Throwable {
    LibLog.close(this.nativeHandle);
    super.finalize();
  }

  public void flushLog() {
    this.context.enforceCallingOrSelfPermission(Manifest.permission.FLUSH_LOG, "Flush somewhere else");
    if (DEBUG) Slog.d(TAG, "Flushing log.");
    LibLog.flushLog(this.nativeHandle);
  }

  public int getUsedLogSize() {
    if (DEBUG) Slog.d(TAG, "Getting used log size.");
    return LibLog.getUsedLogSize(this.nativeHandle);
  }

  public int getTotalLogSize() {
    if (DEBUG) Slog.d(TAG, "Getting total log size.");
    return LibLog.getTotalLogSize(this.nativeHandle);
  }

  public void register(ILogListener listener) throws RemoteException {
    if (listener != null) {
      IBinder binder = listener.asBinder();
      synchronized(this.listeners) {
        if (this.listeners.containsKey(binder)) {
          Slog.w(TAG, "Ignoring duplicate listener registration attempt: " + binder);
        } else {
          ListenerTracker listenerTracker = new ListenerTracker(listener);
          binder.linkToDeath(listenerTracker, 0);
          this.listeners.put(binder, listenerTracker);
          if (DEBUG) Slog.d(TAG, "Registered listener: " + binder);
          if (this.logServiceThread == null) {
            if (DEBUG) Slog.d(TAG, "Starting thread");
            this.logServiceThread = new LogServiceThread();
            this.logServiceThread.start();
          }
        }
      }
    } 
  }
  
  public void unregister(ILogListener listener) {
    if (listener != null) {
      IBinder binder = listener.asBinder();
      synchronized(this.listeners) {
        ListenerTracker listenerTracker = this.listeners.remove(binder);
        if (listenerTracker == null) {
          Slog.w(TAG, "Ignoring unregistered listener unregistration attempt: " + binder);
        } else {
          if (DEBUG) Slog.d(TAG, "Unregistered listener: " + binder);
          binder.unlinkToDeath(listenerTracker, 0);
          if (this.logServiceThread != null && this.listeners.isEmpty()) {
            if (DEBUG) Slog.d(TAG, "Stopping thread");
            this.logServiceThread.interrupt();
            this.logServiceThread = null;
          }
        }
      }
    }
  }

  private final class ListenerTracker implements IBinder.DeathRecipient {
    private final ILogListener listener;
    
    public ListenerTracker(ILogListener listener) {
      this.listener = listener;
    }

    public ILogListener getListener() {
      return this.listener;
    }

    public void binderDied() {
      ILogServiceImpl.this.unregister(this.listener);
    }
  }

  private final class LogServiceThread extends Thread {
    public void run() {
      while(!Thread.interrupted()) {
        try {
          if (DEBUG) Slog.d(TAG, "Waiting for log data");
          int usedSize = ILogServiceImpl.this.getUsedLogSize();
          if (LibLog.waitForLogData(ILogServiceImpl.this.nativeHandle, INCREMENTAL_TIMEOUT)
            || (usedSize != 0 && ILogServiceImpl.this.getUsedLogSize() == 0)) {
            usedSize = ILogServiceImpl.this.getUsedLogSize();
            if (DEBUG) Slog.d(TAG, "Log data changed. Used data is now at " + usedSize);  
            synchronized(ILogServiceImpl.this.listeners) {
              for (ListenerTracker listenerTracker : ILogServiceImpl.this.listeners.values()) {
                try {
                  if (DEBUG) Slog.d(TAG, "Notifying listener: " +  listenerTracker.getListener().asBinder());
                  listenerTracker.getListener().onUsedLogSizeChange(usedSize);
                } catch (RemoteException e) {
                  Slog.e(TAG, "Failed to update listener: " + listenerTracker.getListener().asBinder(), e); 
                  ILogServiceImpl.this.unregister(listenerTracker.getListener());
                }
              }
            }  
          }
        } catch (LibLogException e) {
          Slog.e(TAG, "Oops", e);
          try {
            Thread.sleep(INCREMENTAL_TIMEOUT);
          } catch (InterruptedException e2) {
            break;
          }
        }
      }
    } 
  }
}
