#/bin/sh

CROSS_COMPILE=arm-none-eabi-
TARGET=yyfish_defconfig
usage(){
	echo "Usage
		sh build.sh config/build/clean/distclean
	"
}
if [ $# -gt 0 ] ; then
	case $1 in
	config)make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE $TARGET;;
	menu)make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE menuconfig;;
	build)make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE;;
	*clean)make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE $1;;
	*)usage;;
	esac
else
	make ARCH=arm CROSS_COMPILE=$CROSS_COMPILE
#	usage
fi
