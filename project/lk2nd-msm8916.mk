LOCAL_DIR := $(GET_LOCAL_DIR)

include $(LOCAL_DIR)/lk2nd-defines.mk
include $(LOCAL_DIR)/lk1st-msm8916.mk

APPSBOOTHEADER: $(OUTBOOTIMG) $(OUTBOOTIMGADTB) $(OUTODINTAR)
ANDROID_BOOT_BASE := 0x80000000
