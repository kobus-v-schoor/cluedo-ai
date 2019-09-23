#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>
#include <time.h>

// Include headers of classes that are going to be tested here
#include "include/bot.h"
#include "include/position.h"
#include "include/board.h"

//// deductors
#include "include/deductors/local-exclude.h"

int main(int argc, char* argv[]) {
    // pre-test setup
    srand(time(NULL));

    // run tests
    int result = Catch::Session().run(argc, argv);

    // post-test cleanup

    return result;
}

// vim: set expandtab textwidth=100:
