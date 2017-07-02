#pragma once

// data type containing data to be passed to the algorithm we want to implement 
typedef struct {
    size_t numberOfSpecies;
    size_t numberOfPartitions;
    unsigned char *missingDataMatrix;
    char **speciesNames;
    bool allocatedNameArray;
} missingData;

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
                            size_t partitionNumber) {
    assert(speciesNumber < m->numberOfSpecies);
    assert(speciesNumber >= 0);

    assert(partitionNumber < m->numberOfPartitions);
    assert(partitionNumber >= 0);

    unsigned char value = m->missingDataMatrix[speciesNumber
                                               * m->numberOfPartitions + partitionNumber];
    assert(value == 0 || value == 1);

    return value;
}
