#include "tests_utils.h"
#include "iso6429.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEAVY_CHECK_MARK      "\u2714"
#define HEAVY_BALLOT_X        "\u2718"

#define TEST_PASSED           ISO_6429_FG_GREEN HEAVY_CHECK_MARK ISO_6429_RESET
#define TEST_FAILED           ISO_6429_FG_RED   HEAVY_BALLOT_X   ISO_6429_RESET

const TST_context TST_context_Zero = { 0, 0, 0, 0 };

void TST_chapter( TST_context * ctxt, const char * title ) {
   if( ctxt->failed || ctxt->passed || ctxt->sum_failed || ctxt->sum_passed ) {
      ctxt->sum_failed += ctxt->failed;
      ctxt->sum_passed += ctxt->passed;
      if( ctxt->failed == 0 ) {
         printf( "   %d tests, %sall successful%s\n", ctxt->passed, ISO_6429_FG_GREEN, ISO_6429_RESET );
      }
      else if( ctxt->passed == 0 ) {
         printf( "   %d tests, %sall failed%s\n", ctxt->failed, ISO_6429_FG_RED, ISO_6429_RESET );
      }
      else if( ctxt->failed > 0 ) {
         printf( "   %d tests, %s%d successful%s, %s%d failed%s\n", ctxt->passed + ctxt->failed,
            ISO_6429_FG_GREEN, ctxt->passed, ISO_6429_RESET,
            ISO_6429_FG_RED  , ctxt->failed, ISO_6429_RESET );
      }
   }
   ctxt->failed = 0;
   ctxt->passed = 0;
   if( title ) {
      printf( "\n%s%s%s\n\n", ISO_6429_BOLD, title, ISO_6429_RESET );
   }
}

void TST_assert( TST_context * ctxt, const char * test, bool observed, bool expected ) {
   if( observed == expected ) {
      printf( "   %s %s\n", TEST_PASSED, test );
      ++(ctxt->passed);
   }
   else {
      printf( "   %s %s\n", TEST_FAILED, test );
      ++(ctxt->failed);
   }
}

void TST_ended( TST_context * ctxt ) {
   TST_chapter( ctxt, NULL );
   if( ctxt->sum_failed == 0 ) {
      printf( "\n%s%d tests, %sall successful%s\n", ISO_6429_BOLD, ctxt->sum_passed, ISO_6429_FG_GREEN, ISO_6429_RESET );
   }
   else if( ctxt->sum_passed == 0 ) {
      printf( "\n%s%d tests, %sall failed%s\n", ISO_6429_BOLD, ctxt->sum_failed, ISO_6429_FG_RED, ISO_6429_RESET );
   }
   else if( ctxt->sum_failed > 0 ) {
      printf( "\n%s%d tests, %s%d successful%s, %s%d failed%s\n", ISO_6429_BOLD, ctxt->sum_passed + ctxt->sum_failed,
         ISO_6429_FG_GREEN, ctxt->sum_passed, ISO_6429_RESET,
         ISO_6429_FG_RED  , ctxt->sum_failed, ISO_6429_RESET );
   }
}
