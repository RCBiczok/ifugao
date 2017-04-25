#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <stdlib.h>

typedef struct input_data {
	size_t number_of_species;
	size_t number_of_partitions;
	unsigned char* matrix;
	char** names;
} input_data;

/**
 * Free the input data d points to including the struct.
 *
 * @param d The input data to free
 */
void free_input_data(input_data* d);

/**
 * Parse the input data from the given data file.
 * The struct must be freed afterwards by the caller using free_input_data.
 *
 * @param data_file The input file to read
 *
 * @return The parsed input data or NULL if the data could not be parsed.
 */
input_data* parse_input_data(const char* data_file);

/**
 * Reads the newk tree from the given file.
 * The returned data must be freed by the caller.
 *
 * @param newk_file The file to read.
 * @return The contents of the file.
 */
char* read_newk_tree(const char* newk_file);

#endif /* INPUT_PARSER_H */
