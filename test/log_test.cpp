#include "../lib/log/log.hpp"

int main() {
    Logger *logger = new StandardLogger;
    Logger::Level *debug = new Debug;
    Logger::Level *info = new Info;
    Logger::Level *warning = new Warning;
    Logger::Level *error = new Error;
    Logger::Level *silence = new Silence;

    logger->write(debug, "Debug");
    logger->write(info, "Info");
    logger->write(warning, "Warning");
    logger->write(error, "Error");
    logger->write(silence, "Silence");
    
    delete debug;
    delete info;
    delete warning;
    delete error;
    delete silence;
}