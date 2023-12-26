#include <io.h>
#include <unistd.h>

void hello_world() {
    char message[] = "Hello, World!\n";
    io_write(STDOUT_FILENO, message, sizeof message - 1, 0);
}

int main() {
    async_start();
    async_defer(hello_world, NULL, ASYNC_DEFAULT_STACK_SIZE);
    async_init();
}