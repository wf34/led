#!/bin/bash
set -e
set -x
fifo="/tmp/led/led_pipe"
echo 'kuku' > "$fifo"

#read line < "$fifo"
#printf "%s\n" "$line"

