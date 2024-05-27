#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "usage: $0 <SWIFTSHADER_INSTALL_DIR> <ENGINE_INSTALL_DIR>"
    exit 1
fi

SWIFTSHADER_INSTALL_DIR="${1//\\//}"
ENGINE_INSTALL_DIR="${2//\\//}"
SMOKE_TEST_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

echo "SWIFTSHADER_INSTALL_DIR: $SWIFTSHADER_INSTALL_DIR"
echo "ENGINE_INSTALL_DIR: $ENGINE_INSTALL_DIR"
echo "SMOKE_TEST_DIR: $SMOKE_TEST_DIR"

cd "$ENGINE_INSTALL_DIR/sample"

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    SWIFTSHADER_LIB=vk_swiftshader.dll
elif [[ "$OSTYPE" == "linux-gnu" ]]; then
    SWIFTSHADER_LIB=libvk_swiftshader.so
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$SWIFTSHADER_INSTALL_DIR:$ENGINE_INSTALL_DIR/sample"
else
  echo "Unknown OS: $OSTYPE"
  exit 1
fi

cat <<EOF > vk_swiftshader_icd.json
{
  "file_format_version": "1.0.0",
  "ICD": {
    "library_path": "$SWIFTSHADER_INSTALL_DIR/$SWIFTSHADER_LIB",
    "api_version": "1.0.5"
  }
}
EOF

export VK_DRIVER_FILES="$ENGINE_INSTALL_DIR/sample/vk_swiftshader_icd.json"
export VK_LAYER_SETTINGS_PATH="$SMOKE_TEST_DIR"

echo "VK_DRIVER_FILES: " $VK_DRIVER_FILES
echo "generated driver manifest:"
cat $VK_DRIVER_FILES

./Sample --run-test --config-path "$SMOKE_TEST_DIR/smoke_test_config.ini"
EXIT_CODE=$?
echo "smoke test exit code: $EXIT_CODE"

if [ -s "ValidationLayers.log" ]; then
  echo "errors detected in ValidationLayers.log"
  exit 1
fi

exit $EXIT_CODE
