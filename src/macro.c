#include "macro.h"

#include "includes.h"

#include <sys/stat.h>

size_t getline(char **lineptr, size_t *n, FILE *stream);

Macro* parse_txt(char* path) {
    Macro* macro = malloc(sizeof(Macro));
    memset(macro, 0, sizeof(Macro));

    FILE* f = fopen(path, "r");

    if (!f) {
        printf("Unable to open macro");
        return macro;
    }

    char* line = malloc(256);

    size_t len = 0;
    size_t read;

    struct stat file_info;
	stat(path, &file_info);

    macro->actions = malloc(sizeof(Action));

    read = getline(&line, &len, f);

    sscanf(line, "%i", &macro->fps);

    while ((read = getline(&line, &len, f)) != -1) {
        if (read > 5) {
            Action action = {0,0,0};

            sscanf(line, "%i %i %i", &action.frame, &action.player1_holding, &action.player2_holding);
            macro->actions[macro->action_num] = action;
            macro->actions = realloc(macro->actions, (macro->action_num + 2) * sizeof(Action));
            macro->action_num++;
        }
    }

    macro->actions = realloc(macro->actions, macro->action_num * sizeof(Action));
    free(line);
    return macro;
}

void Macro_free(Macro* macro) {
    free(macro->actions);
    free(macro);
}

size_t getline(char **lineptr, size_t *n, FILE *stream) { //defining _GNU_SOURCE does not seem to work for me (GCC) so im defining it myself
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (n == NULL) {
        return -1;
    }

    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }

    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }

    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (size - 1)) {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL) {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}