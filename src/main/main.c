/**
 * @file main.c
 *
 * @author Adnan FAIZE <adnanfaize@gmail.com>
 */

#include "../core/runtime/runtime.h"

#ifndef APP_VERSION
#define APP_VERSION "error : unknown version"
#endif

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// -v --version 
// -h --help

void help() {

}

int entrypoint(int argc, char **argv) {
    application_t app_details = {
        .title = "Desktop Pet",
        .version = APP_VERSION,
        .entrypoint = nullptr,
    };

    application_params_t app_params = 0;

    for (int i = 0; i < argc; i++) {
        // TODO : parse the arguments into params
    }

    app_details.params = &app_params;

    return application_run(&app_details) ? EXIT_SUCCESS : EXIT_FAILURE;
}

#ifdef PLATFORM_WINDOWS

#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLize, int nCmdShow) {
    return entrypoint(__argc, __argv);
}

#else

int main(int argc, char **argv) {
    return entrypoint(argc, argv);
}

#endif
