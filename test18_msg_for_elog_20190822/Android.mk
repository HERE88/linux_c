LOCAL_PATH := $(call my-dir)

# --------------------------------------
include $(CLEAR_VARS)
LOCAL_SRC_FILES := test18.c rio_socket.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SHARED_LIBRARIES := libcutils libutils
LOCAL_CFLAGS += -static
#LOCAL_CFLAGS += -DOPT_FOR_ANDROID
LOCAL_MODULE := test18
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)


