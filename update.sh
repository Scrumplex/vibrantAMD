#!/bin/sh

usexcb=true

for arg in "$@"; do
	if [ $arg = "--noxcb" ]; then
		usexcb=false
	fi
done

echo "WARNING: This version of vibrantLinux-AMD requires the latest revision of vibrantX as of 29. Apr 2020"

git reset --hard
git pull

if $usexcb; then
	qmake
else
	qmake "DEFINES+=VIBRANT_LINUX_NO_XCB"
fi

make
