#pragma once

#include "types.h"

// data type containing data to be passed to the algorithm we want to implement 
typedef struct {
    size_t numberOfSpecies;
    size_t numberOfPartitions;
    unsigned char *missingDataMatrix;
    char **speciesNames;
    bool allocatedNameArray;
} missingData;

/* functions for handling the missing data data structure */

/**
 * get entry from missing data matrix 
 * 
 * @param m pointer to missing data data structure 
 * 
 * @param speciesNumber species index 
 *
 * @param partitionNumber partition index 
 * 
 * @return the value at the specified matrix position 
 */
unsigned char getDataMatrix(const missingData *m, size_t speciesNumber,
                            size_t partitionNumber);

/**
 * Initialize missing data data type 
 *
 * @param numberOfSpecies number of species in dataset 
 *
 * @param numberOfPartitions number of partitions in dataset 
 *
 * @param speciesNames list of species names in dataset, first entry correpsonds to first row in missingDataMatrix etc. 
 * 
 * @return poitner to missing data data structure 
 */
missingData * initializeMissingData(size_t numberOfSpecies,
        size_t numberOfPartitions, const char **speciesNames);

/**
 * Free missing data data structure 
 * 
 * @param m pointer to missing data data structure 
 */
void freeMissingData(missingData *m);

/**
 * set entry in missing data matrix 
 * 
 * @param m pointer to missing data data structure 
 * 
 * @param speciesNumber species index 
 *
 * @param partitionNumber partition index 
 * 
 * @param value value to be set 
 */
void setDataMatrix(missingData *m, size_t speciesNumber, size_t partitionNumber,
        unsigned char value);

/**
 * copy one dimensional array containing the missing data matrix to the matrix in the missing data data type
 * 
 * @param matrix one-dimensional 
 * 
 * @param m pointer to missing data data structure 
 * 
 */
void copyDataMatrix(const unsigned char *matrix, missingData *m);


