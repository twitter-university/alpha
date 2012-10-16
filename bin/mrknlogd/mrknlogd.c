#define LOG_TAG "MRKN Log Daemon"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <mrknlog.h>
#include <cutils/log.h>
#include <hardware/hardware.h>

int runFlag = 1;

static void sig_handler(int signo) {
  SLOGI("Caught signal %d. Scheduling exit.", signo);
  runFlag = 0;
}

int main (int argc, char* argv[]) {
  int ret;
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <flush-frequency-in-seconds>\n", argv[0]);
    ret = -1;
  } else {
    hw_module_t* module;
    ret = hw_get_module(MRKNLOG_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
    if (ret == 0) {
      struct mrknlog_device_t *dev;
      ret = module->methods->open(module, 0, (struct hw_device_t **) &dev);
      if (ret == 0) {
        int frequency = atoi(argv[1]);
        int totalSize = dev->get_total_log_size(dev);
        int usedSize;
        int count = 1;
        while(runFlag) {
          usedSize = dev->get_used_log_size(dev);
          if (dev->flush_log(dev) == 0) {
            SLOGI("Flushed log (%d, %d of %d bytes). Waiting %d seconds before the next flush.", 
              count, usedSize, totalSize, frequency);
          } else {
            SLOGE("Failed to flush log. Bailing out");
            break;
          }
          count++;
          sleep(frequency);
        }
        SLOGI("Done after %d iterations.", count);
        dev->common.close((struct hw_device_t *)dev);
      } else {
        fprintf(stderr, "Failed to open device: %d", ret);
        ret = -2;
      }
    } else {
      fprintf(stderr, "Failed to get module: %s", MRKNLOG_HARDWARE_MODULE_ID);
      ret = -3;    
    }
  }
  return ret;
}
