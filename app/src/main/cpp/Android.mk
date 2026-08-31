LOCAL_PATH := $(call my-dir)

# Main native library
include $(CLEAR_VARS)

LOCAL_MODULE    := enimod
LOCAL_SRC_FILES := main.cpp \
                   hooks/aimbot.cpp \
                   hooks/esp.cpp \
                   hooks/norecoil.cpp \
                   hooks/anticheat_bypass.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2
LOCAL_CFLAGS    := -O2 -fvisibility=hidden -fno-stack-protector -DANDROID
LOCAL_CPPFLAGS  := -std=c++17 -fno-rtti -fno-exceptions -fvisibility=hidden

# Position independent code for Android
LOCAL_CFLAGS += -fPIC
LOCAL_CPPFLAGS += -fPIC

include $(BUILD_SHARED_LIBRARY)
