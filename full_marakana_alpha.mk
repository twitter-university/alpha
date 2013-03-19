# Inherit from the emulator product, which defines the base OS
$(call inherit-product, $(SRC_TARGET_DIR)/product/full.mk)

# Discard inherited values and use our own instead
PRODUCT_NAME := full_marakana_alpha
PRODUCT_DEVICE := alpha
PRODUCT_MODEL := Full Marakana Alpha Image for Emulator

# Include the common definitions and packages
include $(LOCAL_PATH)/common.mk

# Enable overlays
DEVICE_PACKAGE_OVERLAYS := $(LOCAL_PATH)/overlay

# Add our device-specific packages
PRODUCT_PACKAGES += MrknLogNative
PRODUCT_PACKAGES += MrknLogClient
