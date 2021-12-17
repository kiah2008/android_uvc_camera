#!/bin/bash

export CROSS_ROOT=/home/kiah/worktmp/rootfs-nano
#/home/kiah/worktmp/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu
export CROSS_COMPILE = /home/kiah/worktmp/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin
export CC = ${CROSS_COMPILE}/aarch64-unknown-linux-gnu-gcc 
export CXX = ${CROSS_COMPILE}/aarch64-unknown-linux-gnu-g++ 
export LD = ${CROSS_COMPILE}/aarch64-unknown-linux-gnu-ld 
export AR = ${CROSS_COMPILE}/aarch64-unknown-linux-gnu-ar 
export AS = ${CROSS_COMPILE}/aarch64-unknown-linux-gnu-as 
export RANLIB = ${CROSS_COMPILE}/aarch64-unknown-linux-gnu-ranlib
export NVCC = ${CROSS_ROOT}/usr/local/cuda/bin/nvcc
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SYSROOT/usr/lib/aarch64-linux-gnu:$SYSROOT/lib/aarch64-linux-gnu:$SYSROOT/lib 
export LC_ALL=C 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/kiah/toolchain/gcc-4.8.5-aarch64/aarch64-unknown-linuxgnu/lib64:/home/kiah/toolchain/gcc-4.8.5-aarch64/aarch64-unknown-linux-gnu/sysroot/lib