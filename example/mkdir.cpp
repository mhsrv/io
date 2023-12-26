#include <io.h>

int main(int argc, char **argv) {
    async::init([&]() -> utils::void_t {
        if (argc != 2) {
            VOID_RETURN(io::console::error.write("Invalid syntax"));
        }

        VOID_RETURN(io::dir::mkdir(argv[1], 0));
    });
}