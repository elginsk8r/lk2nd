#include <string.h>
#include <debug.h>
#include <platform/efuse.h>
#include <pm8x41_regulator.h>
#include <libfdt.h>
#include <lk2nd-device.h>
#include "fastboot.h"

static void cmd_oem_dump_speedbin(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	int bin, version;

	efuse_read_speed_bin(&bin, &version);

	snprintf(response, sizeof(response),
		 "Speed bin: %d, PVS version: %d (qcom,speed%d-bin-v%d)",
		 bin, version, bin, version);
	fastboot_info(response);
	fastboot_okay("");
}

static void cmd_oem_dump_regulators(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	struct spmi_regulator *vreg;
	for (vreg = target_get_regulators(); vreg->name; ++vreg) {
		snprintf(response, sizeof(response), "%s: enabled: %d, voltage: %d mV",
			 vreg->name, regulator_is_enabled(vreg),
			 regulator_get_voltage(vreg));
		fastboot_info(response);
	}
	fastboot_okay("");
}

#if WITH_DEBUG_LOG_BUF
static void cmd_oem_lk_log(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk_log_getbuf(), lk_log_getsize());
}
#endif

static void cmd_oem_dtb(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk2nd_dev.fdt, fdt_totalsize(lk2nd_dev.fdt));
}

static void cmd_oem_cmdline(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk2nd_dev.cmdline, strlen(lk2nd_dev.cmdline));
}

void fastboot_lk2nd_register_commands(void) {
	fastboot_register("oem dump-speedbin", cmd_oem_dump_speedbin);
	fastboot_register("oem dump-regulators", cmd_oem_dump_regulators);
#if WITH_DEBUG_LOG_BUF
	fastboot_register("oem lk_log", cmd_oem_lk_log);
#endif

	if (lk2nd_dev.fdt)
		fastboot_register("oem dtb", cmd_oem_dtb);

	if (lk2nd_dev.cmdline)
		fastboot_register("oem cmdline", cmd_oem_cmdline);
}
