#!/bin/bash

set -x  # show commands

rm  -rf `pwd`/build/*

cd `pwd`/build && cmake .. && make -j10