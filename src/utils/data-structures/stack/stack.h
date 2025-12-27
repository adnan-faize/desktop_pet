/**
 * @file stack.h
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

typedef struct _stack_ds_t stack_ds_t;
struct _stack_ds_t {
    void **data;
};

extern stack_ds_t stack_create();

// attach and detach

extern bool stack_insert(stack_ds_t stack, void *data);
extern bool stack_remove(stack_ds_t stack);
