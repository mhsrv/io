#include <io.h>

int main(int argc, char **argv) {
    async::init([&] -> io::void_t {
        if (argc != 2) {
            IO_VOID_RETURN(io::console::error.write("Invalid syntax"));
        }

        IO_VOID_RETURN(io::dir::mkdir(argv[1], 0));
    });
}