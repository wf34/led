#!/bin/bash
set -x

cmdGetState="get-led-state"
cmdGetColor="get-led-color"
cmdGetFreq="get-led-rate"

function promptErr() {
    echo "Wrong input"
    continue
}

function switchVars() {
    stateHandler=$1
    freqHandler=$2
    colorHandler=$3
    echo "What are you interested at? State, Freq, Color? [S] [F] [C]"
    read key
    if [ "$key" == "S" ]; then
        $stateHandler 
    elif [ "$key" == "F" ]; then
        $freqHandler
    elif [ "$key" == "C" ]; then
        $colorHandler
    else
        promptErr
    fi
}

function sendAndRecv() {
    request=$1
    echo "$request" > "${fifoReq}"
    response=$(timeout 1s cat "${fifoResp}")
    echo $response
}

function getState() {
    sendAndRecv "$cmdGetState"
}

function getFreq() {
    sendAndRecv "$cmdFreqState"
}

function getColor() {
    sendAndRecv "$cmdColorState"
}


fifoDir="/tmp/led"
id=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 16 | head -n 1)
fifoReq="${fifoDir}/led_pipe_${id}_req"
fifoResp="${fifoDir}/led_pipe_${id}_resp"

mkdir -p "${fifoDir}"
mkfifo "${fifoReq}"
mkfifo "${fifoResp}"

echo "${fifoReq}"
echo "${fifoResp}"


while true
do
    echo "Get or Set? [G] [S]"
    read keyGS
    if [ "$keyGS" == "G" ]; then
        switchVars getState getFreq getColor
    elif [ "$keyGS" == "S" ]; then
        switchVars setState setFreq setColor
    else
        promptErr
    fi
done




