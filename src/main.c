#include <stdio.h>
#include "engine.h"


int main() {
    Engine engine;

    if (!engine_init(&engine, "My Game", 800, 600)) {
        printf("Engine initialization failed!\n");
        return 1;
    }

    engine_run(&engine);

    engine_cleanup(&engine);
    
    return 0;
}