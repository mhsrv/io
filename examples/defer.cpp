#include "../async/async.h"
#include "../async/io.h"

io::file output{STDOUT_FILENO};

ASYNC_INIT();

// todo: make sure that async::await() doesn't segfault for completed tasks (probably implement promises)

void test() {
    async::yield();
    std::string test = "Test\n";
    output.write(test);
}


void test2() {
    async::yield();
    std::string test = "Test 2\n";
    output.write(test);
}

int main() {
    async::await(test);
    async::defer(test2);
    std::string main = "Main\n";
    output.write(main);
}