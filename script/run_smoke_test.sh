#!/bin/bash

ICD_FILE_PATH="$1"

export VK_ICD_FILENAMES="$ICD_FILE_PATH=vk_swiftshader_icd.json"

echo "VK_ICD_FILENAMES: $VK_ICD_FILENAMES"

./Sample.exe --run-test > "./SampleResults.txt" 2>&1
EXIT_CODE=$?
echo $EXIT_CODE >> "./SampleResults.txt"
exit $EXIT_CODE
