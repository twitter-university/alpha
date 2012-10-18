# Since this file can also be referenced by alpha-sdk_addon 
# we cannot assume LOCAL_PATH points to the directory where
# this file is located. Instead, we create another variable
# to capture this directory.
MY_PATH := $(LOCAL_PATH)/../alpha

# Include all makefiles in sub-directories (one level deep)
include $(call all-subdir-makefiles)

# Enable our custom kernel
LOCAL_KERNEL := $(MY_PATH)/kernel
PRODUCT_COPY_FILES += $(LOCAL_KERNEL):kernel

# Copy our init and ueventd configuration files to the root
# file system (ramdisk.img -> boot.img)
PRODUCT_COPY_FILES += $(MY_PATH)/init.marakanaalphaboard.rc:root/init.marakanaalphaboard.rc
PRODUCT_COPY_FILES += $(MY_PATH)/ueventd.marakanaalphaboard.rc:root/ueventd.marakanaalphaboard.rc

include $(MY_PATH)/packages.mk
