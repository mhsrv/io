#include <io.h>
#include <unistd.h>

int main() {
    std::string message{"Hello, World!\n"};

    async::defer([&message] {
        io::file(STDOUT_FILENO).write(message);
    });

    async::init();
}
