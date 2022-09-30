#include <io.h>

int main() {
    async::init([] -> io::void_t {
        auto* task = async::current_task();
        async::queue_microtask([task] {
            async::resume(task);
        });
        IO_TRY(io::console::output.write("About to suspend execution...\n"));
        async::suspend();
        IO_VOID_RETURN(io::console::output.write("Got resumed!\n"));
    });
}