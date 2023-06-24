#!/usr/bin/bash

# Copyright (C) 2023  Pablo López Sedeño
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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