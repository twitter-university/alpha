package com.marakana.android.service.log;

import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;
import android.util.Slog; /* This is to avoid generating events for ourselves */
import java.util.HashSet;
import java.util.Set;

public class LogManager {
  private static final String TAG = "LogManager";
  private static final String REMOTE_SERVICE_NAME = ILogService.class.getName();
  private static final boolean DEBUG = false;
  
  public static interface LogListener {
    public void onUsedLogSizeChange(int usedLogSize);
  }

  private final ILogService service;

  private final Set<LogListener> listeners = new HashSet<LogListener>();
  
  private final ILogListener listener = new ILogListener.Stub() {
    public void onUsedLogSizeChange(final int usedLogSize) {
      if (DEBUG) Slog.d(TAG, "There is more used data: " + usedLogSize);
      Message message = LogManager.this.handler.obtainMessage();
      message.arg1 = usedLogSize;
      LogManager.this.handler.sendMessage(message);
    }
  };
  
  private final Handler handler = new Handler() {
    @Override
    public void handleMessage(Message message) {
      int usedLogSize = message.arg1;
      if (DEBUG) Slog.d(TAG, "Notifying local listeners of more used data: " + usedLogSize);
      synchronized(LogManager.this.listeners) {        
        for (LogListener logListener : LogManager.this.listeners) {
          if (DEBUG) Slog.d(TAG, "Notifying local listener [" + logListener + "] of more used data: " + usedLogSize);
          logListener.onUsedLogSizeChange(usedLogSize);
        }
      }
    }
  };
  
  public static LogManager getInstance() {
    return new LogManager();
  }
    
  private LogManager() {
    Log.d(TAG, "Connecting to ILogService by name [" + REMOTE_SERVICE_NAME + "]");
    this.service = ILogService.Stub.asInterface(ServiceManager.getService(REMOTE_SERVICE_NAME));
    if (this.service == null) {
      throw new IllegalStateException("Failed to find ILogService by name [" + REMOTE_SERVICE_NAME + "]");
    }
  }   
  
  public void flushLog() {
    try {
      if (DEBUG) Slog.d(TAG, "Flushing log.");
      this.service.flushLog();
    } catch (RemoteException e) {
      throw new RuntimeException("Failed to flush log", e);
    }
  }
  
  public int getTotalLogSize() {
    try {
      if (DEBUG) Slog.d(TAG, "Getting total log size.");
      return this.service.getTotalLogSize();
    } catch (RemoteException e) {
      throw new RuntimeException("Failed to get total log size", e);
    }
  }
  
  public int getUsedLogSize() {
    try {
      if (DEBUG) Slog.d(TAG, "Getting used log size.");
      return this.service.getUsedLogSize();
    } catch (Exception e) {
      throw new RuntimeException("Failed to get used log size", e);
    }
  }
  
  public void register(LogListener listener) {
    if (listener != null) {
      synchronized(this.listeners) {
        if (this.listeners.contains(listener)) {
          Log.w(TAG, "Already registered: " + listener);
        } else {
          try {
            boolean registerRemote = this.listeners.isEmpty();
            if (DEBUG) Log.d(TAG, "Registering local listener.");
            this.listeners.add(listener);
            if (registerRemote) {
              if (DEBUG) Log.d(TAG, "Registering remote listener.");
              this.service.register(this.listener);
            }
          } catch (RemoteException e) {
            throw new RuntimeException("Failed to register " + listener, e);
          }
        }
      }
    }
  }
  
  public void unregister(LogListener listener) {
    if (listener != null) {
      synchronized(this.listeners) {
        if (!this.listeners.contains(listener)) {
          Log.w(TAG, "Not registered: " + listener);
        } else {
          if (DEBUG) Log.d(TAG, "Unregistering local listener.");
          this.listeners.remove(listener);
        }
        if (this.listeners.isEmpty()) {
          try {
            if (DEBUG) Log.d(TAG, "Unregistering remote listener.");
            this.service.unregister(this.listener);
          } catch (RemoteException e) {
            throw new RuntimeException("Failed to unregister " + listener, e);
          }
        }
      }
    }
  }
}
