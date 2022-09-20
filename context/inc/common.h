#include <stddef.h>

typedef struct stack {
    char *sp;
    size_t stack_length;
}context_stack_t;

extern void get_context(native_context_t *c);
extern void set_context(native_context_t *c);
extern void swap_context(native_context_t *a, native_context_t *b);
extern context_stack_t prepare_stack(char *mem, size_t stack_length);
typedef void context_fn(void *args);
native_context_t make_context(context_stack_t stack, context_fn *fn, void *data, native_context_t* link);
