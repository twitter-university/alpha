package com.marakana.android.lib.log;

public class LibLog {
  public native static void flushLog() throws LibLogException;
  public native static int getTotalLogSize() throws LibLogException;
  public native static int getUsedLogSize() throws LibLogException;
  public native static boolean waitForLogData(int timeoutInMs) throws LibLogException;

  static {
     System.loadLibrary("mrknlog_jni");
  }
}

