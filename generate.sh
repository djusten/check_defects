#!/bin/sh

PROJECT_DIR=`pwd`
BUILD_DIR=${PROJECT_DIR}/build
SRC_DIR=${PROJECT_DIR}/src

SUDO=''
GIT="/usr/bin/git"

PACKAGES="build-essential git cmake cppcheck autoconf intltool gtk-doc-tools libgtk2.0-dev libxml2-dev"

prepare_libaravis () {
  echo ">>> Preparing libaravis"

  ${GIT} clone https://github.com/AravisProject/aravis.git --branch ARAVIS_0_6_1 && \
  if [[ $? -ne 0 ]] ; then echo "Error $1"; exit 1; fi

  cd aravis && \
    ./autogen.sh && \
    make -j4

  ${SUDO} make install
  ${SUDO} ldconfig

  cd ..
  rm -rf aravis
}

prepare () {
  echo ">>> Preparing environment..."

  USER=`id -u`

  if [ "${USER}" -ne 0 ]; then
    echo "! This preparation needs to run as root"
    SUDO=sudo
  fi

  echo "This preparation will install the following packages and its dependencies:"
  echo "${PACKAGES}"
  echo -n "Do you agree? (Y/n) "

  read opt
  echo

  if [ "${opt}" != "n" ]; then
    ${SUDO} apt-get update && \
      ${SUDO} apt-get -y install ${PACKAGES}
  fi

  prepare_libaravis
}

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
  echo "  -p Prepare environment (requires sudo)"
  echo "  -C CMake Build"
  echo "  -b Build"
  echo "  -k Static code check"
  echo "  -r Run app"
  echo "  -c Clean"
  echo "  -d Erase build dir"
  echo ""
}

RUN=0
while getopts ":p :C :b :k :c :r :d" opt; do
    RUN=1
    case $opt in
        p)
            prepare
        ;;
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
