# Defines a list of languages to be supported by our device
$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_small.mk)

# Defines the rules for building the base Android platform,
# but itself is not specialized for any particular device
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic.mk)

# Discard inherited values and use our own instead.
PRODUCT_NAME := full_marakana_alpha
PRODUCT_DEVICE := alpha
PRODUCT_MODEL := Full Marakana Alpha Image for Emulator

# Include the common definitions and packages
include $(LOCAL_PATH)/common.mk

# Add our device-specific packages
PRODUCT_PACKAGES += MrknLogLibClient 
PRODUCT_PACKAGES += MrknLogServiceClient 
