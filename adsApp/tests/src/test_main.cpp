#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

int main(int argc, char** argv) {
    doctest::Context context;
    
    context.setOption("abort-after", 5);
    context.setOption("no-breaks", false);
    
    context.applyCommandLine(argc, argv);
    
    int res = context.run();
    return res;
}