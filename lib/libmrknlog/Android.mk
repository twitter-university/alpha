LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := libmrknlog.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../include/
LOCAL_SHARED_LIBRARIES := libcutils 
LOCAL_MODULE := libmrknlog
include $(BUILD_SHARED_LIBRARY)
