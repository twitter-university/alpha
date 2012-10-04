package com.marakana.android.logservice;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import java.util.HashMap;
import java.util.Map;
import com.marakana.android.service.log.ILogListener;
import com.marakana.android.service.log.ILogService;
import com.marakana.android.lib.log.LibLog;
import com.marakana.android.lib.log.LibLogException;

class ILogServiceImpl extends ILogService.Stub {
  private static final String TAG = "ILogServiceImpl";
  private static final int INCREMENTAL_TIMEOUT = 10 * 1000;
  private final Map<IBinder, ListenerTracker> listeners = new HashMap<IBinder, ListenerTracker>();
  private final Context context;
  private LogServiceThread logServiceThread;

  ILogServiceImpl(Context context) {
    this.context = context;
  }

  public void flushLog() {
    this.context.enforceCallingOrSelfPermission(Manifest.permission.FLUSH_LOG, "Flush somewhere else");
    Log.d(TAG, "Flushing logs. If it works, you won't see this message if you refresh you logcat reader.");
    LibLog.flushLog();
  }

  public int getUsedLogSize() {
    return LibLog.getUsedLogSize();
  }

  public int getTotalLogSize() {
    return LibLog.getTotalLogSize();
  }

  public void register(ILogListener listener) throws RemoteException {
    if (listener != null) {
      IBinder binder = listener.asBinder();
      synchronized(this.listeners) {
        if (!this.listeners.containsKey(binder)) {
          ListenerTracker listenerTracker = new ListenerTracker(listener);
          binder.linkToDeath(listenerTracker, 0);
          this.listeners.put(binder, listenerTracker);
          if (this.logServiceThread == null) {
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
        if (listenerTracker != null) {
          binder.unlinkToDeath(listenerTracker, 0);
          if (this.logServiceThread != null && this.listeners.isEmpty()) {
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
          if (LibLog.waitForLogData(INCREMENTAL_TIMEOUT)) {
            int usedSize = getUsedLogSize();
            synchronized(ILogServiceImpl.this.listeners) {
              for (ListenerTracker listenerTracker : ILogServiceImpl.this.listeners.values()) {
                try {
                  listenerTracker.getListener().onUsedLogSizeChange(usedSize);
                } catch (RemoteException e) {
                  Log.wtf(TAG, "Failed to update listener", e); 
                  ILogServiceImpl.this.unregister(listenerTracker.getListener());
                }
              }
            }  
          }
        } catch (LibLogException e) {
          Log.wtf(TAG, "Oops", e);
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
