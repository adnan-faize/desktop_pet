/**
 * @file main.c
 *
 * @author Adnan Faize <adnanfaize@gmail.com>
 */

#include "application/application.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int main(void)
{
    application_t app_params =
        {
            .title = "Desktop Pet",
            .version = "alpha.0.1"
        };

    return application_run(&app_params) ? EXIT_SUCCESS : EXIT_FAILURE;
}
