#!/usr/bin/bash

# The MIT License (MIT)
# Copyright (c) 2023 Pablo López Sedeño
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the “Software”), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

if ! [[ "$1" =~ ^[0-9]+$ ]]; then
    echo "ERROR: First argument must be a number"
    exit
fi

i=0
cont=0
while test $cont -eq 0 -a $i -lt 10; do
    i=$(($i+1))
    notify-send --app-name "Mission" "New mission $i" -u critical 2> /dev/null
    ./build/drone_server 14540 14541 14542 14543 14544
    cont=$?
done

if test $cont -eq 0; then
    notify-send --app-name "Mission" "All missions complete" -u critical 2> /dev/null
else
    notify-send --app-name "Mission" "There was an error" -u critical 2> /dev/null
fi