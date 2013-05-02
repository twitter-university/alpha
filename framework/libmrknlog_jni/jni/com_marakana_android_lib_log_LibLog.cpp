#include <jni.h>
#include <mrknlog.h>
#include <hardware/hardware.h>
#include "JNIHelp.h"

static const char * class_name = "com/marakana/android/lib/log/LibLog";

static jfieldID nativeHandleFieldId;

static void throwLibLogException(JNIEnv *env, const char *msg) {
  jniThrowException(env, "com/marakana/android/lib/log/LibLogException", msg);
}

static void lib_init(JNIEnv *env, jclass clazz) {
  nativeHandleFieldId = env->GetFieldID(clazz, "nativeHandle", "I");
  if (!nativeHandleFieldId) {
    throwLibLogException(env, "No such field 'nativeHandle'");
  }
}

static void native_init(JNIEnv *env, jobject object) {
  hw_module_t* module;
  int err = hw_get_module(MRKNLOG_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
  if (err) {
    throwLibLogException(env, "Failed to get module");
  } else {
    struct mrknlog_device_t *dev;
    err = module->methods->open(module, 0, (struct hw_device_t **) &dev);
    if (err) {
      throwLibLogException(env, "Failed to open device");
    } else {
      env->SetIntField(object, nativeHandleFieldId, (jint) dev);     
    }
  }
}

static struct mrknlog_device_t * getDevice(JNIEnv *env, jobject object) {
  if (nativeHandleFieldId) {
    jint dev = env->GetIntField(object, nativeHandleFieldId);
    if (dev) {
      return (struct mrknlog_device_t *) dev;
    }  
  }
  throwLibLogException(env, "Not initialized or closed");
  return NULL;
}

static void native_close(JNIEnv *env, jobject object) {
  struct mrknlog_device_t *dev = getDevice(env, object);
  if (dev) {
    dev->common.close((struct hw_device_t *)dev);
    env->SetIntField(object, nativeHandleFieldId, 0);
  }
}

static void flushLog(JNIEnv *env, jobject object) {
  struct mrknlog_device_t *dev = getDevice(env, object);
  if (dev && dev->flush_log(dev) != 0) {
      throwLibLogException(env, "Failed to flush log");
  }
}

static jint getTotalLogSize(JNIEnv *env, jobject object) {
  struct mrknlog_device_t *dev = getDevice(env, object);
  int ret = 0;
  if (dev) {
    ret = dev->get_total_log_size(dev);
    if (ret < 0) {
      throwLibLogException(env, "Failed to get total log size");
    }
  }
  return ret;
}

static jint getUsedLogSize(JNIEnv *env, jobject object) {
  struct mrknlog_device_t *dev = getDevice(env, object);
  int ret = 0;
  if (dev) {
    ret = dev->get_used_log_size(dev);
    if (ret < 0) {
      throwLibLogException(env, "Failed to get used log size");
    }
  }
  return ret;
}

static jboolean waitForLogData(JNIEnv *env, jobject object, jint timeoutInMs) {
  struct mrknlog_device_t *dev = getDevice(env, object);
  int ret = 0;
  if (dev) {
    ret = dev->wait_for_log_data(dev, timeoutInMs);
    if (ret < 0) {
      throwLibLogException(env, "Failed while waiting for log data");
    } 
  }
  return ret > 0 ? JNI_TRUE : JNI_FALSE;
}

static JNINativeMethod method_table[] = {
  { "libInit", "()V", (void *) lib_init},
  { "init", "()V", (void *) native_init },
  { "close", "()V", (void *) native_close },
  { "flushLog", "()V", (void *) flushLog },
  { "getTotalLogSize", "()I", (void *) getTotalLogSize },
  { "getUsedLogSize", "()I", (void *) getUsedLogSize },
  { "waitForLogData", "(I)Z", (void *) waitForLogData }
};


extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env = NULL;
  if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) == JNI_OK) {
    if (jniRegisterNativeMethods(env, class_name, method_table, NELEM(method_table)) == 0) {
      return JNI_VERSION_1_6;
    }
  }
  return JNI_ERR;
}
