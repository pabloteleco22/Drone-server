/** Copyright (C) 2023  Pablo López Sedeño
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

    std::string operation_name{"Hola mundo"};

    operation_tools.set_name(operation_name);

    ASSERT_EQ(operation_name, operation_tools.get_name());
}