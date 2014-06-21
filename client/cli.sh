#!/bin/bash
#set -x

# Protocol commands:
cmdGetState="get-led-state"
cmdGetColor="get-led-color"
cmdGetFreq="get-led-rate"
cmdSetState="set-led-state"
cmdSetColor="set-led-color"
cmdSetFreq="set-led-rate"

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
    response=$(cat "${fifoResp}")
    if [[ "$response" == OK* ]]; then
        echo "Request successed"
        echo "${response:3:${#response}-3}"
    else
        echo "Request failed"
    fi
}

# Getters #################

function getState() {
    sendAndRecv "$cmdGetState"
}

function getFreq() {
    sendAndRecv "$cmdGetFreq"
}

function getColor() {
    sendAndRecv "$cmdGetColor"
}

# Setters #########
function setState() {
    echo "Switch on or switch off? [1] [0]"
    read keyState
    request="${cmdSetState}"
    if [ "$keyState" == "1" ]; then
        request="${request} on"
    elif [ "$keyState" == "0" ]; then
        request="${request} off"
    else
        promptErr
    fi
    sendAndRecv "${request}"
}

function setFreq() {
    echo "Input frequency in hertz [0..5]"
    read keyFreq
    if ! [[ $keyFreq =~ ^-?[0-9]+$ ]];then
        promptErr
    fi
    if [[ $keyFreq -lt 0 || $keyFreq -gt 5 ]]; then 
        promptErr
    fi
    
    request="${cmdSetFreq} ${keyFreq}"
    sendAndRecv "${request}"
}

function setColor() {
    echo "What color? [red] [green] [blue]"
    read keyColor

    if [[ "$keyColor" != "red" && \
          "$keyColor" != "green" && \
          "$keyColor" != "blue" ]]; then
        promptErr
    fi
    request="${cmdSetColor} ${keyColor}"
    sendAndRecv "${request}"
}

function handler() {
    rm "${fifoReq}"
    rm "${fifoResp}"
    exit
}


function main() {
    trap handler SIGINT
    fifoDir="/tmp/led"
    id=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 16 | head -n 1)
    echo "Session ${id}"
    readonly fifoReq="${fifoDir}/led_pipe_${id}_req"
    readonly fifoResp="${fifoDir}/led_pipe_${id}_resp"
    
    mkdir -p "${fifoDir}"
    mkfifo "${fifoReq}"
    mkfifo "${fifoResp}"
    
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
}

main


