#include <io.h>

int main(int argc, char **argv) {
    async::init([&]{
        if (argc != 2) {
            io::console::error.write("Invalid syntax");
            return;
        }

        io::dir::mkdir(argv[1], 0);
    });
}