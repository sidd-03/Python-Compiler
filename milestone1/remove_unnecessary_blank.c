#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int is_empty_line(const char *line) {
    while (*line != '\0') {
        if (!isspace(*line)) {
            return 0; // Not empty
        }
        line++;
    }
    return 1; // Empty line
}

void remove_empty_lines(const char *input_filename, const char *output_filename) {
    FILE *input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    FILE *output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        fclose(input_file);
        exit(EXIT_FAILURE);
    }

    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), input_file) != NULL) {
        if (!is_empty_line(buffer)) {
            fputs(buffer, output_file);
        }
    }

    fclose(input_file);
    fclose(output_file);
}

int main() {
    const char *input_filename = "public1.py";  // Change this to your input file name
    const char *output_filename = "output.py";  // Change this to your output file name

    remove_empty_lines(input_filename, output_filename);

    printf("Empty lines removed successfully.\n");

    return 0;
}
