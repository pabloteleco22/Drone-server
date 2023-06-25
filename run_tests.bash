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

failed_tests=0
file_names=''
files_executed=0

for test in $(find build -regex ".*_\(test\|demo\)$"); do
    test_name=$(basename $test)
    echo -e "\033[1;4;34mRunning \"$test_name\"\033[0m"
    ./$test
    if test $? -ne 0; then
        failed_tests=$[$failed_tests+1]
        if test "$file_names" = ""; then
            file_names=$test_name
        else
            file_names="$file_names\n$test_name"
        fi
    fi
    files_executed=$[$files_executed+1]

    echo ""
done

echo ""
echo -e "\033[1;4;34mSUMMARY\033[0m"
if test $failed_tests -eq 0; then
    echo -e "\033[1;32m$files_executed test files OK\033[0m"
else
    echo -e "\033[1;31mFailed test files: $failed_tests/$files_executed\033[0m"
    echo -e $file_names
fi