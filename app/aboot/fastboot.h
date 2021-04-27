/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __APP_FASTBOOT_H
#define __APP_FASTBOOT_H

#define MAX_RSP_SIZE            64
#define MAX_GET_VAR_NAME_SIZE   256

void fastboot_extra_register_commands(void);
void fastboot_lk2nd_register_commands(void);

int fastboot_init(void *xfer_buffer, unsigned max);
void fastboot_stop(void);

/* register a command handler
 * - command handlers will be called if their prefix matches
 * - they are expected to call fastboot_okay() or fastboot_fail()
 *   to indicate success/failure before returning
 */
void fastboot_register(const char *prefix,
		       void (*handle)(const char *arg, void *data, unsigned size));

/* publish a variable readable by the built-in getvar command */
void fastboot_publish(const char *name, const char *value);

/* only callable from within a command handler */
void fastboot_okay(const char *result);
void fastboot_fail(const char *reason);
void fastboot_info(const char *reason);
void fastboot_stage(const void *data, unsigned sz);


#endif
