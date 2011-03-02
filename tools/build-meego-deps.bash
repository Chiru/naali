#!/bin/bash

# Script to build naali and deps
# 
# Installs dependencies needed for building in MeeGo sysroot
# Dependencies needed to run naali in target device stored in cache dir
#
# Requirements: MeeGo sysroot, mic-image-creator (mic2)

if ! test $1; then
	echo "Please provide path to MeeGo sysroot to build against"
	echo "Syntax: ./build-meego-deps.bash <path-to-meego-sysroot>"
	exit 1
else
    sysroot=$1
fi

scripts=meegoScripts

# Copy build script and spec files to sysroot and execute them in rooted environment
sudo cp -r $scripts $sysroot/
sudo mic-chroot $sysroot -e "su -c ./meegoScripts/chrootbuild"

# Copy build dependencies and binaries to working directory
sudo cp $sysroot/packages/naali-deps.tar.gz . && echo "Built dependencies can be found from ./naali-deps.tar.gz"
sudo cp $sysroot/packages/naali.tar.gz . && echo "Built binaries can be found from ./naali.tar.gz"
