/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Device (EC0)
{
	Name (_HID, EisaId ("PNP0C09"))
	Name (_UID, 1)
	Name (_GPE, EC_SCI_GPI)
	Name (_STA, 0xf)

	Name (_CRS, ResourceTemplate() {
		IO (Decode16,
		    CONFIG_EC_BASE_ACPI_DATA,
		    CONFIG_EC_BASE_ACPI_DATA,
		    4, 4)
		IO (Decode16,
		    CONFIG_EC_BASE_ACPI_COMMAND,
		    CONFIG_EC_BASE_ACPI_COMMAND,
		    4, 4)
	})

	/* Handle registration of EmbeddedControl region */
	Name (EREG, Zero)
	OperationRegion (ERAM, EmbeddedControl, 0, 0xff)
	Method (_REG, 2)
	{
		/* Indicate region is registered */
		EREG = Arg1

		/* Store initial value for power status */
		ECPR = R (APWR)

		/* Indicate to EC that OS is ready for queries */
		W (ERDY, One)

		/* Tell EC to stop emulating PS/2 mouse */
		W (PS2M, Zero)
	}

	/*
	 * Find bitmask for field
	 *  Arg0 = EC field structure
	 *  Arg1 = Value
	 */
	Method (EBIT, 2, NotSerialized)
	{
		Local0 = DeRefOf (Arg0[1])	/* Mask */
		Local1 = Arg1 & Local0
		FindSetRightBit (Local0, Local2)
		If (Local2) {
			Local1 >>= Local2 - 1
		}
		Return (Local1)
	}

	/*
	 * READ or WRITE from EC region
	 *  Arg0 = EC field structure
	 *  Arg1 = Value to write
	 */
	Method (ECRW, 2, Serialized)
	{
		If (!EREG) {
			Return (Zero)
		}

		Local0 = DeRefOf (Arg0[0])	/* Byte offset */
		Local1 = DeRefOf (Arg0[1])	/* Mask */
		Local2 = DeRefOf (Arg0[2])	/* Read/Write */

		OperationRegion (ERAM, EmbeddedControl, Local0, 2)
		Field (ERAM, ByteAcc, Lock, WriteAsZeros)
		{
			BYT1, 8,
			BYT2, 8,
		}

		If (Local2 == RD) {
			/* Read first byte */
			Local3 = BYT1

			/* Read second byte if needed */
			FindSetLeftBit (Local1, Local4)
			If (Local4 > 8) {
				Local4 = BYT2
				Local4 <<= 8
				Local3 |= Local4
			}

			Local5 = EBIT (Arg0, Local3)
			Printf ("ECRD %o = %o", Local0, Local5)
			Return (Local5)
		} ElseIf (Local2 == WR) {
			/* Write byte */
			Printf ("ECWR %o = %o", Local0, Arg1)
			BYT1 = Arg1
		}
		Return (Zero)
	}

	/*
	 * Read a field from EC
	 *  Arg0 = EC field structure
	 */
	Method (R, 1, Serialized)
	{
		Return (ECRW (Arg0, Zero))
	}

	/*
	 * Write value to a field from EC
	 *  Arg0 = EC field structure
	 *  Arg1 = Value to write
	 */
	Method (W, 2, Serialized)
	{
		Return (ECRW (Arg0, Arg1))
	}

	#include "ec_dev.asl"
	#include "ec_ram.asl"
	#include "ac.asl"
	#include "battery.asl"
	#include "event.asl"
	#include "lid.asl"
	#include "platform.asl"
}
