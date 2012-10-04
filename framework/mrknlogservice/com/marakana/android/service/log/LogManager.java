package com.marakana.android.service.log;

import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;
import java.util.HashSet;
import java.util.Set;

public class LogManager {
  private static final String TAG = "LogManager";
  private static final String REMOTE_SERVICE_NAME = ILogService.class.getName();
  
  public static interface LogListener {
    public void onUsedLogSizeChange(int usedLogSize);
  }

  private final ILogService service;

  private final Set<LogListener> listeners = new HashSet<LogListener>();
  
  private final ILogListener listener = new ILogListener.Stub() {
    public void onUsedLogSizeChange(final int usedLogSize) {
      Message message = LogManager.this.handler.obtainMessage();
      message.arg1 = usedLogSize;
      LogManager.this.handler.sendMessage(message);
    }
  };
  
  private final Handler handler = new Handler() {
    @Override
    public void handleMessage(Message message) {
      int usedLogSize = message.arg1;
      synchronized(LogManager.this.listeners) {
        for (LogListener logListener : LogManager.this.listeners) {
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
      Log.d(TAG, "Flushing logs. If it works, you won't see this message.");
      this.service.flushLog();
    } catch (RemoteException e) {
      throw new RuntimeException("Failed to flush log", e);
    }
  }
  
  public int getTotalLogSize() {
    try {
      return this.service.getTotalLogSize();
    } catch (RemoteException e) {
      throw new RuntimeException("Failed to get total log size", e);
    }
  }
  
  public int getUsedLogSize() {
    try {
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
            if (this.listeners.isEmpty()) {
              this.service.register(this.listener);
            }
            this.listeners.add(listener);
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
        if (!this.listeners.remove(listener)) {
           Log.w(TAG, "Not registered: " + listener);
        }
        if (this.listeners.isEmpty()) {
          try {
            this.service.unregister(this.listener);

          } catch (RemoteException e) {
            throw new RuntimeException("Failed to unregister " + listener, e);
          }
        }
      }
    }
  }
}
