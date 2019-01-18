#!/bin/sh

PROJECT_DIR=`pwd`
BUILD_DIR=${PROJECT_DIR}/build
SRC_DIR=${PROJECT_DIR}/src

cmakebuild () {
    echo ">>> Creating project..."
    if [ ! -d ${BUILD_DIR} ]; then
        mkdir -p ${BUILD_DIR}
    fi
    cd ${BUILD_DIR} && \
    cmake ${PROJECT_DIR}
}

build () {
    echo ">>> Building..."
    cd ${BUILD_DIR} && \
    make
}

check_src () {
  echo ">>> Checking sources..."
  cd ${SRC_DIR} && \
  cppcheck *.cpp
}

run_app () {
    echo ">>> Running app..."
    cd ${BUILD_DIR} && \
        ./check-defects
}

clean () {
    echo ">>> Cleaning..."
    cd ${BUILD_DIR} 2> /dev/null && \
        make clean
}

deepclean () {
    echo ">>> Erasing build dir..."
    if [ ! -d ${BUILD_DIR} ]; then
        echo "! Build dir does not exist."
        return
    fi
    rm -rf ${BUILD_DIR}
}

invalid() {
  echo "Invalid option: -$1"
}

param() {
  echo "Option -$1 requires an argument"
}

check() {
  if [ $1 != 0 ]; then
    echo "! Error running last command =("
    sleep 1
    exit 1
  fi
}

usage() {
  echo "Usage: $1 [OPTIONS]"
  echo ""
  echo "  -C \t\tCMake Build"
  echo "  -b \t\tBuild"
  echo "  -k \t\tStatic code check"
  echo "  -r \t\tRun app"
  echo "  -c \t\tClean"
  echo "  -d \t\tErase build dir"
  echo ""
}

RUN=0
while getopts ":C :b :k :c :r :d" opt; do
    RUN=1
    case $opt in
        C)
            cmakebuild
        ;;
        b)
            build
        ;;
        k)
            check_src
        ;;
        c)
            clean
        ;;
        d)
            deepclean
        ;;
        r)
            run_app
        ;;
        :)
            param $OPTARG
            RUN=0
        ;;
        \?)
            invalid $OPTARG
            RUN=0
        ;;
    esac
    check $?
done

if [ "$RUN" != "1" ]; then
  usage $0
  exit 1
fi
