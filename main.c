#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARGPARSE_IMPLEMENTATION
#include "argparse.h"

#define MAX_LINE_LENGTH 1024

#define return_defer(code)                                                     \
    do {                                                                       \
        result = code;                                                         \
        goto defer;                                                            \
    } while (0)

struct lines {
        char **items;
        unsigned int count;
        unsigned int capacity;
};

void lines_free(struct lines *lines) {
    for (unsigned int i = 0; i < lines->count; i++) {
        free(lines->items[i]);
    }
    if (lines->items != NULL)
        free(lines->items);
}

int lines_contains(struct lines *lines, char *item) {
    for (unsigned int i = 0; i < lines->count; i++) {
        if (strcmp(lines->items[i], item) == 0) {
            return 1;
        }
    }
    return 0;
}

struct column {
        char *name;
        char **items;
        unsigned int count;
        unsigned int capacity;
};

void column_free(struct column col) {
    if (col.items != NULL)
        free(col.items);
}

struct dataframe {
        struct column *items;
        unsigned int count;
        unsigned int capacity;
};

void dataframe_free(struct dataframe *df) {
    for (unsigned int i = 0; i < df->count; i++) {
        column_free(df->items[i]);
    }
    if (df->items != NULL)
        free(df->items);
}

void dataframe_parse(struct lines *lines, struct dataframe *df) {
    char *header = lines->items[0];
    char *token = strtok(header, ",");
    while (token != NULL) {
        struct column col = {0};
        col.name = token;
        arg_da_append(df, col);
        token = strtok(NULL, ",");
    }

    for (int i = 1; i < lines->count; i++) {
        char *line = lines->items[i];
        char *token = strtok(line, ",");
        int j = 0;
        while (token != NULL) {
            struct column *col = &df->items[j];
            arg_da_append(col, token);
            token = strtok(NULL, ",");
            j++;
        }
    }
}

int dataframe_summary(char *filename, struct dataframe *df, struct lines names) {
    int result = 0;

    FILE *file = NULL;
    if (filename == NULL) {
        file = stdout;
    } else {
        file = fopen(filename, "w");
    }

    if (file == NULL) {
        ARG_LOG_ERROR("Failed to open file for writing");
        return_defer(1);
    }

    fprintf(file, "column,min,max,avg\n");
    for (unsigned int i = 0; i < df->count; i++) {
        struct column col = df->items[i];

        if (!lines_contains(&names, col.name)) {
            continue;
        }

        fprintf(file, "%s,", col.name);
        float item = atof(col.items[0]);
        float min = item;
        float max = item;
        float sum = item;
        for (unsigned int j = 1; j < col.count; j++) {
            item = atof(col.items[j]);
            if (item < min) {
                min = item;
            }
            if (item > max) {
                max = item;
            }
            sum += item;
        }
        float avg = sum / col.count;

        fprintf(file, "%.2f,%.2f,%.2f\n", min, max, avg);
    }

defer:
    if (filename != NULL) {
        fclose(file);
    }

    return result;
}

int read_lines(const char *filename, struct lines *lines) {
    int result = 0;

    FILE *file = NULL;
    if (filename == NULL) {
        file = stdin;
    } else {
        file = fopen(filename, "r");
    }

    if (file == NULL) {
        ARG_LOG_ERROR("Failed to open file for reading");
        return_defer(1);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        arg_da_append(lines, strdup(line));
    }

defer:
    if (filename != NULL) {
        fclose(file);
    }

    return result;
}

int main(int argc, char **argv) {
    int result = 0;

    struct argparse_parser *parser =
        argparse_new("csv_summarizer", "Summarize a CSV file", "v0.1");
    if (parser == NULL) {
        ARG_LOG_ERROR("Failed to create parser");
        return_defer(1);
    }

    argparse_add_argument(
        parser, (struct argparse_options){.short_name = 'i',
                                          .long_name = "input",
                                          .description = "Input CSV file",
                                          .type = ARGUMENT_TYPE_VALUE,
                                          .required = 0});

    argparse_add_argument(
        parser, (struct argparse_options){.short_name = 'o',
                                          .long_name = "output",
                                          .description = "Output CSV file",
                                          .type = ARGUMENT_TYPE_VALUE,
                                          .required = 0});

    argparse_add_argument(
        parser, (struct argparse_options){.short_name = 'c',
                                          .long_name = "columns",
                                          .description = "Output CSV file",
                                          .type = ARGUMENT_TYPE_POSITIONAL,
                                          .required = 1});

    argparse_parse(parser, argc, argv);

    char *input = argparse_get_value(parser, "input");
    char *output = argparse_get_value(parser, "output");
    char *columns = argparse_get_value(parser, "columns");

    struct lines lines = {0};
    if (read_lines(input, &lines) != 0) {
        return_defer(1);
    }

    struct dataframe df = {0};
    dataframe_parse(&lines, &df);

    struct lines names = {0};
    char *token = strtok(columns, ",");
    while (token != NULL) {
        arg_da_append(&names, strdup(token));
        token = strtok(NULL, ",");
    }

    if (dataframe_summary(output, &df, names) != 0) {
        return_defer(1);
    }

defer:
    if (parser != NULL)
        argparse_free(parser);
    lines_free(&lines);
    dataframe_free(&df);
    return 0;
}
