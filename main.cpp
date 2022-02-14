#include <iostream>
#include <stdexcept>

#include "application.h"

int main() {
    Application app;

    try {
        app.start();
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    return 0;
}
