package com.marakana.android.logservice;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Binder;
import android.os.RemoteException;
import android.util.Log;
import com.marakana.android.service.log.ILogService;
import com.marakana.android.lib.log.LibLog;
import java.io.FileDescriptor;
import java.io.PrintWriter;
import java.util.concurrent.atomic.AtomicInteger;

class ILogServiceImpl extends ILogService.Stub {
  private static final String TAG = "ILogServiceImpl";
  private final Context context;
  private final AtomicInteger flushCounter = new AtomicInteger();

  ILogServiceImpl(Context context) {
    this.context = context;
  }

  public void flushLog() {
    if (this.context.checkCallingOrSelfPermission(Manifest.permission.FLUSH_LOG) != 
        PackageManager.PERMISSION_GRANTED) {
      throw new SecurityException("Requires FLUSH_LOG permission");
    }
    Log.d(TAG, "Flushing logs. If it works, you won't see this message.");
    LibLog.flushLog();
    this.flushCounter.incrementAndGet();
  }

  public int getUsedLogSize() {
    return LibLog.getUsedLogSize();
  }

  public int getTotalLogSize() {
    return LibLog.getTotalLogSize();
  }

  protected void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
    if (this.context.checkCallingOrSelfPermission(android.Manifest.permission.DUMP) != 
        PackageManager.PERMISSION_GRANTED) {
      pw.print("Permission Denial: can't dump ILogService from from pid=");
      pw.print(Binder.getCallingPid());
      pw.print(", uid=");
      pw.println(Binder.getCallingUid());
      return;
    } else if (args.length > 0 && args[0] != null) {
      if (args[0].equals("flush-count")) {
        pw.println(this.flushCounter.get());
      } else if (args[0].equals("used-size")) {
        pw.println(this.getUsedLogSize());
      } else if (args[0].equals("total-size")) {
        pw.println(this.getTotalLogSize());
      } else {
        pw.println("Usage: ILogService [flush-count|used-size|total-size]");
      }
    } else {
      pw.println("ILogServiceState:");
      pw.print("Flush count: ");
      pw.println(this.flushCounter.get());
      pw.print("Used log size: ");
      pw.println(this.getUsedLogSize());
      pw.print("Total log size: ");
      pw.println(this.getTotalLogSize());
    }
  }
}
