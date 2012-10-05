#ifndef _MRKNLOG_INTERFACE_H
#define _MRKNLOG_INTERFACE_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <hardware/hardware.h>

__BEGIN_DECLS

#define MRKNLOG_HARDWARE_MODULE_ID "mrknlog"

struct mrknlog_device_t {
  struct hw_device_t common;
  
  int fd;
  
  /*
   * Flush the log device
   * 
   * Returns: 0 on success, error code on failure
   */
  int (*flush_log)(struct mrknlog_device_t* dev);
  
  /*
   * Get the total log size
   * 
   * Returns: total log size, < 0 on failure
   */
  int (*get_total_log_size)(struct mrknlog_device_t* dev);

  /*
   * Get the used log size
   * 
   * Returns: used log size, < 0 on failure
   */
  int (*get_used_log_size)(struct mrknlog_device_t* dev);
  
  /*
   * Wait until more log data becomes available or until timeout expires
   * timeout: the max wait time (in ms). The value of -1 means wait forever
   * Returns: < 0 or error, 0 on timeout, the amount of new data (in bytes)
   */ 
  int (*wait_for_log_data)(struct mrknlog_device_t* dev, int timeout);
};

__END_DECLS

#endif /* End of the _MRKNLOG_INTERFACE_H block */
