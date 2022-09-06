#include <iostream>
#include <cstring>
#include "../async/io.h"
#include "../async/async.h"


ASYNC_ENTRYPOINT(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Invalid syntax." << std::endl;
        return 1;
    }

    auto err = io::mkdir(argv[1], 0);

    if (err) {
        std::cerr << std::strerror(err) << "." << std::endl;
        return 1;
    } else {
        return 0;
    }
}


