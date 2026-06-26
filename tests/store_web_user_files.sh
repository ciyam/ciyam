#!/bin/bash

if [ -f .web_device_admin ]; then
 mkdir -p test_web_info
 mv .web* test_web_info
fi
