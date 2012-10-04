LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := com_marakana_android_lib_log_LibLog.cpp
LOCAL_C_INCLUDES += $(JNI_H_INCLUDE) $(LOCAL_PATH)/../../../include/
LOCAL_SHARED_LIBRARIES := libmrknlog libnativehelper
LOCAL_MODULE := libmrknlog_jni
include $(BUILD_SHARED_LIBRARY)
