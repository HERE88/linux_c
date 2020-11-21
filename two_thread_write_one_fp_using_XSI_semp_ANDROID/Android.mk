LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS               := optional
LOCAL_SRC_FILES                 := two_thread_write_one_fp_using_XSI_semp.c
LOCAL_C_INCLUDES                :=
LOCAL_SHARED_LIBRARIES          := libcutils
#LOCAL_LDLIBS                    += -lpthread
LOCAL_CFLAGS                    +=

LOCAL_MODULE                    := test_XSI
include $(BUILD_EXECUTABLE)


