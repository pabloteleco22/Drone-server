#include "../flag/flag.hpp"

#include <iostream>

using std::cout;
using std::endl;

int main() {
    cout << "FixedFlag:" << endl;
    FixedFlag fixed_flag;

    cout << (Telemetry::PositionNed)fixed_flag << endl;

    cout << "RandomFlag:" << endl;
    RandomFlag random_flag1;

    cout << random_flag1.get_flag_position() << endl;

    cout << "RandomFlag: {10, 5} {9, 9} {-10, -15}" << endl;
    RandomFlag random_flag2{{10, 5}, {9, 9}, {-10, -15}};

    cout << (std::string)random_flag2 << endl;

    return 0;
}