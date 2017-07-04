#include "leaf_set.h"

BitLeafSet::BitLeafSet(size_t amount_of_leaves) {
    boost::dynamic_bitset<> new_set(amount_of_leaves);
    this->set = new_set;
    // all leaves are in the set -> all set to true
    this->set.set();
}

BitLeafSet::BitLeafSet(bitset &set) {
    this->set = set;
}

std::vector<LeafSetPtr> BitLeafSet::apply_constraints(
        const std::vector<constraint> &constraints) const {
    std::vector<bitset> new_sets;
    
    for (constraint c : constraints) {
        assert(this->set[c.smaller_left] && this->set[c.smaller_right]);
        
        bool existed = false; 
        size_t found_set_ind;
        
        // check if set with one of the constraints' leaves has already been
        // created, store result in 'existed'
        for(size_t i; i < new_sets.size(); ++i) {
             bitset& bit_set = new_sets[i];
            if(bit_set.test(c.smaller_left) || bit_set.test(c.smaller_right)) {
                // set for at least one of the constraints' leaves exists
                if(existed) {// constraint merges two existing sets
                    //store merged sets as first set
                    new_sets[found_set_ind] |= bit_set;
                    //override second set
                    size_t last_ind = new_sets.size()-1;
                    if(i != last_ind) { // second set is not last set
                        new_sets[i] = new_sets[last_ind]; // move
                    }
                    new_sets.pop_back();
                    break; // constraints can only ever merge two sets
                } else {// constraint belongs to already existing set
                    bit_set.set(c.smaller_left);
                    bit_set.set(c.smaller_right);
                    found_set_ind = i;
                    existed = true;
                }
            }
        }
        if(!existed) {// constraint needs a new set
            new_sets.emplace_back(this->set.size());
            // no set exists yet, create new one
            new_sets.back().set(c.smaller_left);
            new_sets.back().set(c.smaller_right);
        }
    }
    
    std::vector<LeafSetPtr> return_sets;
    return_sets.reserve(new_sets.size());
    for(bitset set: new_sets) {
        BitLeafSetPtr wrapped_set = std::make_unique<BitLeafSet>(set);
        return_sets.push_back(std::move(wrapped_set));
    }
    return return_sets;
}

bool BitLeafSet::contains(const size_t leaf) const {
    assert(leaf < this->set.size());
    
    return this->set.test(leaf);
}

std::pair<LeafSetPtr, LeafSetPtr> BitLeafSet::get_nth_partition_pair(
            const std::vector<LeafSetPtr> &partitions, const size_t n) const {
    //TODO asserts
    // TODO assert partitions isoftype std::vector<BitLeafSet>
    BitLeafSetPtr part_one = std::make_unique<BitLeafSet>(this->set.size());
    BitLeafSetPtr part_two = std::make_unique<BitLeafSet>(this->set.size());
    
    //TODO assert(n > 0 && n <= number_partition_pair(partitions));
    //TODO assert(is_bit_set(n,0)==false); // 1st bit is never set
    
    for (size_t i = 1; i < partitions.size(); i++) {
        auto bit_set_ptr = dynamic_cast<BitLeafSet&>(*partitions[i]);
        if (is_bit_set(n, i)) {
            part_one->set |= bit_set_ptr.set;
        } else {
            part_two->set |= bit_set_ptr.set;
        }
    }
    
    // no bit may be set in both sets
    assert(!(part_one->set & part_two->set).any());
    
    return std::make_pair(std::move(part_one), std::move(part_two));
}

void BitLeafSet::insert_leaf(const size_t leaf) {
    assert(leaf < this->set.size());
    
    this->set.set(leaf);
}

void BitLeafSet::remove_leaf(const size_t leaf) {
    assert(leaf < this->set.size());
    
    this->set.reset(leaf);
}

size_t BitLeafSet::size() const {
    return this->set.count();
}

std::vector<size_t> BitLeafSet::get_all_leaves() const {
    std::vector<size_t> result;
    size_t current_pos = set.find_next(0);
    
    while(current_pos!=boost::dynamic_bitset<>::npos) { // npos basically equals end()
        result.push_back(current_pos);
        size_t current_pos = this->set.find_next(current_pos);
    }
    
    return result;
}

std::vector<constraint> BitLeafSet::filter_constraints(
            const std::vector<constraint> &constraints) const {
    std::vector<constraint> valid_constraints;
    
    for (constraint c : constraints) {
        if (c.smaller_left == c.bigger_left) {
            if (this->set.test(c.smaller_left)
                && this->set.test(c.smaller_right)
                && this->set.test(c.bigger_right)) {
                // constraint is valid on leaf set
                valid_constraints.push_back(c);
            }
        } else {
            assert(c.smaller_right == c.bigger_right);
            
            if (this->set.test(c.smaller_left)
                && this->set.test(c.smaller_right)
                && this->set.test(c.bigger_left)) {
                // constraint is valid on leaf set
                valid_constraints.push_back(c);
            }
        }
    }
    return valid_constraints;
}

