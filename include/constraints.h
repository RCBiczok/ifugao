/*** DO NOT INCLUDE DIRECTLY, INCLUDE types.h INSTEAD ***/
#pragma once

#include "types.h"

/**
 * Returns a vector containing all constraints infered from the given supertree.
 *
 * @param supertree All supertree from which the constraints will be extracted
 * @return All constraints of the given super tree.
 */
std::vector<constraint> extract_constraints_from_tree(
        const Tree supertree);

