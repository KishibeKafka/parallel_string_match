#!/bin/bash

# 修改DATA_PATH为data目录地址
DATA_PATH="/root/ParallelComputing/data"

mkdir -p build && cd build

cmake -DDATA_PATH="${DATA_PATH}" ..

make

./src/matcher