#include <io.h>

int main() {
    async::init([]() -> utils::void_t {
        auto* task = async::current_task();
        async::queue_microtask([task] {
            async::resume(task);
        });
        TRY(io::console::output.write("About to suspend execution...\n"));
        async::suspend();
        VOID_RETURN(io::console::output.write("Got resumed!\n"));
    });
}