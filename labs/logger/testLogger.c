#include "logger.c"

int main() {
    infof("General information");
    warnf("Warnings");
    errorf("Errors");
    panicf("Panics. Non recoverable issues with core dump");

    return 0;
}