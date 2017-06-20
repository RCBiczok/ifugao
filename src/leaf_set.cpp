#include "leaf_set.h"

std::vector<BitLeafSet> BitLeafSet::apply_constraints(
        const std::vector<constraint> &constraints) const {
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

bool BitLeafSet::contains(const size_t leaf) const {
    assert(leaf < set.size());
    
    return set.test(leaf);
}

std::tuple<BitLeafSet, BitLeafSet> BitLeafSet::get_nth_partition_tuple(
            const std::vector<LeafSet> &partitions, const size_t n) const {
    
    //TODO asserts
    // TODO assert partitions isoftype std::vector<BitLeafSet>
    BitLeafSet part_one;
    BitLeafSet part_two;
    
    //TODO assert(n > 0 && n <= number_partition_tuples(partitions));
    //TODO assert(is_bit_set(n,0)==false); // 1st bit is never set
    
    for (size_t i = 1; i < partitions.size(); i++) {
        if (is_bit_set(n, i)) {
            part_one.set|=(BitLeafSet(partitions[i])).set;
        } else {
            part_two.set|=(BitLeafSet(partitions[i])).set;
        }
    }
    
    // no bit may be set in both sets
    assert(!(part_one.set & part_two.set).any());
    
    return std::make_tuple(part_one, part_two);
}

void BitLeafSet::insert_leaf(const size_t leaf) {
    assert(leaf < set.size());
    
    set.set(leaf);
}

void BitLeafSet::remove_leaf(const size_t leaf) {
    assert(leaf < set.size());
    
    set.reset(leaf);
}

size_t BitLeafSet::size() const {
    return set.count();
}

std::vector<size_t> BitLeafSet::get_all_leaves() const {
    std:vector<size_t> result;
    size_t current_pos = set.find_next(0);
    
    while(current_pos!=dynamic_bitset::npos) { // npos basically equals end()
        result.push_back(current_pos);
        size_t current_pos = set.find_next(current_pos);
    }
    
    return result;
}
