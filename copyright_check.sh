#!/bin/bash

echo "------------------------------------------------------------------------------"
echo "Checking for invalid Qualcomm Copyright ..."
echo "------------------------------------------------------------------------------"
rm -rf copyright_check.log copyright_check_count.log copyright_check_fail.log copyright_check_pass.log
grep_common_flags="-nri --exclude=*.log --exclude copyright_check.sh --exclude build.db --exclude-dir=Build"
grep -e "Qualcomm Inc" -e "Qualcomm, Inc" -e "Techologies" -e "QUALCOMM, Technology" -e "QUALCOMM Technology"  * $grep_common_flags >copyright_check.log 
cat copyright_check.log | wc -l >copyright_check_count.log

threshold=1

if [ "$1" = "" ]
then echo no arg
else echo arg is $1
    threshold=$1
fi

value=$(<copyright_check_count.log)

if [ "$value" -gt "$threshold" ]; then
  echo "[FAIL] Invalid copyright count $value is more than $threshold "
  echo "[FAIL] Invalid copyright count $value is more than $threshold " >copyright_check_fail.log
  echo "----------------------------------------------------------------------------"
  echo "............................ Invalid copyright file list  .................."
  echo "----------------------------------------------------------------------------"
  cat  copyright_check.log
  exit -1
else
  echo "[PASS] Current invalid copyright count value is $value and threshold is $threshold"
  echo "[PASS] Cirrent invalid copyright count value is $value and threshold is $threshold" >copyright_check_pass.log
fi
echo "------------------------------------------------------------------------------"
