#include "../async/async.h"
#include "../async/io.h"

io::file error{STDERR_FILENO};

ASYNC_INIT();

int main(int argc, char **argv) {
    if (argc != 2) {
        std::string str = "Invalid syntax";
        error.write(str);
        return 1;
    }
    auto err = io::mkdir(argv[1], 0);

    if (err) {
        std::string msg = std::strerror(err);
        error.write(msg);
        return 1;
    } else {
        return 0;
    }
}


