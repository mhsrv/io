#include <io.h>

int main() {
    async::init([] -> utils::void_t {
        VOID_RETURN(io::console::output.write("Hello, World!\n"));
    });
}
