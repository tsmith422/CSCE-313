#include <threading.h>

void t_init()
{
        // TODO
        // initialize contexts
        for (int i = 0; i < NUM_CTX; ++i)
        {
                contexts[i].state = INVALID;
                memset(&(contexts[i].context), 0, sizeof(ucontext_t));
        }

        // initialize current_context_idx
        current_context_idx = 0;
}

int32_t t_create(fptr foo, int32_t arg1, int32_t arg2)
{
        // TODO
}

int32_t t_yield()
{
        // TODO
}

void t_finish()
{
        // TODO
}
