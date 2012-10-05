#define LOG_NDEBUG 1
#define LOG_FILE "/dev/log/main"
#define LOG_TAG "MrknLog"

#include <mrknlog.h>
#include <cutils/log.h>
#include <cutils/logger.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <sys/ioctl.h>

static int ioctl_log(int mode, int request) {
  int logfd = open(LOG_FILE, mode);
  if (logfd < 0) {
    SLOGE("Failed to open %s: %s", LOG_FILE, strerror(errno));
    return -1;
  } else {
    int ret = ioctl(logfd, request);
    close(logfd);
    return ret;
  }
}

static int flush_log(struct mrknlog_device_t* dev) {
  SLOGV("Flushing %s", LOG_FILE);
  return ioctl_log(O_WRONLY, LOGGER_FLUSH_LOG);
}

static int get_total_log_size(struct mrknlog_device_t* dev) {
  SLOGV("Getting total buffer size of %s", LOG_FILE);
  return ioctl_log(O_RDONLY, LOGGER_GET_LOG_BUF_SIZE);
}

static int get_used_log_size(struct mrknlog_device_t* dev) {
  SLOGV("Getting used buffer size of %s", LOG_FILE);
  return ioctl_log(O_RDONLY, LOGGER_GET_LOG_LEN);
}

static int wait_for_log_data(struct mrknlog_device_t* dev, int timeout) {
  SLOGV("Waiting for log data on %s with timeout=%d", LOG_FILE, timeout);
  int ret;
  struct pollfd pfd;
  pfd.fd = dev->fd;
  pfd.events = POLLIN;
  
  ret = poll(&pfd, 1, timeout);
  if (ret < 0) {
    SLOGE("Failed to poll %s: %s", LOG_FILE, strerror(errno));
    return -1;
  } else if (ret == 0) {
    return 0; /* timeout */
  } else {
    /* consume all of the available data */
    unsigned char buf[LOGGER_ENTRY_MAX_LEN + 1] __attribute__((aligned(4)));
    int new_data_counter = 0;
    while(1) {
      /* we have to read because the file is not seekable */
      ret = read(dev->fd, buf, LOGGER_ENTRY_MAX_LEN);
      if (ret < 0) {
        SLOGE("Failed to read %s: %s", LOG_FILE, strerror(errno));
        return -1;
      } else if (ret == 0) {
        SLOGE("Unexpected EOF on reading %s", LOG_FILE);
        return -1;
      } else {
        new_data_counter += ret;
        ret = ioctl(dev->fd, LOGGER_GET_NEXT_ENTRY_LEN);
        if (ret < 0) {
          SLOGE("Failed to get next entry len on %s: %s", LOG_FILE, strerror(errno));
          return -1;
        } else if (ret == 0) { /* no more data; we are done */
          SLOGV("Got %d / %d / %d on %s", new_data_counter,
            get_used_log_size(dev), get_total_log_size(dev), LOG_FILE);
          return new_data_counter;
        }
      } 
    }
  }
}

static int close_mrknlog(struct mrknlog_device_t* dev) {
	SLOGV("Closing %s", LOG_FILE);
	if (dev) {
    close(dev->fd);
		free(dev);
  }
	return 0;
}

static int open_mrknlog(const struct hw_module_t *module, char const *name,
   struct hw_device_t **device) {
  int fd = open(LOG_FILE, O_RDWR);
  if (fd < 0) {
    SLOGE("Failed to open %s: %s", LOG_FILE, strerror(errno));
    return -1;
  } else {
	  struct mrknlog_device_t *dev = malloc(sizeof(struct mrknlog_device_t));
    if (!dev) {
      return -ENOMEM;
    }
    SLOGV("Opened %s", LOG_FILE);
	  memset(dev, 0, sizeof(*dev));
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t *)module;
    dev->common.close = (int (*)(struct hw_device_t *))close_mrknlog;
	  dev->fd = fd;
    dev->flush_log = flush_log;
    dev->get_total_log_size = get_total_log_size;
    dev->get_used_log_size = get_used_log_size;
    dev->wait_for_log_data = wait_for_log_data;
	  *device = (struct hw_device_t *)dev;
    return 0;
  }
}

static struct hw_module_methods_t mrknlog_module_methods = {
	.open =  open_mrknlog,
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
	.tag = HARDWARE_MODULE_TAG,
	.version_major = 1,
	.version_minor = 0,
	.id = MRKNLOG_HARDWARE_MODULE_ID,
	.name = "mrknlog module",
	.author = "Marakana, Inc.",
	.methods = &mrknlog_module_methods,
};
