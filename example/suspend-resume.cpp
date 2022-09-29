#include <io.h>

int main() {
    async::init([] {
        auto* task = async::current_task();
        async::queue_microtask([task] {
            async::resume(task);
        });
        io::console::output.write("About to suspend execution...\n");
        async::suspend();
        io::console::output.write("Got resumed!\n");
    });
}