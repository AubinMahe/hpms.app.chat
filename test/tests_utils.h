#pragma once

#include <stdbool.h>

typedef struct {
   unsigned passed;
   unsigned failed;
   unsigned sum_passed;
   unsigned sum_failed;
} TST_context;

extern const TST_context TST_context_Zero;

void TST_chapter( TST_context * ctxt, const char * title );
void TST_assert( TST_context * ctxt, const char * test, bool observed, bool expected );
void TST_ended( TST_context * ctxt );

#define ASSERT( C, T, E ) TST_assert( C, #T, T, E )
