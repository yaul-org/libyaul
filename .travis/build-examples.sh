#!/bin/bash -e

pwd

set -x

if ! [ -f "yaul.env" ]; then
    exit 1
fi

. ./yaul.env

make clean-examples
make examples
