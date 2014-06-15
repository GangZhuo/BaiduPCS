#!/bin/bash
VER=`git describe --tags`
if git status | grep -q "modified:" ; then
    VER="${VER}M"
fi
echo "Version: $VER"

rm -f ./version.h
cat ./version.h.in | sed "s/\$GIT_VERSION/$VER/g" > ./version.h
#echo $GIT_VERSION
echo "Generated version.h"