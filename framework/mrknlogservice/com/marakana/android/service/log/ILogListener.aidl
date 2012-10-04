package com.marakana.android.service.log;

/**
 * Listener for used log size change events.
 *
 * {@hide}
 */
oneway interface ILogListener {
  void onUsedLogSizeChange(int usedLogSize);
}
