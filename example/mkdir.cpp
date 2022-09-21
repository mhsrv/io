#include <io.h>
#include <unistd.h>

io::file error{STDERR_FILENO};

int main(int argc, char **argv) {
    async::defer([&]{
        if (argc != 2) {
            std::string str = "Invalid syntax";
            error.write(str);
            return;
        }
        auto err = io::mkdir(argv[1], 0);

        if (err) {
            std::string msg = std::strerror(err);
            error.write(msg);
            return;
        } else {
            return;
        }
    });
    async::init();
}