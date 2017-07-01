#include "constraints.h"

std::vector<constraint> extract_constraints_from_tree(const Tree supertree) {
    std::vector<constraint> constraints;
    
    supertree->get_constraints(constraints);
    
    return constraints;
}
