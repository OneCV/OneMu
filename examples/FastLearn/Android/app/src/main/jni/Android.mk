# Copyright (C) 2009 The Android Open Source Project
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
#
LOCAL_PATH := $(call my-dir)

#load prebuilt lib
include $(CLEAR_VARS)
LIB_PATH := $(LOCAL_PATH)/cv/lib/android
LOCAL_MODULE    := cv-prebuilt
LOCAL_SRC_FILES := $(LIB_PATH)/libOneCV.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/cv/include
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LIB_PATH := $(LOCAL_PATH)/cv/lib/android
LOCAL_MODULE    := cvapp-prebuilt
LOCAL_SRC_FILES := $(LIB_PATH)/libOneCVAPP.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/cv/include
include $(PREBUILT_SHARED_LIBRARY)

#build jni shared library
include $(CLEAR_VARS)
LOCAL_MODULE    := OneDetectorJni
LOCAL_SRC_FILES := OneDetectorJni.c
LOCAL_SHARED_LIBRARIES := cv-prebuilt
LOCAL_SHARED_LIBRARIES += cvapp-prebuilt
LOCAL_LDLIBS := -llog -lm
LOCAL_C_INCLUDES := $(LOCAL_PATH)/cv/include
include $(BUILD_SHARED_LIBRARY)


