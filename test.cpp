#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>
#include <time.h>

int main(int argc, char* argv[]) {
    // pre-test setup
    srand(time(NULL));

    // run tests
    int result = Catch::Session().run(argc, argv);

    return result;
}

// vim: set expandtab textwidth=100:
