#include "constraints.h"

std::vector<constraint> extract_constraints_from_tree(const Tree supertree) {
    std::vector<constraint> constraints;
    
    supertree.get_constraints(constraints);
    
    return constraints;
}

std::vector<constraint> find_constraints(const LeafSet &leaves,
                                         const std::vector<constraint> &constraints) {
    std::vector<constraint> valid_constraints;

    for (constraint cons : constraints) {
        if (cons.smaller_left == cons.bigger_left) {
            if (leaves.contains(cons.smaller_left) != leaves.end()
                && leaves.contains(cons.smaller_right) != leaves.end()
                && leaves.contains(cons.bigger_right) != leaves.end()) {
                // constraint is valid on leaf set
                valid_constraints.push_back(cons);
            }
        } else {
            // smaller_right == bigger_right
            if (leaves.contains(cons.smaller_left) != leaves.end()
                && leaves.contains(cons.smaller_right) != leaves.end()
                && leaves.contains(cons.bigger_left) != leaves.end()) {
                // constraint is valid on leaf set
                valid_constraints.push_back(cons);
            }
        }
    }
    return valid_constraints;
}
