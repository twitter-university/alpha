#!/bin/bash

# This script is provided to help set up the basic alpha device - assuming it does not already exist.
# Make sure to run it from the root of the Android source directory

mkdir -p device/marakana/alpha

echo 'add_lunch_combo full_marakana_alpha-eng' > device/marakana/alpha/vendorsetup.sh

echo 'PRODUCT_MAKEFILES := $(LOCAL_DIR)/full_alpha.mk' > device/marakana/alpha/AndroidProducts.mk

cat > device/marakana/alpha/full_alpha.mk << "EOF"
# Defines a list of languages to be supported by our device
$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_small.mk)

# Defines the rules for building the base Android platform,
# but itself is not specialized for any particular device
$(call inherit-product, $(SRC_TARGET_DIR)/product/generic.mk)

# Discard inherited values and use our own instead.
PRODUCT_NAME := full_marakana_alpha
PRODUCT_DEVICE := alpha
PRODUCT_MODEL := Full Marakana Alpha Image for Emulator
EOF

cp build/target/board/generic/BoardConfig.mk device/marakana/alpha/.
cp build/target/board/generic/AndroidBoard.mk device/marakana/alpha/.
cp build/target/board/generic/device.mk device/marakana/alpha/.
cp build/target/board/generic/system.prop device/marakana/alpha/.

SIGNER="/C=US/ST=California/L=San Francisco/O=Marakana Inc./OU=Android/CN=Android Platform Signer/emailAddress=android@marakana.com"
rm build/target/product/security/*.p*
echo | development/tools/make_key build/target/product/security/platform "$SIGNER"
echo | development/tools/make_key build/target/product/security/shared "$SIGNER"
echo | development/tools/make_key build/target/product/security/media "$SIGNER"
echo | development/tools/make_key build/target/product/security/testkey "$SIGNER"

echo
echo "You are ready to initiate the build. Run the following: "
echo "source build/envsetup.sh && lunch full_marakana_alpha-eng && time make -j2"


