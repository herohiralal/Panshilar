#!/bin/bash
set -euo pipefail

if [ "$1" == "-firsttime" ]; then
    source ./setup.sh
fi

source ./build-libs.sh
source ./build-tools.sh
