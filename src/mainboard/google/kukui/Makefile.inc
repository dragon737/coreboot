bootblock-y += boardid.c
bootblock-y += bootblock.c
bootblock-y += chromeos.c
bootblock-y += memlayout.ld
decompressor-y += memlayout.ld

verstage-y += chromeos.c
verstage-y += memlayout.ld

romstage-y += boardid.c
romstage-y += chromeos.c
romstage-y += memlayout.ld
romstage-y += romstage.c

ramstage-y += boardid.c
ramstage-y += chromeos.c
ramstage-y += mainboard.c
ramstage-y += memlayout.ld
