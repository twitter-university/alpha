#include <jni.h>
#include <mrknlog.h>
#include <hardware/hardware.h>
#include "JNIHelp.h"

static void throwLibLogException(JNIEnv *env, const char *msg) {
  jniThrowException(env, "com/marakana/android/lib/log/LibLogException", msg);
}

static jint native_init(JNIEnv *env, jclass clazz) {
  hw_module_t* module;
  int err = hw_get_module(MRKNLOG_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
  if (err) {
    throwLibLogException(env, "Failed to get module");
    return -1;
  }
  struct mrknlog_device_t *dev;
  err = module->methods->open(module, 0, (struct hw_device_t **) &dev);
  if (err) {
    throwLibLogException(env, "Failed to open device");
    return -2;
  }
  return (jint) dev;
}

static void native_close(JNIEnv *env, jclass clazz, jint handle) {
  struct mrknlog_device_t *dev = (struct mrknlog_device_t *) handle;
  dev->common.close((struct hw_device_t *)dev);
}

static void flushLog(JNIEnv *env, jclass clazz, jint handle) {
  struct mrknlog_device_t *dev = (struct mrknlog_device_t *) handle;
  if (dev->flush_log(dev) != 0) {
    throwLibLogException(env, "Failed to flush log");
  }
}

static jint getTotalLogSize(JNIEnv *env, jclass clazz, jint handle) {
  struct mrknlog_device_t *dev = (struct mrknlog_device_t *) handle;
  int ret = dev->get_total_log_size(dev);
  if (ret < 0) {
    throwLibLogException(env, "Failed to get total log size");
  }
  return ret;
}

static jint getUsedLogSize(JNIEnv *env, jclass clazz, jint handle) {
  struct mrknlog_device_t *dev = (struct mrknlog_device_t *) handle;
  int ret = dev->get_used_log_size(dev);
  if (ret < 0) {
    throwLibLogException(env, "Failed to get used log size");
  }
  return ret;
}

static jboolean waitForLogData(JNIEnv *env, jclass clazz, jint handle, jint timeoutInMs) {
  struct mrknlog_device_t *dev = (struct mrknlog_device_t *) handle;
  int ret = dev->wait_for_log_data(dev, timeoutInMs);
  if (ret < 0) {
    throwLibLogException(env, "Failed while waiting for log data");
  } 
  return ret > 0 ? JNI_TRUE : JNI_FALSE;
}

static JNINativeMethod method_table[] = {
  { "init", "()I", (void *) native_init },
  { "close", "(I)V", (void *) native_close },
  { "flushLog", "(I)V", (void *) flushLog },
  { "getTotalLogSize", "(I)I", (void *) getTotalLogSize },
  { "getUsedLogSize", "(I)I", (void *) getUsedLogSize },
  { "waitForLogData", "(II)Z", (void *) waitForLogData }
};

extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env = NULL;
  if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) == JNI_OK) {
    if (jniRegisterNativeMethods(env, "com/marakana/android/lib/log/LibLog", 
      method_table, NELEM(method_table)) == 0) {
      return JNI_VERSION_1_4;
    }
  }
  return JNI_ERR;
}
