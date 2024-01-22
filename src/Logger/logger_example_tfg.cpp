#define EX3

#ifdef EX1
#include "loggerbuilder.hpp"

int main() {
    using namespace simple_logger;

    Debug debug;
    Info info;
    Warning warning;
    Error error;

    StandardLoggerBuilder logger_builder;
    Logger *standard_logger{ logger_builder.build() };

    standard_logger->write(debug, "Mensaje de depuración");
    standard_logger->write(info, "Mensaje de información");
    standard_logger->write(warning, "Mensaje de aviso");
    standard_logger->write(error, "Mensaje de error");

    delete standard_logger;

    return 0;
}
#elif defined(EX2)
#include "loggerbuilder.hpp"
#include <fstream>

int main() {
    using namespace simple_logger;
    using std::ostream;
    using std::fstream;
    using std::ios;

    Debug debug;
    Info info;
    Warning warning;
    Error error;

    HourLoggerDecoration decoration;
    std::fstream out_stream{ "registro.log", ios::out };
    StreamLoggerBuilder logger_builder{ &out_stream };
    logger_builder.set_decoration(&decoration);

    Logger *stream_logger{ logger_builder.build() };

    stream_logger->write(debug, "Mensaje de depuración");
    stream_logger->write(info, "Mensaje de información");
    stream_logger->write(warning, "Mensaje de aviso");
    stream_logger->write(error, "Mensaje de error");

    delete stream_logger;

    return 0;
}
#elif defined(EX3)
#include "loggerbuilder.hpp"
#include <fstream>
#include <string>

int main() {
    using namespace simple_logger;
    using std::ostream;
    using std::fstream;
    using std::ios;

    Debug debug;
    Info info;
    Warning warning;
    Error error;

    TimedLoggerDecoration time_decoration;

    HourLoggerDecoration hour_decoration;

    UserCustomGreeter custom_greeter{
        [&hour_decoration](const string &) {
            string message{"Logger empezado a las " +
                hour_decoration.get_decoration()};

            // Para eliminar el separador que imprime
            // hour_decoration al final
            message.pop_back();
            message.pop_back();

            return message;
        }
    };

    std::fstream out_stream{ "registro.log", ios::out };

    StandardLoggerBuilder standard_logger_builder;
    StreamLoggerBuilder stream_logger_builder{ &out_stream };

    standard_logger_builder.set_decoration(&hour_decoration)
        .set_greeter(&custom_greeter);

    stream_logger_builder.set_decoration(&time_decoration)
        .set_greeter(&custom_greeter);

    Logger *standard_logger{ standard_logger_builder.build() };
    Logger *stream_logger{ stream_logger_builder.build() };
    BiLogger bi_logger{ standard_logger, stream_logger };

    bi_logger.write(debug, "Mensaje de depuración");
    bi_logger.write(info, "Mensaje de información");
    bi_logger.write(warning, "Mensaje de aviso");
    bi_logger.write(error, "Mensaje de error");

    delete standard_logger;
    delete stream_logger;

    return 0;
}
#elif defined(EX4)
#include "loggerbuilder.hpp"

int main() {
    using namespace simple_logger;

    Debug debug;
    Info info;
    Warning warning;
    Error error;

    StandardLoggerBuilder logger_builder;
    Logger *standard_logger{ logger_builder.build() };

    UserCustomFilter custom_filter{
        [&debug, &warning](const Level &level) {
            return ((level == debug) || (level >= warning));
        }
    };

    standard_logger->set_level_filter(&custom_filter);

    standard_logger->write(debug, "Mensaje de depuración");
    standard_logger->write(info, "Mensaje de información");
    standard_logger->write(warning, "Mensaje de aviso");
    standard_logger->write(error, "Mensaje de error");

    delete standard_logger;

    return 0;
}
#endif