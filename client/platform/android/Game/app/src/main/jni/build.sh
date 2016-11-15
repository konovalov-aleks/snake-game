#!/bin/bash

ARCHITECTURES=('armv7a' 'i686')

BUILD_ROOT=$(pwd)/../../../build/jni
SYMB_DIR=$BUILD_ROOT/symbols
TOOLCHAIN_ROOT=$SBISPlatformSDK_374220/tools/cmake_support/tools/toolchains

mkdir -p $SYMB_DIR
cd $BUILD_ROOT

build()
{
    for arch in ${ARCHITECTURES[*]}
    do
        echo "Building for $arch architecture"

        buildfolder=${BUILD_ROOT}/${arch}

        mkdir -p ${buildfolder}
        cd $arch
        if [[ $arch == armv7a ]]
        then
            target_arch=armeabi-v7a
            toolchain=${TOOLCHAIN_ROOT}/android-toolchain-arm.cmake
        elif [[ $arch == i686 ]]
        then
            target_arch=x86
            toolchain=${TOOLCHAIN_ROOT}/android-toolchain-${arch}.cmake
        fi
    
        if ! cmake3 -DSBIS_OS=Android -DSBIS_ARCHITECTURE=$arch -DSBIS_OPTIMIZATIONS=On -DSBIS_UNIT_TESTS=Off -DCMAKE_TOOLCHAIN_FILE=$toolchain -DSBIS_STRIP_DEBUG_INFO_TO=${SYMB_DIR} -G Ninja ../../../../../../../game-client/
        then
            echo "[CMAKE FAILED]"
            exit 1
        fi
        if ! ninja $@
        then
            echo "[BUILD FAILED]"
            exit 2
        fi
    
        target_dir=$(readlink -m ${BUILD_ROOT}/../../src/main/libs/${target_arch}/)
        mkdir -p ${target_dir}
        if ! cp -v ${buildfolder}/bin/libsnake-game.so ${target_dir} || ! cp -v ${ANDROID_CRYSTAX_NDK}/sources/crystax/libs/${target_arch}/libcrystax.so ${target_dir}
        then
            echo "[COPY FAILED]"
            exit 3
        fi
        cd -
    done
}

clean()
{
    for arch in ${ARCHITECTURES[*]}
    do
        echo "Cleaning $arch"

        buildfolder=${BUILD_ROOT}/${arch}
        if [ -d "$buildfolder" ]
        then
            cd $buildfolder
            ninja clean
            cd -
        fi
    done
}

if [[ $1 == clean ]]
then
    clean
else
    build
fi

cd -
