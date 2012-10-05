package com.marakana.android.lib.log;

public class LibLog {
  public native static int init() throws LibLogException;
  public native static void close(int handle);
  public native static void flushLog(int handle) throws LibLogException;
  public native static int getTotalLogSize(int handle) throws LibLogException;
  public native static int getUsedLogSize(int handle) throws LibLogException;
  public native static boolean waitForLogData(int handle, int timeoutInMs) throws LibLogException;

  static {
     System.loadLibrary("mrknlog_jni");
  }
}

