#include <io.h>
#include <unistd.h>

io::file error{STDERR_FILENO};

int main(int argc, char **argv) {
    async::init([&]{
        if (argc != 2) {
            error.write("Invalid syntax");
            return;
        }
        auto err = io::mkdir(argv[1], 0);

        if (err) {
            std::string msg = std::strerror(err);
            error.write("Invalid syntax");
        }
    });
}