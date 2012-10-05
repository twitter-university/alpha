package com.marakana.android.lib.log;

/** @hide */
public class Main {
  public static void main (String[] args) {
	try {
      int handle = LibLog.init();
      int usedSize = LibLog.getUsedLogSize(handle);
      int totalSize = LibLog.getTotalLogSize(handle);
      LibLog.flushLog(handle);
      LibLog.close(handle);
      System.out.printf("Flushed log. Previously it was consuming %d of %d bytes\n",
          usedSize, totalSize);
    } catch (LibLogException e) {
	  System.err.println("Failed to flush the log");
	  e.printStackTrace();
    }
  }
}
