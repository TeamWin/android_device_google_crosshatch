#!/bin/sh

# Copyright 2018 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

source ../../../common/clear-factory-images-variables.sh
BUILD=4779185
DEVICE=crosshatch
PRODUCT=crosshatch
VERSION=ppr1.180514.001
SRCPREFIX=signed-
BOOTLOADER=b1c1-0.1-4772579
RADIO=g845-00008-180429-B-4752067
source ../../../common/generate-factory-images-common.sh
