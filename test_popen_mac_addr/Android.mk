LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS               := optional
LOCAL_SRC_FILES                 := main.c
LOCAL_C_INCLUDES                :=
LOCAL_SHARED_LIBRARIES          := libcutils
#LOCAL_LDLIBS                    += -lpthread
LOCAL_CFLAGS                    +=

LOCAL_MODULE                    := test_popen_mac
include $(BUILD_EXECUTABLE)


