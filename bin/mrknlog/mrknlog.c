#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <mrknlog.h>
#include <hardware/hardware.h>

int main (int argc, char* argv[]) {
  hw_module_t* module;
  int ret = hw_get_module(MRKNLOG_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
  if (ret == 0) {
    struct mrknlog_device_t *dev;
    ret = module->methods->open(module, 0, (struct hw_device_t **) &dev);
    if (ret == 0) {
      int usedSize = dev->get_used_log_size(dev);
      int totalSize = dev->get_total_log_size(dev);
      if (totalSize >= 0 && usedSize >= 0) {
        if (dev->flush_log(dev) == 0) {
          printf("Flushed log. Previously it was consuming %d of %d bytes\n",
            usedSize, totalSize);
          ret = 0;   
        } else {
          fprintf(stderr, "Failed to flush log: %s", strerror(errno));
          ret = -1;
        }
      } else {
        fprintf(stderr, "Failed to get log size: %s", strerror(errno));
        ret = -2;
      }
      dev->common.close((struct hw_device_t *)dev);
    } else {
      fprintf(stderr, "Failed to open device: %d", ret);
      ret = -3;
    }
  } else {
    fprintf(stderr, "Failed to get module: %s", MRKNLOG_HARDWARE_MODULE_ID);
    ret = -4;    
  }
  return ret;
}
