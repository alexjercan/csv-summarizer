#include <stdio.h>
#define ARGPARSE_IMPLEMENTATION
#include "argparse.h"

int main(int argc, char **argv) {
    struct argparse_parser *parser =
        argparse_new("csv_summarizer", "Summarize a CSV file", "v0.1");

    argparse_parse(parser, argc, argv);

    argparse_free(parser);
    return 0;
}
