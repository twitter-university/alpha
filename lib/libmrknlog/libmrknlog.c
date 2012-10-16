#define LOG_NDEBUG 1                                               /* <1> */
#define LOG_FILE "/dev/log/main"                                   /* <2> */
#define LOG_TAG "MrknLog"                                          /* <1> */

#include <mrknlog.h>                                               /* <3> */
#include <cutils/log.h>                                            /* <1> */
#include <cutils/logger.h>                                         /* <4> */
#include <fcntl.h>
#include <poll.h>                                                  /* <5> */
#include <errno.h>
#include <sys/ioctl.h>                                             /* <5> */

static int ioctl_log(int mode, int request) {
  int logfd = open(LOG_FILE, mode);                                /* <5> */
  if (logfd < 0) {
    SLOGE("Failed to open %s: %s", LOG_FILE, strerror(errno));     /* <1> */
    return -1;
  } else {
    int ret = ioctl(logfd, request);                               /* <5> */
    close(logfd);                                                  /* <5> */
    return ret;
  }
}

static int flush_log(struct mrknlog_device_t* dev) {
  SLOGV("Flushing %s", LOG_FILE);                                  /* <1> */
  return ioctl_log(O_WRONLY, LOGGER_FLUSH_LOG);                    /* <4> */
}

static int get_total_log_size(struct mrknlog_device_t* dev) {
  SLOGV("Getting total buffer size of %s", LOG_FILE);              /* <1> */
  return ioctl_log(O_RDONLY, LOGGER_GET_LOG_BUF_SIZE);             /* <4> */
}

static int get_used_log_size(struct mrknlog_device_t* dev) {
  SLOGV("Getting used buffer size of %s", LOG_FILE);               /* <1> */
  return ioctl_log(O_RDONLY, LOGGER_GET_LOG_LEN);                  /* <4> */
}

static int wait_for_log_data(struct mrknlog_device_t* dev, int timeout) {
  SLOGV("Waiting for log data on %s with timeout=%d", 
    LOG_FILE, timeout); /* <1> */
  int ret;
  struct pollfd pfd;
  pfd.fd = dev->fd;
  pfd.events = POLLIN;
  
  ret = poll(&pfd, 1, timeout);                                    /* <5> */
  if (ret < 0) {
    SLOGE("Failed to poll %s: %s", LOG_FILE, strerror(errno));     /* <1> */
    return -1;
  } else if (ret == 0) {
    return 0; /* timeout */
  } else {
    /* consume all of the available data */
    unsigned char buf[LOGGER_ENTRY_MAX_LEN + 1] __attribute__((aligned(4)));
    int new_data_counter = 0;
    while(1) {
      /* we have to read because the file is not seekable */
      ret = read(dev->fd, buf, LOGGER_ENTRY_MAX_LEN);              /* <5> */
      if (ret < 0) {
        SLOGE("Failed to read %s: %s", LOG_FILE, strerror(errno)); /* <1> */
        return -1;
      } else if (ret == 0) {
        SLOGE("Unexpected EOF on reading %s", LOG_FILE);           /* <1> */
        return -1;
      } else {
        new_data_counter += ret;
        /* check to see if there is more data to read */
        ret = ioctl(dev->fd, LOGGER_GET_NEXT_ENTRY_LEN);           /* <4> */
        if (ret < 0) {
          SLOGE("Failed to get next entry len on %s: %s",          /* <1> */
            LOG_FILE, strerror(errno)); 
          return -1;
        } else if (ret == 0) { /* no more data; we are done */
          SLOGV("Got %d / %d / %d on %s", new_data_counter,        /* <1> */
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
    close(dev->fd);                                                /* <5> */
    free(dev);                                                     /* <6> */
  }
  return 0;
}

static int open_mrknlog(const struct hw_module_t *module, char const *name,
   struct hw_device_t **device) {
  int fd = open(LOG_FILE, O_RDWR);                                 /* <5> */
  if (fd < 0) {
    SLOGE("Failed to open %s: %s", LOG_FILE, strerror(errno));
    return -1;
  } else {
    struct mrknlog_device_t *dev =                                 /* <3> */
      malloc(sizeof(struct mrknlog_device_t));                     /* <6> */
    if (!dev) {
      return -ENOMEM;
    }
    SLOGV("Opened %s", LOG_FILE);                                  /* <1> */
    memset(dev, 0, sizeof(*dev));                                  /* <6> */
    dev->common.tag = HARDWARE_DEVICE_TAG;                         /* <6> */
    dev->common.version = 0;                                       /* <6> */
    dev->common.module = (struct hw_module_t *)module;             /* <6> */
    dev->common.close =                                            /* <7> */
      (int (*)(struct hw_device_t *)) close_mrknlog;
    dev->fd = fd;                                                  /* <6> */
    dev->flush_log = flush_log;                                    /* <7> */
    dev->get_total_log_size = get_total_log_size;                  /* <7> */
    dev->get_used_log_size = get_used_log_size;                    /* <7> */
    dev->wait_for_log_data = wait_for_log_data;                    /* <7> */
    *device = (struct hw_device_t *)dev;
    return 0;
  }
}

static struct hw_module_methods_t mrknlog_module_methods = {
  .open = open_mrknlog,                                            /* <7> */
};

struct hw_module_t HAL_MODULE_INFO_SYM = {                         /* <9> */
  .tag = HARDWARE_MODULE_TAG,
  .version_major = 1,
  .version_minor = 0,
  .id = MRKNLOG_HARDWARE_MODULE_ID,                                /* <3> */
  .name = "mrknlog module",
  .author = "Marakana, Inc.",
  .methods = &mrknlog_module_methods,                              /* <7> */
};
