#pragma once

#include <memory>
#include <vector>
#include <set>
#include <assert.h>
#include <iostream>

#include "constraints.h"
#include "leaf_set.h"
#include "tree.h"


/**
 * Method calling overview:
 * 
 *  list_trees
 *    find_all_rooted_trees
 *      [recursion stop]
 *        get_all_binary_trees
 *      apply_constraints
 *     
 *   d_print_tree
 * 
 */

/**
 * Calculates the number of trees on the terrace.
 *
 * @param constraints All constraints to fulfill.
 * @param leaves All leaves of the tree.
 * @param root_species_name the root species
 * @param file File to write all trees in newick format into, iff file != nullptr.
 * @return Number of all trees on the terrace.
 */
size_t list_trees(const std::vector<constraint> &constraints,
                  const size_t &root_id,
                  const LeafSet &leaves,
                  const std::vector<std::string> &leaf_to_label,
                  FILE *file);

//TODO doc
/** Combines all sets (constraints need to be applied already) */
std::vector<std::shared_ptr<Tree> > find_all_unrooted_trees(
        const LeafSet &leaves, const std::vector<constraint> &constraints,
        const size_t &root_id);


/** Combines all sets (constraints need to be applied already) */
std::vector<std::shared_ptr<Node> > find_all_rooted_trees(
        const LeafSet &leaves,
        const std::vector<constraint> &constraints);


std::vector<std::shared_ptr<Node> > get_all_binary_trees(const LeafSet &leaves);

/** merges two sub-trees */
std::vector<std::shared_ptr<Tree> > merge_subtrees(
        std::vector<std::shared_ptr<Node> > &left,
        std::vector<std::shared_ptr<Node> > &right);

/** write the given tree to the gien file */
void write_tree(Tree &tree, FILE &file);

/**
 * Checks whenever the n-th bit is set in the given number
 * @param num the number to check
 * @param n the n-th bit to check whenever it is set or not
 * @return true of the n-th bit is set
 */
inline bool is_bit_set(size_t num, size_t n) {
    return 1 == ((num >> n) & 1);
}

/**
 * Returns the number of partition tuples that can be formed by combining the
 * given list of partitions. The formular is 2^(n-1) - 1, where n is the size of
 * the list.
 * @param partitions the list of partitions
 * @return the number of partition tuples that can be formed from the given list
 */
inline size_t number_partition_tuples(
        std::vector<std::shared_ptr<LeafSet> > &partitions) {
    assert(partitions.size() > 1);

    return (1 << (partitions.size() - 1)) - 1;
}

