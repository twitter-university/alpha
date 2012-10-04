#ifndef _MRKN_LOG_H_
#define _MRKN_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Flush the log.
 * @return < 0 on error; 0 otherwise
 */
extern int mrkn_flush_log();

/*
 * Get the max log size
 * @return < 0 on error; max log size otherwise
 */
extern int mrkn_get_total_log_size();

/*
 * Get the used log size
 * @return < 0 on error; used log size otherwise
 */
extern int mrkn_get_used_log_size();

/*
 * Wait until log data becomes available or until timeout expires.
 * @param timeout - max wait time, in ms (-1 means wait forever)
 * @return < 0 or error, 0 on timeout, > 0 on data available
 */ 
extern int mrkn_wait_for_log_data(int timeout);

#ifdef __cplusplus
}  /* End of the 'extern "C"' block */
#endif
#endif /* End of the _MRKN_LOG_H_ block */

