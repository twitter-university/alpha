package com.marakana.android.lib.log;

public class LibLog {
  
  private int nativeHandle;
  
  public LibLog() {
    this.init();
  }
    
  @Override
  protected void finalize() {
    this.close();
  }

  private native void init() throws LibLogException;
  public native void close();  
  public native void flushLog() throws LibLogException;
  public native int getTotalLogSize() throws LibLogException;
  public native int getUsedLogSize() throws LibLogException;
  public native boolean waitForLogData(int timeoutInMs) throws LibLogException;

  static {
     System.loadLibrary("mrknlog_jni");
  }
}

