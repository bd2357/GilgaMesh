#!/bin/bash

set -e

platform='unknown'
unamestr=`uname`
if [[ "$unamestr" == 'Linux' ]]; then
   platform='linux'
elif [[ "$unamestr" == 'Darwin' ]]; then
   platform='osx'
elif [[ "$unamestr" == 'MSYS_NT-10.0' ]]; then
   platform='windows'
else
	echo 'Unknown platform, aborting.'
	exit
fi

if [[ $platform == 'osx' ]]; then
	JLINK=jlinkexe
else
	JLINK=JLinkExe
fi

GCC_ARM_TOOLCHAIN=deploy/gcc-arm-none-eabi/
GCC_ARM_SIZE=$GCC_ARM_TOOLCHAIN/bin/arm-none-eabi-size
BOARD=PCA10028

function compile-root {
    cd _build
    cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DIS_TEST_BUILD=False -DIS_PERIPHERAL=False ..
    make clean && make
    cd -
}

function compile-peripheral {
    cd _build
    cmake -DCMAKE_BUILD_TYPE=MinSizeRel -DIS_TEST_BUILD=False -DIS_PERIPHERAL=True ..
    make clean && make
    cd -
}

function run-tests {
    cd _build
    cmake -DCMAKE_BUILD_TYPE=Debug -DIS_TEST_BUILD=True -DIS_PERIPHERAL=True ..
    make
    cd -

    for TEST in `ls _build/tests/*Test`;
    do
        ./$TEST
    done
}

function reset-device {
    $JLINK deploy/reset-to-factory.jlink
}

function size {
    $GCC_ARM_SIZE _build/MinSizeRel/MeshyMesh.elf
}

function mterm {
    if [ -z "$1" ]
    then
        echo "No device file supplied. e.g. '/dev/cu.usbmodem1411'"
        exit 1
    fi
    echo "To access minicom command options, press ESC + Z"
    minicom --device $1 --b 38400
}

function compile-deploy-root {
    compile-root
    cd deploy
    $JLINK deploy.jlink
    cd -
}

function compile-deploy-peripheral {
    compile-peripheral
    cd deploy
    $JLINK deploy.jlink
    cd -
}

function deploy {
    cd deploy
    $JLINK deploy.jlink
    cd -
}

function compile-full-deploy-root {
    compile-root
    cd deploy
    $JLINK full_deploy.jlink
    cd -
}

function compile-full-deploy-peripheral {
    compile-peripheral
    cd deploy
    $JLINK full_deploy.jlink
    cd -
}

function helptext {
    echo "Usage: ./build.sh <command>"
    echo ""
    echo "Available commands are:"
    echo "    ut                Run all unit tests"
    echo "    cr                Compile for root"
    echo "    cp                Compile for peripheral"
    echo "    dr                Compile and deploy current code to root"
    echo "    dp                Compile and deploy current code to peripheral"
    echo "    d                 Deploy current code to any device"
    echo "    fr                Reset, compile, deploy softdevice, and deploy current code to root"
    echo "    fp                Reset, compile, deploy softdevice, and deploy current code to peripheral"
    echo "    r                 Resets one device to factory settings"
    echo "    m <device file>   Open minicom terminal to specified device at baudrate 38400 (Requires minicom to be installed)"
    echo ""
    echo "Less often used commands..."
    echo "    size              Show the size of the currently compiled firmware"
    echo ""
    echo "Available board is:"
    echo "    PCA10028"
}

case "$1" in
    ut) run-tests
    ;;
    cr) compile-root
    ;;
    cp) compile-peripheral
    ;;
    dr) compile-deploy-root
    ;;
    dp) compile-deploy-peripheral
    ;;
    d) deploy
    ;;
    fr) compile-full-deploy-root
    ;;
    fp) compile-full-deploy-peripheral
    ;;
    r) reset-device
    ;;
    m) mterm "$2"
    ;;
    size) size
    ;;
    *) helptext
    ;;
esac
