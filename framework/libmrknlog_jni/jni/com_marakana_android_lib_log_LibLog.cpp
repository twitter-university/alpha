#include <mrknlog.h>
#include <jni.h>
#include "JNIHelp.h"

static void ThrowLibLogException(JNIEnv *env, const char *msg) {
  jniThrowException(env, "com/marakana/android/lib/log/LibLogException", msg);
}

static void flushLog(JNIEnv *env, jclass clazz) {
  if (mrkn_flush_log() != 0) {
    ThrowLibLogException(env, "Failed to flush log");
  }
}

static jint getTotalLogSize(JNIEnv *env, jclass clazz) {
  int result = mrkn_get_total_log_size();
  if (result < 0) {
    ThrowLibLogException(env, "Failed to get total log size");
  }
  return result;
}

static jint getUsedLogSize(JNIEnv *env, jclass clazz) {
  int result = mrkn_get_used_log_size();
  if (result < 0) {
    ThrowLibLogException(env, "Failed to get used log size");
  }
  return result;
}

static jboolean waitForLogData(JNIEnv *env, jclass clazz, jint timeoutInMs) {
  int ret = mrkn_wait_for_log_data(timeoutInMs);
  if (ret < 0) {
    ThrowLibLogException(env, "Failed while waiting for log data");
  } 
  return ret > 0 ? JNI_TRUE : JNI_FALSE;
}

static JNINativeMethod method_table[] = {
  { "flushLog", "()V", (void *) flushLog },
  { "getTotalLogSize", "()I", (void *) getTotalLogSize },
  { "getUsedLogSize", "()I", (void *) getUsedLogSize },
  { "waitForLogData", "(I)Z", (void *) waitForLogData }
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
