#!/usr/bin/bash

failed_tests=0
file_names=''

for test in $(find build -regex ".*_\(test\|demo\)$"); do
    test="$(cut -d '/' -f2 <<< $test)"
    echo -e "\033[1;4;34mRunning \"$test\"\033[0m"
    ./build/$test
    if test $? -ne 0; then
        failed_tests=$[$failed_tests+1]
        if test "$file_names" = ""; then
            file_names="$(cut -d '/' -f2 <<< $test)"
        else
            file_names="$file_names\n$(cut -d '/' -f2 <<< $test)"
        fi
    fi

    echo ""
done

echo ""
echo -e "\033[1;4;34mSUMMARY\033[0m"
if test $failed_tests -eq 0; then
    echo -e "\033[1;32mAll test files OK\033[0m"
else
    echo -e "\033[1;31mFailed test files: $failed_tests\033[0m"
    echo -e $file_names
fi