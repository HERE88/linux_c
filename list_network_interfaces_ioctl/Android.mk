LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := list_network_interfaces_ioctl.c
LOCAL_SHARED_LIBRARIES :=
LOCAL_CFLAGS += -static
LOCAL_CPPFLAGS +=
LOCAL_MODULE := list_network_interfaces_ioctl
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

