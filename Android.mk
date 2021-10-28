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


LOCAL_PATH := $(call my-dir)
TARGET_ARCH_ABI := $(APP_ABI)

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Creating prebuilt for dependency: beatsaber-hook - version: 3.3.4
include $(CLEAR_VARS)
LOCAL_MODULE := beatsaber-hook_3_3_4
LOCAL_EXPORT_C_INCLUDES := extern/beatsaber-hook
LOCAL_SRC_FILES := extern/libbeatsaber-hook_2_3_2.so
LOCAL_CPP_FEATURES += exceptions rtti
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: modloader - version: 1.2.3
include $(CLEAR_VARS)
LOCAL_MODULE := modloader
LOCAL_EXPORT_C_INCLUDES := extern/modloader
LOCAL_SRC_FILES := extern/libmodloader.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: custom-json-data - version: 0.14.0
include $(CLEAR_VARS)
LOCAL_MODULE := custom-json-data
LOCAL_EXPORT_C_INCLUDES := extern/custom-json-data
LOCAL_SRC_FILES := extern/libcustom-json-data.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: custom-types - version: 0.12.7
include $(CLEAR_VARS)
LOCAL_MODULE := custom-types
LOCAL_EXPORT_C_INCLUDES := extern/custom-types
LOCAL_SRC_FILES := extern/libcustom-types.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: questui - version: 0.11.1
include $(CLEAR_VARS)
LOCAL_MODULE := questui
LOCAL_EXPORT_C_INCLUDES := extern/questui
LOCAL_SRC_FILES := extern/libquestui.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: tracks - version: 0.3.1
include $(CLEAR_VARS)
LOCAL_MODULE := tracks
LOCAL_EXPORT_C_INCLUDES := extern/tracks
LOCAL_SRC_FILES := extern/libtracks.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: pinkcore - version: 1.6.1
include $(CLEAR_VARS)
LOCAL_MODULE := pinkcore
LOCAL_EXPORT_C_INCLUDES := extern/pinkcore
LOCAL_SRC_FILES := extern/libpinkcore.so
include $(PREBUILT_SHARED_LIBRARY)
# Creating prebuilt for dependency: questui_components - version: 0.1.16
include $(CLEAR_VARS)
LOCAL_MODULE := questui_components
LOCAL_EXPORT_C_INCLUDES := extern/questui_components
LOCAL_SRC_FILES := extern/libquestui_components.a
LOCAL_CPP_FEATURES += rtti
include $(PREBUILT_STATIC_LIBRARY)
# Creating prebuilt for dependency: codegen - version: 0.14.0
include $(CLEAR_VARS)
LOCAL_MODULE := codegen
LOCAL_EXPORT_C_INCLUDES := extern/codegen
LOCAL_SRC_FILES := extern/libcodegen.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := chroma
LOCAL_SRC_FILES += $(call rwildcard,src/,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook,*.cpp)
LOCAL_SRC_FILES += $(call rwildcard,extern/beatsaber-hook/src/inline-hook,*.c)
LOCAL_SHARED_LIBRARIES += modloader
LOCAL_SHARED_LIBRARIES += beatsaber-hook_3_3_4
LOCAL_SHARED_LIBRARIES += custom-json-data
LOCAL_SHARED_LIBRARIES += custom-types
LOCAL_SHARED_LIBRARIES += questui
LOCAL_SHARED_LIBRARIES += tracks
LOCAL_SHARED_LIBRARIES += pinkcore
LOCAL_SHARED_LIBRARIES += codegen
LOCAL_STATIC_LIBRARIES += questui_components
LOCAL_LDLIBS += -llog
LOCAL_CFLAGS += -I'extern/libil2cpp/il2cpp/libil2cpp' -DDEBUGB='1' -DID='"chroma"' -DVERSION='"2.4.10q4"' -I'./shared' -I'./extern' -isystem'extern/codegen/include' -O3
LOCAL_CPPFLAGS += -std=c++2a -O3 -frtti
LOCAL_C_INCLUDES += ./include ./src
LOCAL_CPP_FEATURES += rtti
#LOCAL_STATIC_LIBRARIES += cryptopp
#LOCAL_STATIC_LIBRARIES += curl
LOCAL_EXPORT_C_FLAGS :=
include $(BUILD_SHARED_LIBRARY)
