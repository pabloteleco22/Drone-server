#include "../lib/log/log.hpp"

#include <iostream>

using std::cout;
using std::endl;
using std::shared_ptr;
using std::make_shared;

int main() {
    //shared_ptr<Logger> logger{make_shared<StandardLogger>(new StandardLogger)};
    shared_ptr<Logger> logger{make_shared<StandardLogger>(new StandardLogger)};

    shared_ptr<Level> level{make_shared<Level>(new Level)};
    shared_ptr<Level> debug{make_shared<Debug>(new Debug)};
    shared_ptr<Level> info{make_shared<Info>(new Info)};
    shared_ptr<Level> warning{make_shared<Warning>(new Warning)};
    shared_ptr<Level> error{make_shared<Error>(new Error)};
    shared_ptr<Level> silence{make_shared<Silence>(new Silence)};

    cout << "All levels" << endl;
    logger->write(level, "Level");
    logger->write(debug, "Debug");
    logger->write(info, "Info");
    logger->write(warning, "Warning");
    logger->write(error, "Error");
    logger->write(silence, "Silence");

    cout << "Min Warning level" << endl;
    logger->set_min_level(warning);

    logger->write(level, "Level");
    logger->write(debug, "Debug");
    logger->write(info, "Info");
    logger->write(warning, "Warning");
    logger->write(error, "Error");
    logger->write(silence, "Silence");

   return 0;
}