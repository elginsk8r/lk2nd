LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LK_TOP_DIR)/platform/msm_shared/include -I$(LK_TOP_DIR)/lib/zlib_inflate

DEFINES += ASSERT_ON_TAMPER=1

MODULES += lib/zlib_inflate

# fs_boot modules:
MODULES += \
	lib/fs \
	lib/bio \
	lib/partition

OBJS += \
	$(LOCAL_DIR)/aboot.o \
	$(LOCAL_DIR)/fastboot.o \
	$(LOCAL_DIR)/fs_boot.o

ifneq ($(DISABLE_RECOVERY_MESSAGES),1)
DEFINES += RECOVERY_MESSAGES=1
OBJS += $(LOCAL_DIR)/recovery.o
endif

ifeq ($(ENABLE_UNITTEST_FW), 1)
OBJS += \
	$(LOCAL_DIR)/fastboot_test.o
	ifeq ($(ENABLE_MDTP_SUPPORT),1)
		OBJS += \
			$(LOCAL_DIR)/mdtp_lk_ut.o
	endif
endif

ifeq ($(ENABLE_MDTP_SUPPORT),1)
OBJS += \
	$(LOCAL_DIR)/mdtp.o \
	$(LOCAL_DIR)/mdtp_ui.o \
	$(LOCAL_DIR)/mdtp_fuse.o \
	$(LOCAL_DIR)/mdtp_defs.o \
	$(LOCAL_DIR)/mdtp_fs.o
endif

OBJS += $(LOCAL_DIR)/fastboot-extra.o

ifneq ($(filter lk2nd,$(ALLMODULES)),)
OBJS += $(LOCAL_DIR)/fastboot-lk2nd.o
endif

ifneq ($(wildcard $(LOCAL_DIR)/fastboot-$(TARGET).c),)
OBJS += $(LOCAL_DIR)/fastboot-$(TARGET).o
endif
