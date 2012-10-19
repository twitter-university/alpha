package com.marakana.android.service.log;

/**
 * Used for receiving notifications from the LogManager when the buffer size has changed.
 */
public interface LogListener {
  
  /**
   * Called when the buffer size has changed.
   * Invoked on the main/looper/UI thread.
   *
   * @param usedLogSize the new log buffer size.
   */
  public void onUsedLogSizeChange(int usedLogSize);
}

