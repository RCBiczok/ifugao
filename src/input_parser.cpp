#include "input_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util.h"

//function to free input data

void free_input_data(input_data *d) {
    for (size_t i = 0; i < d->number_of_species; ++i) {
        delete[] d->names[i];
    }

    delete[] d->names;
    delete[] d->matrix;
    delete d;
}


//read newick tree string from empirical input example files

char *read_newk_tree(const char *newk_file) {
    FILE *f = fopen(newk_file, "rb");

    if (f == NULL) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    if (fsize < 0) {
        fclose(f);
        return NULL;
    }

    char *string = new char[static_cast<size_t>(fsize) + 1];

    auto read = fread(string, static_cast<size_t>(fsize), 1, f);
    if (read <= 0) {
        delete [] string;
        fclose(f);
        return NULL;
    }
    string[static_cast<size_t>(fsize)] = 0;

    fclose(f);

    return string;
}


//parse input data from input example files

input_data *parse_input_data(const char *data_file) {
    FILE *f = fopen(data_file, "rb");

    if (f == NULL) {
        return NULL;
    }

    input_data *result = new input_data;

    char input_buffer[256];

    if (result != NULL) {
        auto read = fscanf(f, "%zu %zu", &(result->number_of_species), &(result->number_of_partitions));
        if (read != 2) {
            delete result;
            fclose(f);
            return NULL;
        }

        result->matrix = new unsigned char[result->number_of_species * result->number_of_partitions];
        result->names = new char *[result->number_of_species];

        for (size_t s = 0; s < result->number_of_species; ++s) {
            for (size_t p = 0; p < result->number_of_partitions; ++p) {
                read = fscanf(f, "%1hhu", &result->matrix[s * result->number_of_partitions + p]);
                if (read != 1) {
                    delete result;
                    fclose(f);
                    return NULL;
                }
            }

            // discard one (whitespace)
            fgetc(f);

            auto readStr = fgets(input_buffer, 256, f);
            if(readStr == NULL) {
                delete result;
                fclose(f);
                return NULL;
            }


            if (feof(f) != 0) {
                // end of line reached, but there is more to read (num_species is too big)
                return NULL;
            }

            size_t len = strlen(input_buffer);
            if (len < 255) {
                result->names[s] = new char[len];
                strncpy(result->names[s], input_buffer, len);
                // replace newline by NULL
                result->names[s][len - 1] = 0;
            } else {
                assert(0);
            }
        }
    }
    auto readStr = fgets(input_buffer, 256, f);
    if (feof(f) == 0 && readStr != NULL) {
        // end of line is not reached yet, but there are no more species to read (num_species is too small)
        return NULL;
    }

    fclose(f);

    return result;
}

ntree_t *get_newk_tree(const char *nwk_file) {
    ntree_t *tree = ntree_parse_newick(nwk_file);
    if (tree == nullptr) {
        /* Most likely file could not be read, although this usually should be
           checked by previously called functions. */
        return nullptr;
    }
    fix_tree(tree); //necessary since tree seems to be corrupt
    return tree;
}

ntree_t *get_newk_tree_from_string(const char *nwk_string) {
    ntree_t *tree = ntree_parse_newick_from_string(nwk_string);
    if (tree == nullptr) {
        return nullptr;
    }
    fix_tree(tree); //necessary since tree seems to be corrupt
    return tree;
}
