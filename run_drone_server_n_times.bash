#!/usr/bin/bash

i=0
cont=0
while test $cont -eq 0 -a $i -lt 10; do
    i=$(($i+1))
    notify-send --app-name "Mission" "New mission $i" -u critical
    ./build/drone_server 14540 14541 14542 14543 14544
    cont=$?
done

if test $cont -eq 0; then
    notify-send --app-name "Mission" "All missions complete" -u critical
else
    notify-send --app-name "Mission" "There was an error" -u critical
fi