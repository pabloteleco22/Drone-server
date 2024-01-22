/**
 * The MIT License (MIT)
 * Copyright (c) 2023 Pablo López Sedeño
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <gtest/gtest.h>
#include "../src/operation/operation.hpp"

#include <string>

TEST(OperationTools, CheckGetStatusCode) {
    OkCode ok_code;

    OperationTools operation_tools;

    operation_tools.set_failure(ok_code);

    ASSERT_EQ(operation_tools.get_status_code(), ok_code);
}

TEST(OperationTools, CheckCritical) {
    OkCode ok_code;

    OperationTools operation_tools;

    operation_tools.set_failure(ok_code, true);

    ASSERT_EQ(operation_tools.get_status_code(), ok_code);
    ASSERT_TRUE(operation_tools.is_critical());
}

TEST(OperationTools, CheckNotCritical) {
    OkCode ok_code;

    OperationTools operation_tools;

    operation_tools.set_failure(ok_code, false);

    ASSERT_EQ(operation_tools.get_status_code(), ok_code);
    ASSERT_FALSE(operation_tools.is_critical());
}

TEST(OperationTools, CheckDefaultCritical) {
    OkCode ok_code;

    OperationTools operation_tools;

    operation_tools.set_failure(ok_code);

    ASSERT_EQ(operation_tools.get_status_code(), ok_code);
    ASSERT_FALSE(operation_tools.is_critical());
}

TEST(OperationTools, GetDefaultName) {
    OperationTools operation_tools;

    std::string operation_name{ "Hola mundo" };

    operation_tools.set_name(operation_name);

    ASSERT_EQ(operation_name, operation_tools.get_name());
}