LOCAL_PATH := $(call my-dir)

# --------------------------------------
include $(CLEAR_VARS)
LOCAL_SRC_FILES := test_main.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SHARED_LIBRARIES := libcutils libutils
LOCAL_CFLAGS += -static
#LOCAL_CFLAGS += -DOPT_FOR_ANDROID
LOCAL_MODULE := test_main
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)


# --------------------------------------
include $(CLEAR_VARS)
LOCAL_SRC_FILES := test_main2.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_SHARED_LIBRARIES := libcutils libutils
LOCAL_CFLAGS += -static
#LOCAL_CFLAGS += -DOPT_FOR_ANDROID
LOCAL_MODULE := test_main2
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)