##
## This file is part of the coreboot project.
##
## Copyright (C) 2009 Ronald G. Minnich
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

## include generic winbond pre-ram stage driver
romstage-$(CONFIG_SUPERIO_WINBOND_COMMON_ROMSTAGE) += common/early_init.c

subdirs-y += w83627dhg
subdirs-y += w83627ehg
subdirs-y += w83627hf
subdirs-y += w83627thg
subdirs-y += w83627uhg
subdirs-y += w83667hg-a
subdirs-y += w83697hf
subdirs-y += w83977tf
subdirs-y += wpcd376i
