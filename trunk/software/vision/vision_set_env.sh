#!/bin/bash

pushd $(dirname $(readlink -f "$BASH_SOURCE")) > /dev/null
MDA_VISION_SETTINGS_PATH="$PWD/settings/"
popd > /dev/null
echo $SCRIPT_DIR

export MDA_VISION_SETTINGS_PATH
echo "MDA_VISION_SETTINGS_PATH=$MDA_VISION_SETTINGS_PATH"
