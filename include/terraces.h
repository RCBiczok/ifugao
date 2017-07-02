#pragma once

#include "types.h"
#include "ifugao.h"
#include "input_parser.h"
#include "util.h"

#include <stdio.h>
#include <stdbool.h>
#include <gmp.h>
#include <cstring>

/* 
 Error Codes

 the return value of our function is an error code
 we will define these together as the project proceeds, e.g.
 0:  succesfull completion
 -1: problem parsing Newick file
 -2: #species in Newick file does not correspond to number of species in data matrix
 -3: entries in data matrix not either 0 or 1
 -4: less than 4 spcies in input tree
 -5: only one partition in data matrix
 -6: reserved for something you must think about anyway (tree can't be rooted)
 -7: no output file specified
 */

#define TERRACE_SUCCESS                   0 
#define TERRACE_NEWICK_ERROR             -1
#define TERRACE_NOT_ENOUGH_SPECIES       -2 
#define TERRACE_MATRIX_ERROR             -3 
#define TERRACE_NUM_SPECIES_ERROR        -4 
#define TERRACE_NUM_PARTITIONS_ERROR     -5
#define TERRACE_THINK_ON_YOUR_OWN_ERROR  -6
#define TERRACE_OUTPUT_FILE_ERROR        -7
#define TERRACE_NO_SPECIES_WITH_FULL_SET -8
/* to be extended */

/* Argument to control output of terraceAnalysis function (ta_outspec) */

/** 
 count unrooted trees on terrace
 */
#define TA_COUNT              1

/**
 print unrooted trees on terrace to file
 @TODO: should TA_ENUMERATE automatically imply TA_COUNT?
 @TODO: Yes it should!
 */
#define TA_ENUMERATE          2

/**
 just detect if the tree is on a terrace. this should run much quicker than TA_COUNT or TA_ENUMERATE,
 because we can brake off, as soon as we have found that thera are at least two trees
 on the terrace.
 @TODO: how the output should look like in this case?
 @TODO: return any integer in terraceSize large than 1 if the tree is on a terrace
 */
#define TA_DETECT             4

/**
 print trees on a terrace in compressed form using some external binary tree compression tool.
 optional, does not need to be implemented, only if you want to.
 */
#define TA_ENUMERATE_COMPRESS 8

/**
 * Returns a vector containing all constraints infered from the given supertree.
 *
 * @param supertree the supertree we want to extract constraints from
 * @param missing_data the missing data information
 * @return All constraints of the given supertree.
 */
std::vector<constraint> extract_constraints_from_supertree(
        const Tree supertree, const missingData* missing_data);

/**
 * Function that tells us, given a tree, and a missing data matrix as well as its dimensions,
 * if the tree is on a terrace, how many trees there are on the terrace, it also prints trees on the terrace
 * to file, if specified and might compress them as well.
 *
 * We might need to change the data type of variable terraceSize that is being written in this function.
 * Why?
 * 
 * @param[in] m struct containing missing data matrix, number of partitions, number of species, and list of
 * species names
 * 
 * @param[in] newickTreeString Unrooted strictly binary phylogenetic tree,
 * in Newick format for which we want to find out if it is on a terrace.
 * Denoted by T in the task specification
 * 
 * @param[in] ta_outspec bit-masked integer as combination of TA_* constants to control the outputs
 * 
 * @param[out] allTreesOnTerrace output file for unrooted tree enumeration.
 * Trees should be displayed in standard unrooted Newick format, and you should print one tree per line.
 *
 * qparam[out] terraceSize number of unrooted trees on the terrace
 * 
 * @return TERRACE_SUCCESS on success, or an error code (see TERRACE_*) on failure
 */
int terraceAnalysis(missingData *m, const char *newickTreeString,
        const int ta_outspec, FILE *allTreesOnTerrace, mpz_t *terraceSize);

/*
 * for debug statements
 */
#ifndef DEBUG
void d_print_tree_impl(const char* file, const int line, const ntree_t* tree);
/* TODO reenable if desired, needs rewriting though
void d_print_tree_impl(const char* file, const int line, const Tree tree);
*/
#define d_print_tree(t) do { \
   d_print_tree_impl(__FILE__, __LINE__, t);\
} while(false)
#define d_printf(...) do { \
   fprintf(stderr, "DEBUG(%s, %d): ", __FILE__, __LINE__);\
   fprintf(stderr, __VA_ARGS__);\
} while(false)
#else
#define d_printf(...) do {} while(false)
#define d_print_tree(tree) do {} while(false)
#endif
