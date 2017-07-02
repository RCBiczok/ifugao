#include "missing_data.h"

#include <cstring> // for strlen, strcpy, memcpy

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

/* allocate and initialize */
missingData *initializeMissingData(size_t numberOfSpecies,
                                   size_t numberOfPartitions, const char **speciesNames) {
    missingData *m = new missingData();

    m->numberOfSpecies = numberOfSpecies;

    m->numberOfPartitions = numberOfPartitions;

    //if the species names have been passed by the application programmer just set a pointer to them
    if (speciesNames != nullptr) {
        m->speciesNames = const_cast<char **>(speciesNames);
        m->allocatedNameArray = 0;
    }
    //otherwise, we assume that species names are just indexes
    else {
        m->allocatedNameArray = 1;
        m->speciesNames = new char*[numberOfSpecies];

        size_t i;

        char buffer[1024];

        for (i = 0; i < numberOfSpecies; i++) {
            sprintf(buffer, "%zu", i);

            size_t n = strlen(buffer);

            m->speciesNames[i] = new char[n + 1];

            strcpy(m->speciesNames[i], buffer);
        }
    }
    
    m->missingDataMatrix =
            new unsigned char [numberOfSpecies * numberOfPartitions];
    
    return m;
}

/** Free missingData */
void freeMissingData(missingData *m) {
    free(m->missingDataMatrix);

    if (m->allocatedNameArray == 1) {
        size_t i;
        for (i = 0; i < m->numberOfSpecies; i++)
            free(m->speciesNames[i]);
        free(m->speciesNames);
    }

    free(m);
}

/** Set an element in the missing data matrix */
void setDataMatrix(missingData *m, size_t speciesNumber, size_t partitionNumber,
                   unsigned char value) {
    assert(speciesNumber < m->numberOfSpecies);
    assert(speciesNumber >= 0);

    assert(partitionNumber < m->numberOfPartitions);
    assert(partitionNumber >= 0);

    assert(value == 0 || value == 1);

    m->missingDataMatrix[speciesNumber * m->numberOfPartitions + partitionNumber] =
            value;
}

void copyDataMatrix(const unsigned char *matrix, missingData *m) {
    memcpy(m->missingDataMatrix, matrix,
           m->numberOfPartitions * m->numberOfSpecies);
}
