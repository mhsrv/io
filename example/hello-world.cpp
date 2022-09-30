#include <io.h>

int main() {
    async::init([] -> io::void_t {
        IO_VOID_RETURN(io::console::output.write("Hello, World!\n"));
    });
}
