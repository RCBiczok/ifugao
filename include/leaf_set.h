/*** DO NOT INCLUDE DIRECTLY, INCLUDE types.h INSTEAD ***/
#pragma once

//"constraints.h" included via types.h

#include <boost/dynamic_bitset.hpp>
#include <tuple>

class LeafSet {
public:
    virtual std::vector<LeafSet> apply_constraints(const std::vector<constraint> &constraints);
    virtual bool contains(size_t leaf);
    static std::tuple<LeafSet, LeafSet> get_nth_partition_tuple(
            std::vector<LeafSet> &partitions, size_t n) {
        // Should never be called!
        assert(false);
        return nullptr;
    }
    /**
     * Inserts the given leaf, if not already contained.
     * @param leaf The leaf to insert.
     */
    virtual void insert_leaf(size_t leaf);
    
    /**
     * Removes the given leaf, if contained.
     * @param leaf The leaf to remove.
     */
    virtual void remove_leaf(size_t leaf);
    
    /**
     * Returns a LeafSet that contains all leaves which are not contained in
     * this LeafSet, but in the base LeafSet.
     * @param base A LeafSet of which this set is a subset to. If parameter is
     * not given, a base of all leaves is assumed.
     * @return A LeafSet that contains all leaves which are not contained in
     * this LeafSet, but in the base LeafSet.
     */
    virtual LeafSet get_complementing_leaf_set_to_base(LeafSet base = nullptr);
};

class BitLeafSet : LeafSet {
public:
    std::vector<LeafSet> apply_constraints(const std::vector<constraint> &constraints);
    bool contains(size_t leaf);
    static std::tuple<LeafSet, LeafSet> get_nth_partition_tuple(
            std::vector<LeafSet> &partitions, size_t n);
    void insert_leaf(size_t leaf);
    void remove_leaf(size_t leaf);
    LeafSet get_complementing_leaf_set_to_base(LeafSet base = nullptr);

private:
    boost::dynamic_bitset<> set;
};
