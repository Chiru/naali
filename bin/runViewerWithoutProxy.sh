#!/bin/bash

echo "Disabling up proxy"
rm -f $HOME/.realXtend/configuration/Asset.xml

echo "Removing assetcache"
rm -rf $HOME/.realXtend/assetcache/*

echo "Running viewer"
./viewer "$@"

