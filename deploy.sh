#!/bin/bash

sudo stm32flash  -w build/firmware.hex -v -g 0x0 /dev/ttyUSB0

ls -lh build/firmware.bin | awk '{print("Firmware size: " $5)}'
