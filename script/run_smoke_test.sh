#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "usage: $0 <ICD_FILE_PATH> <INSTALL_DIR> <OUT_FILE>"
    exit 1
fi

SWIFTSHADER_INSTALL_DIR="$1"
ENGINE_INSTALL_DIR="$2"
OUT_FILE="$3"

echo "SWIFTSHADER_INSTALL_DIR: $SWIFTSHADER_INSTALL_DIR"
echo "ENGINE_INSTALL_DIR: $ENGINE_INSTALL_DIR"
echo "OUT_FILE: $OUT_FILE"

cd "$ENGINE_INSTALL_DIR/sample"
cp "$SWIFTSHADER_INSTALL_DIR/vk_swiftshader_icd.json" .
#cp "$SWIFTSHADER_INSTALL_DIR/vk_swiftshader_driver.json" .
cp "$SWIFTSHADER_INSTALL_DIR/vk_swiftshader.dll" .
export VK_ICD_FILENAMES="./vk_swiftshader_icd.json"
#export VK_DRIVER_FILES=c:/dev/NcEngine/install/swiftshader/vk_swiftshader_driver.json

echo $VK_ICD_FILENAMES
cat $VK_ICD_FILENAMES
#echo $VK_DRIVER_FILES
#cat $VK_DRIVER_FILES

# cp "$SWIFTSHADER_INSTALL_DIR/vulkan-1.dll" .

./Sample --run-test > "$OUT_FILE" 2>&1
EXIT_CODE=$?
echo $EXIT_CODE
exit $EXIT_CODE
