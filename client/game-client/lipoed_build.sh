#! /bin/bash

function join_by { local IFS="$1"; shift; echo "$*"; }

archs=( armv7 arm64 i386 x86_64 )

for arch in ${archs[@]}
do
    echo "---Building game-client for ${arch} architecture"
    build_dir=build_${arch}
    [ ! -d ${build_dir} ] && mkdir ${build_dir}
    pushd ${build_dir}
    cmake -DSBIS_LIBS_TYPE=static -DSBIS_OS=ios -DSBIS_ARCHITECTURE=${arch} -DSBIS_OPTIMIZATIONS=1 -DCMAKE_TOOLCHAIN_FILE=${SBISPlatformSDK_374220}/tools/cmake_support/lib/common/core/toolchains/ios.toolchain.clang.cmake.txt -G Ninja ../
    ninja
    popd
done

universal_bin_dir=build_universal/bin
[ ! -d ${universal_bin_dir} ] && mkdir -p ${universal_bin_dir}

libnames=( engine snake-game )

for libname in ${libnames[@]}
do
    echo "---Lipo'ing ${libname} library into ${universal_bin_dir} binary directory"
    arch_paths=
    for arch in ${archs[@]}
    do
        arch_paths="${arch_paths} build_${arch}/bin/lib${libname}.a"
    done
    lipo ${arch_paths} -create -output ${universal_bin_dir}/lib${libname}.a
done
