package com.marakana.android.lib.log;

/** @hide */
public class Main {
  public static void main (String[] args) {
    try {
      LibLog libLog = new LibLog();
      try {
        int usedSize = libLog.getUsedLogSize();
        int totalSize = libLog.getTotalLogSize();
        libLog.flushLog();
        System.out.printf("Flushed log. Previously it was consuming %d of %d bytes\n",
          usedSize, totalSize);
      } finally {
        libLog.close();
      }
    } catch (LibLogException e) {
	    System.err.println("Failed to flush the log");
	    e.printStackTrace();
    }
  }
}
