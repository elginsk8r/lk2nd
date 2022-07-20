// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <stdlib.h>
#include <debug.h>
#include <lib/fs.h>

#include <lk2nd/boot.h>

#include "boot.h"

static char abootimg_hdr[] = "ANDROID!";

struct action_abootimg_data {
	char path[128];
};

static enum lk2nd_boot_aboot_action action_abootimg(void *data)
{
	struct action_abootimg_data *adata = data;

	dprintf(INFO, "abootimg: Booting %s\n", adata->path);


	dprintf(CRITICAL, "abootimg: Boot failed!\n");

	return LK2ND_ABOOT_ACTION_FASTBOOT;
}


void action_abootimg_register(char *root)
{
	struct action_abootimg_data *adata;
	struct dirhandle *dirh;
	struct filehandle *fileh;
	struct dirent dirent;
	char path[128], tmp[128], *dot;
	int ret, priority = 25;

	ret = fs_open_dir(root, &dirh);
	if (ret < 0) {
		dprintf(INFO, "fs_open_dir ret = %d\n", ret);
		return;
	}

	while (fs_read_dir(dirh, &dirent) >= 0) {
		dot = strrchr(dirent.name, '.');
		if (!dot || (dot && strcmp(dot, ".img")))
			continue;

		strcpy(path, root);
		strcat(path, "/");
		strcat(path, dirent.name);

		ret = fs_open_file(path, &fileh);
		if (ret < 0) {
			dprintf(INFO, "fs_open_file ret = %d\n", ret);
			continue;
		}

		ret = fs_read_file(fileh, &tmp, 0, 8);
		if (ret == 8 && !strncmp(tmp, abootimg_hdr, 8)) {
			dprintf(INFO, "abootimg: Found %s\n", path);

			if (!strcmp(dirent.name, "boot.img"))
				priority = 50;
			else if (!strcmp(dirent.name, "lk2nd.img"))
				priority = -1;

			adata = malloc(sizeof(*adata));
			strcpy(adata->path, path);
			strcpy(tmp, "image: ");
			strcat(tmp, path);

			lk2nd_boot_add_action(tmp, priority, action_abootimg,
					(void*)adata);
		}

		ret = fs_close_file(fileh);
	}
	fs_close_dir(dirh);
}

