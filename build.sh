#!/bin/bash

export IDF_PATH=/Users/xiaolisong/Git/esp32/esp-idf

source $IDF_PATH/export.sh

idf.py build

mkdir -p output

cp -rf build/bootloader/bootloader.bin output
cp -rf build/partition_table/partition-table.bin output
cp -rf build/esp32-sense.bin output
cp -rf build/esp32-sense.elf output
cp -rf partitions.csv output