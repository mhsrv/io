#include <io.h>
#include <unistd.h>

int main() {
    async::init([] {
        auto* task = async::current_task();
        io::file out(STDOUT_FILENO);
        async::queue_microtask([task] {
            async::resume(task);
        });
        out.write("About to suspend execution...\n");
        async::suspend();
        out.write("Got resumed!\n");
    });
}