// Copyright (C) 2022 saccharineboi

#include "init.h"
#include "framebuffer.h"
#include "lights.h"
#include "error.h"
#include "cursedgl.h"

#include <locale.h>
#include <string.h>
#include <stdlib.h>

////////////////////////////////////////
static struct notcurses* ctx = NULL;
static notcurses_options options;
static ncblitter_e currentBlitter = NCBLIT_1x1;

////////////////////////////////////////
#define CURSEDGL_OPTION_NAME_MAX_LEN 100

////////////////////////////////////////
#define CURSEDGL_HELP       "Usage: <executable> [OPTION]...\n" \
                            "Options:\n" \
                            "\t-h, --help\t\tprint this help and exit\n" \
                            "\t-v, --version\t\tprint the version and exit\n" \
                            "\t-k, --keep\t\tdo not switch to alternate screen\n" \
                            "\t-l, --log\t\tlogging level: from -1 (silent) to 7 (manic)\n" \
                            "\t-s, --suppress\t\tsuppress banner\n" \
                            "\t-b, --blitter\t\t1x1 (default), 2x1, 2x2, 3x2, braille\n"

////////////////////////////////////////
#define CURSEDGL_LICENSE_INFO "Copyright (C) saccharineboi\n" \
                              "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n" \
                              "This is free software: you are free to change and redistribute it.\n" \
                              "There is NO WARRANTY, to the extent permitted by law.\n"

////////////////////////////////////////
static void applyOptions(int argc, char** argv)
{
    if (argc == 1)
        return;

    for (int i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], "-v", CURSEDGL_OPTION_NAME_MAX_LEN) ||
            !strncmp(argv[i], "--version", CURSEDGL_OPTION_NAME_MAX_LEN)) {
            printf("CursedGL %d.%d.%d\n", CURSEDGL_VERSION_MAJOR,
                                                   CURSEDGL_VERSION_MINOR,
                                                   CURSEDGL_VERSION_PATCH);
            printf("\n%s", CURSEDGL_LICENSE_INFO);
            exit(EXIT_SUCCESS);
        }
        else if (!strncmp(argv[i], "-h", CURSEDGL_OPTION_NAME_MAX_LEN) ||
                 !strncmp(argv[i], "--help", CURSEDGL_OPTION_NAME_MAX_LEN)) {
            printf("%s", CURSEDGL_HELP);
            exit(EXIT_SUCCESS);
        }
        else {
            if (!strncmp(argv[i], "-k", CURSEDGL_OPTION_NAME_MAX_LEN) ||
                !strncmp(argv[i], "--keep", CURSEDGL_OPTION_NAME_MAX_LEN)) {
                options.flags |= NCOPTION_NO_ALTERNATE_SCREEN;
            }
            if (!strncmp(argv[i], "-l", CURSEDGL_OPTION_NAME_MAX_LEN) ||
                !strncmp(argv[i], "--log", CURSEDGL_OPTION_NAME_MAX_LEN)) {
                if (i < argc - 1) {
                    int logLevel = atoi(argv[i + 1]);
                    switch (logLevel) {
                        case 0:
                            options.loglevel = NCLOGLEVEL_TRACE;
                            break;
                        case 1:
                            options.loglevel = NCLOGLEVEL_DEBUG;
                            break;
                        case 2:
                            options.loglevel = NCLOGLEVEL_VERBOSE;
                            break;
                        case 3:
                            options.loglevel = NCLOGLEVEL_INFO;
                            break;
                        case 4:
                            options.loglevel = NCLOGLEVEL_WARNING;
                            break;
                        case 5:
                            options.loglevel = NCLOGLEVEL_ERROR;
                            break;
                        case 6:
                            options.loglevel = NCLOGLEVEL_FATAL;
                            break;
                        case 7:
                            options.loglevel = NCLOGLEVEL_PANIC;
                            break;
                        default:
                            options.loglevel = NCLOGLEVEL_SILENT;
                            break;
                    }
                }
                else {
                    printf("%s: not enough arguments\n", argv[0]);
                    exit(EXIT_SUCCESS);
                }
            }
            if (!strncmp(argv[i], "-s", CURSEDGL_OPTION_NAME_MAX_LEN) ||
                !strncmp(argv[i], "--suppress", CURSEDGL_OPTION_NAME_MAX_LEN)) {
                options.flags |= NCOPTION_SUPPRESS_BANNERS;
            }
            if (!strncmp(argv[i], "-b", CURSEDGL_OPTION_NAME_MAX_LEN) ||
                !strncmp(argv[i], "--blitter", CURSEDGL_OPTION_NAME_MAX_LEN)) {
                if (i < argc - 1) {
                    if (!strncmp(argv[i + 1], "1x1", CURSEDGL_OPTION_NAME_MAX_LEN))
                        currentBlitter = NCBLIT_1x1;
                    else if (!strncmp(argv[i + 1], "2x1", CURSEDGL_OPTION_NAME_MAX_LEN))
                        currentBlitter = NCBLIT_2x1;
                    else if (!strncmp(argv[i + 1], "2x2", CURSEDGL_OPTION_NAME_MAX_LEN))
                        currentBlitter = NCBLIT_2x2;
                    else if (!strncmp(argv[i + 1], "3x2", CURSEDGL_OPTION_NAME_MAX_LEN))
                        currentBlitter = NCBLIT_3x2;
                    else if (!strncmp(argv[i + 1], "braille", CURSEDGL_OPTION_NAME_MAX_LEN))
                        currentBlitter = NCBLIT_BRAILLE;
                    else
                        currentBlitter = NCBLIT_1x1;
                }
                else {
                    printf("%s: not enough arguments\n", argv[0]);
                    exit(EXIT_SUCCESS);
                }
            }
        }
    }
}

////////////////////////////////////////
bool txInit(int argc, char** argv)
{
    applyOptions(argc, argv);

    if (!setlocale(LC_ALL, "")) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInit: couldn't set locale");
        return false;
    }

    if (!(ctx = notcurses_core_init(&options, NULL))) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInit: couldn't initialize notcurses");
        return false;
    }

    if (!txInitFramebuffer(notcurses_stdplane(ctx))) {
        txOutputMessage(TX_ERROR, "[CursedGL] txInit: couldn't initialize framebuffer");
        return false;
    }

    txOutputMessage(TX_INFO, "[CursedGL] txInit: successfully initialized CursedGL");
    return true;
}

////////////////////////////////////////
bool txEnd()
{
    if (!txFreeFramebuffer()) {
        txOutputMessage(TX_ERROR, "[CursedGL] txEnd: couldn't free the framebuffer(s)");
        return false;
    }
    return !notcurses_stop(ctx);
}

////////////////////////////////////////
struct notcurses* txGetContext()
{
    return ctx;
}

////////////////////////////////////////
ncblitter_e txGetCurrentBlitter()
{
    return currentBlitter;
}
