/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 HardenedLinux.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stddef.h>
#include <arch/encoding.h>
#include <arch/smp/smp.h>
#include <arch/smp/spinlock.h>
#include <mcall.h>
#include <commonlib/compiler.h>
#include <console/console.h>

void smp_pause(int working_hartid)
{
#define SYNCA (OTHER_HLS(working_hartid)->entry.sync_a)
#define SYNCB (OTHER_HLS(working_hartid)->entry.sync_b)

	int hartid = read_csr(mhartid);

	if (hartid != working_hartid) {
		/* waiting for work hart */
		do {
			barrier();
		} while (SYNCA != 0x01234567);

		clear_csr(mstatus, MSTATUS_MIE);
		write_csr(mie, MIP_MSIP);

		/* count how many cores enter the halt */
		__sync_fetch_and_add(&SYNCB, 1);

		do {
			barrier();
			__asm__ volatile ("wfi");
		} while ((read_csr(mip) & MIP_MSIP) == 0);
		set_msip(hartid, 0);
		HLS()->entry.fn(HLS()->entry.arg);
	} else {
		/* Initialize the counter and
		 * mark the work hart into smp_pause */
		SYNCB = 0;
		SYNCA = 0x01234567;

		/* waiting for other Hart to enter the halt */
		do {
			barrier();
		} while (SYNCB + 1 < CONFIG_RISCV_HART_NUM);

		/* initialize for the next call */
		SYNCA = 0;
		SYNCB = 0;
	}
#undef SYNCA
#undef SYNCB
}

void smp_resume(void (*fn)(void *), void *arg)
{
	int hartid = read_csr(mhartid);

	if (fn == NULL)
		die("must pass a non-null function pointer\n");

	for (int i = 0; i < CONFIG_RISCV_HART_NUM; i++) {
		OTHER_HLS(i)->entry.fn = fn;
		OTHER_HLS(i)->entry.arg = arg;
	}

	for (int i = 0; i < CONFIG_RISCV_HART_NUM; i++)
		if (i != hartid)
			set_msip(i, 1);

	HLS()->entry.fn(HLS()->entry.arg);
}
