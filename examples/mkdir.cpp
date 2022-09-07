#include <cstring>
#include "../async/io.h"
#include "../async/async.h"



ASYNC_ENTRYPOINT(int argc, char **argv) {
    if (argc != 2) {
        std::string str = "Invalid syntax.";
        io::write(STDERR_FILENO, str.c_str(), str.length(), 0);
        return 1;
    }
    auto err = io::mkdir(argv[1], 0);

    if (err) {
        std::string error = std::strerror(err);
        io::write(STDERR_FILENO, error.c_str(), error.length(), 0);
        return 1;
    } else {
        return 0;
    }
}


