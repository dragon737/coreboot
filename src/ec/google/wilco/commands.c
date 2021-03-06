/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <stdint.h>
#include <string.h>

#include "ec.h"
#include "commands.h"

int wilco_ec_get_info(enum get_ec_info_cmd type, char *info)
{
	struct ec_response_get_ec_info rsp;

	if (!info)
		return -1;
	if (wilco_ec_sendrecv(KB_EC_INFO, type, &rsp, sizeof(rsp)) < 0)
		return -1;

	/* Copy returned string */
	strncpy(info, rsp.data, sizeof(rsp.data));
	return 0;
}

void wilco_ec_print_all_info(void)
{
	char info[EC_INFO_MAX_SIZE];

	if (!wilco_ec_get_info(GET_EC_LABEL, info))
		printk(BIOS_INFO, "EC Label      : %s\n", info);

	if (!wilco_ec_get_info(GET_EC_SVN_REV, info))
		printk(BIOS_INFO, "EC Revision   : %s\n", info);

	if (!wilco_ec_get_info(GET_EC_MODEL_NO, info))
		printk(BIOS_INFO, "EC Model Num  : %s\n", info);

	if (!wilco_ec_get_info(GET_EC_BUILD_DATE, info))
		printk(BIOS_INFO, "EC Build Date : %s\n", info);
}

static int wilco_ec_get_power_smi(struct ec_pm_event_state *pm)
{
	struct ec_response_power_smi {
		uint8_t pm_event_1;
		uint8_t pm_state_1;
		uint8_t hotkey;
		uint8_t pm_state_2;
		uint8_t pm_state_3;
		uint8_t pm_state_4;
		uint8_t pm_state_5;
		uint8_t pm_event_2;
		uint8_t pm_state_6;
	} __packed rsp;

	if (!pm)
		return -1;
	if (wilco_ec_sendrecv_noargs(KB_POWER_SMI, &rsp, sizeof(rsp)) < 0)
		return -1;

	pm->event[0] = rsp.pm_event_1;
	pm->event[1] = rsp.pm_event_2;
	pm->state[0] = rsp.pm_state_1;
	pm->state[1] = rsp.pm_state_2;
	pm->state[2] = rsp.pm_state_3;
	pm->state[3] = rsp.pm_state_4;
	pm->state[4] = rsp.pm_state_5;
	pm->state[5] = rsp.pm_state_6;
	pm->hotkey = rsp.hotkey;

	return 0;
}

static int wilco_ec_get_power_status(struct ec_pm_event_state *pm)
{
	struct ec_response_power_status {
		uint8_t pm_state_1;
		uint8_t pm_state_2;
		uint8_t pm_state_3;
		uint8_t pm_state_4;
		uint8_t pm_state_5;
		uint8_t ac_type_lsb;
		uint8_t pm_state_6;
		uint8_t pm_event_2;
		uint8_t ac_type_msb;
	} __packed rsp;

	if (!pm)
		return -1;
	if (wilco_ec_sendrecv_noargs(KB_POWER_STATUS, &rsp, sizeof(rsp)) < 0)
		return -1;

	pm->hotkey = 0;
	pm->event[0] = 0;
	pm->event[1] = rsp.pm_event_2;
	pm->state[0] = rsp.pm_state_1;
	pm->state[1] = rsp.pm_state_2;
	pm->state[2] = rsp.pm_state_3;
	pm->state[3] = rsp.pm_state_4;
	pm->state[4] = rsp.pm_state_5;
	pm->state[5] = rsp.pm_state_6;
	pm->ac_type = rsp.ac_type_msb << 8 | rsp.ac_type_lsb;

	return 0;
}

int wilco_ec_get_pm(struct ec_pm_event_state *pm, bool clear)
{
	if (clear)
		return wilco_ec_get_power_smi(pm);
	else
		return wilco_ec_get_power_status(pm);
}

int wilco_ec_get_lid_state(void)
{
	struct ec_pm_event_state pm;

	if (wilco_ec_get_power_status(&pm) < 0)
		return -1;

	return !!(pm.state[0] & EC_PM1_LID_OPEN);
}

void wilco_ec_slp_en(void)
{
	/* EC does not respond to this command */
	if (wilco_ec_mailbox(WILCO_EC_MSG_NO_RESPONSE,
			     KB_SLP_EN, NULL, 0, NULL, 0) < 0)
		printk(BIOS_ERR, "%s: command failed\n", __func__);
}

void wilco_ec_power_off(enum ec_power_off_reason reason)
{
	/* EC does not respond to this command */
	if (wilco_ec_mailbox(WILCO_EC_MSG_NO_RESPONSE,
			     KB_POWER_OFF, &reason, 1, NULL, 0) < 0)
		printk(BIOS_ERR, "%s: command failed\n", __func__);
}
