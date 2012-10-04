package com.marakana.android.service.log;

import com.marakana.android.service.log.ILogListener;

/**
 * System-private API for talking to the LogService.
 *
 * {@hide}
 */
interface ILogService {
  void flushLog();
  int getTotalLogSize();
  int getUsedLogSize();
  void register(in ILogListener listener);
  void unregister(in ILogListener listener);
}
