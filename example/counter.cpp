#include <io.h>

int main() {
    async::init([] {
        for(size_t i = 0; i < 100; i++) {
            async::defer([i] {
                io::console::output.write(std::to_string(i) + "\n");
            });
        }
    });
}
