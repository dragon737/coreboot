/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Tristan Corrick <tristan@corrick.kiwi>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

DefinitionBlock(
	"dsdt.aml",
	"DSDT",
	0x02,		/* DSDT Revision: ACPI v3.0 */
	"COREv4",	/* OEM ID */
	"COREBOOT",	/* OEM Table ID */
	0x20181031	/* OEM Revision */
)
{
	#include "acpi/platform.asl"
	#include <southbridge/intel/lynxpoint/acpi/platform.asl>
	#include <southbridge/intel/lynxpoint/acpi/globalnvs.asl>
	#include <southbridge/intel/lynxpoint/acpi/sleepstates.asl>
	#include <cpu/intel/haswell/acpi/cpu.asl>

	Scope (\_SB)
	{
		Device (PCI0)
		{
		#include <northbridge/intel/haswell/acpi/haswell.asl>
		#include <southbridge/intel/lynxpoint/acpi/pch.asl>
		#include <drivers/intel/gma/acpi/default_brightness_levels.asl>
		}
	}
}
