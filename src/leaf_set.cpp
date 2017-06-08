#include "leaf_set.h"

std::vector<BitLeafSet> BitLeafSet::apply_constraints(
        const std::vector<constraint> &constraints) {
    std::vector<LeafSet> new_sets;
    
    size_t index_containing_left_constraint = 0;
    size_t index_containing_right_constraint = 0;
    
    for (constraint cons : constraints) {
        //TODO shouldn't find constraints guarantee this? if yes make it an assert
        if(this.set[cons.smaller_left] && this.set[cons.smaller_right]){
            bool existed = false;
            size_t found_set_ind;
            // check if set with one of the constraint's leaves has already been created
            for(size_t i; i < new_sets.size(); ++i) {
                auto new_set = new_sets[i];
                if(new_set.set[smaller_left] || new_set.set[smaller_right]) {
                    // set for at least one of the constraint leaves exists
                    
                    if(existed) {// constraint merges two existing sets
                        //store merged sets as first set
                        new_sets[found_set_ind].set |= new_set.set;
                        //override second set
                        size_t last_ind = new_sets.size()-1;
                        if(i != last_ind) {
                            new_sets[i] = new_sets[last_ind];
                        }
                        new_sets.pop_back();
                        break; // constraints can only ever merge two sets
                    } else {// constraint belongs to already existing set
                        new_set.set[smaller_left] = true;
                        new_set.set[smaller_right] = true;
                        found_set_ind = i;
                        existed = true;
                    }
                }
            }
            if(!existed) {// constraint needs a new set
                new_sets.emplace_back();
                // no set exists yet, create new one
                new_sets.back().set[smaller_left] = true;
                new_sets.back().set[smaller_right] = true;
            }
        }
    }
    return new_sets;
}

bool BitLeafSet::contains(size_t leaf) {
    return set[leaf];
}

std::tuple<BitLeafSet, BitLeafSet> BitLeafSet::get_nth_partition_tuple(
            std::vector<BitLeafSet> &partitions, size_t n) {
    
}

