#!/bin/bash
set -x
fifoDir="/tmp/led"
id=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 16 | head -n 1)
fifoReq="${fifoDir}/led_pipe_${id}_req"
fifoResp="${fifoDir}/led_pipe_${id}_resp"

mkdir -p "${fifoDir}"
mkfifo "${fifoReq}"
mkfifo "${fifoResp}"

echo "${fifoReq}"
echo "${fifoResp}"

echo 'kuku' > "${fifoReq}"
response=`timeout 1s cat "${fifoResp}"
`

#read "${fifoResp}"

#read -t 1 line < "$fifoResp"
#read -t 2 <>"$fifoResp"

echo $response

#rm("${fifoReq}")
#rm("${fifoResp}")


