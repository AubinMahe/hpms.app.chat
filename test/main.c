#include "all_tests.h"

#include <stdlib.h>

int main( int argc, char * argv[] ) {
   return tests_run( argc, argv,
      "command_parser_test", command_parser_test,
      NULL );
}
