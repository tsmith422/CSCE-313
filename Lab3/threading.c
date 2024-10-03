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
        for (volatile int empty_idx = 0; empty_idx < NUM_CTX; ++empty_idx)
        {
                // Find empty entry (INVALID state)
                if (contexts[empty_idx].state == INVALID)
                {
                        getcontext(&contexts[empty_idx].context);

                        // Allocate stack??
                        contexts[empty_idx].context.uc_stack.ss_sp = (char *)malloc(STK_SZ);
                        contexts[empty_idx].context.uc_stack.ss_size = STK_SZ;
                        contexts[empty_idx].context.uc_stack.ss_flags = 0;
                        contexts[empty_idx].context.uc_link = NULL;

                        // Modify content
                        makecontext(&contexts[empty_idx].context, (void (*)())foo, 2, arg1, arg2);

                        // Set state so it's ready to be used (VALID)
                        contexts[empty_idx].state = VALID;
                        return 0;
                }
        }

        return 1;
}

int32_t t_yield()
{
        // TODO
        getcontext(&contexts[current_context_idx].context);
        for (int i = 0; i < NUM_CTX; ++i)
        {
                if (contexts[i].state == VALID)
                {
                        int8_t old_current = current_context_idx;
                        current_context_idx = (uint8_t)i;
                        swapcontext(&contexts[old_current].context, &contexts[current_context_idx].context);
                }
        }

        return 0;
}

void t_finish()
{
        // TODO
}
