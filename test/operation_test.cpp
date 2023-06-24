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