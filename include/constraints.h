/*** DO NOT INCLUDE DIRECTLY, INCLUDE types.h INSTEAD ***/
#pragma once

#include "tree.h"

/**
 * A simple constraint structure. It is meant to be used as:
 * lca(smaller_left, smaller_right) < lca(bigger_left, bigger_right)
 */
struct constraint {
    size_t smaller_left;
    size_t bigger_left;
    size_t smaller_right;
    size_t bigger_right;
};

#include "leaf_set.h"

/**
 * Returns a vector containing all constraints that still are valid for the given set of leaves.
 *
 * @param leaves Leaves to search valid constraints for.
 * @param constraints All constraints that could still be valid.
 * @return All constraints that are still valid.
 */
std::vector<constraint> find_constraints(const LeafSet &leaves,
                                         const std::vector<constraint> &constraints);

/**
 * Returns a vector containing all constraints infered from the given supertree.
 *
 * @param supertree All supertree from which the constraints will be extracted
 * @return All constraints of the given super tree.
 */
std::vector<constraint> extract_constraints_from_tree(
        const Tree supertree);

