#include <io.h>

int main() {
    async::init([] {
        io::console::output.write("Hello, World!\n");
    });
}
