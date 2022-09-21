#include <io.h>
#include <unistd.h>

int main() {
    async::init([] {
        io::file(STDOUT_FILENO).write("Hello, World!\n");
    });
}
