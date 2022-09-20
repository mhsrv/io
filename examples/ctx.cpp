#include <cstdlib>
#include <cstdio>
#include <platform_context.h>

void foo(void *test) {
    printf("you called foo %zu\n", (size_t)test);
    exit(1);
}
void test(void*) {
    printf("first\n");
}

int main() {
    char data[4096];
    char data2[4096];
    auto foo_ctx = make_context(prepare_stack(data, sizeof data), foo, reinterpret_cast<void *>(1234), nullptr);
    auto test_ctx = make_context(prepare_stack(data2, sizeof data2), test, nullptr, &foo_ctx);
    set_context(&test_ctx);
}