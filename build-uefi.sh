#!/bin/bash
#

set -e

snb_ivb=("butterfly" "link" "lumpy" "parrot_ivb" "parrot_snb" "stout" "stumpy")
hsw=("falco" "leon" "mccloud" "monroe" "panther" "peppy" "tricky" "wolf" \
	"zako");
bdw=("auron_paine" "auron_yuna" "gandof" "guado" "lulu" "rikku" "samus" \
	"tidus");
byt=("banjo" "candy" "clapper" "enguarde" "glimmer" "gnawty" "heli" \
	"kip" "ninja" "orco" "quawks" "squawks" "sumo" "swanky" "winky");
bsw=("banon" "celes" "cyan" "edgar" "kefka" "reks" "relm" \
	"setzer" "terra" "ultima" "wizpig");
skl=("asuka" "caroline" "cave" "chell" "lars" "sentry")
kbl=("fizz")

if [ -z "$1" ]; then
	build_targets=($(printf "%s " "${hsw[@]}" "${bdw[@]}" "${byt[@]}" "${snb_ivb[@]}" "${bsw[@]}" "${skl[@]}" "${kbl[@]}"));
else
	build_targets=($@)
fi

for device in ${build_targets[@]}
do
	filename="coreboot_tiano-${device}-mrchromebox_`date +"%Y%m%d"`.rom"
	rm -f ~/dev/firmware/${filename}*
	rm -rf ./build
	cp configs/.config.${device}.uefi .config
	make
	cp ./build/coreboot.rom ./${filename}
	smmstore=$(mktemp)
	dd if=/dev/zero bs=256K count=1 | tr '\000' '\377' > ${smmstore}
	cbfstool ${filename} add -f "${smmstore}" -n "smm store" -t raw -a 0x10000
	cbfstool ${filename} print
	sha1sum ${filename} > ${filename}.sha1
	mv ${filename}* ~/dev/firmware/
	if [ "${device}" == "peppy" ]; then
		filename="coreboot_tiano-${device}_elan-mrchromebox_`date +"%Y%m%d"`.rom"
		rm -rf ./build
		sed -i 's/# CONFIG_ELAN_TRACKPAD_ACPI is not set/CONFIG_ELAN_TRACKPAD_ACPI=y/' .config
		make
		cp ./build/coreboot.rom ./${filename}
		cbfstool ${filename} print
		sha1sum ${filename} > ${filename}.sha1
		mv ${filename}* ~/dev/firmware/
	fi
done
