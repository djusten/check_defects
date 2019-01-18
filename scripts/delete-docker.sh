#!/bin/sh

IMAGE_ID=`docker image ls | grep checkdefect | awk -F ' ' '{print $3}'`
if [[ $? -ne 0 ]] ; then echo "Error $1"; exit 1; fi
docker image rm ${IMAGE_ID}
