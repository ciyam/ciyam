#!/bin/bash

if [ -d test_web_info ]; then
 mv test_web_info/.web* .
 rmdir test_web_info
fi
