#include <io.h>
#include <unistd.h>

int main() {
    async::init([] {
        for(size_t i = 0; i < 100; i++) {
            async::defer([i] {
                io::file(STDOUT_FILENO).write(std::to_string(i) + "\n");
            });
        }
    });
}
