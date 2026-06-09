#include "asset_loader.h"
#include "engine.h"
#include <stdio.h>

int main() {
    Engine engine;

    if (!engine_init(&engine, "My Game", 1280, 720)) {
        printf("Engine initialization failed!\n");
        return 1;
    }

    engine_run(&engine);

    engine_cleanup(&engine);
    
    return 0;
}