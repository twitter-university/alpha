LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := mrknlogd.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../include/
LOCAL_SHARED_LIBRARIES := libcutils libhardware
LOCAL_CFLAGS += -g -O0
LOCAL_MODULE := mrknlogd
include $(BUILD_EXECUTABLE)

