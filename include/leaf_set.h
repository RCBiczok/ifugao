/*** DO NOT INCLUDE DIRECTLY, INCLUDE types.h INSTEAD ***/
#pragma once

#include <boost/dynamic_bitset.hpp>

#include "types.h"
class LeafSet; // for ptr declaration only
class BitLeafSet; // for ptr declaration only
typedef std::unique_ptr<LeafSet> LeafSetPtr;
typedef std::unique_ptr<BitLeafSet> BitLeafSetPtr;

typedef boost::dynamic_bitset<> bitset;

class LeafSet {
public:
    //TODO deleted constructor
    /********* set operations ***********/
    /**
     * Whether a leaf is contained in this set.
     * @param leaf The leaf to find.
     * @return Whether leaf is contained or not.
     */
    virtual bool contains(const size_t leaf) const = 0;
    
    /**
     * Inserts the given leaf, if not already contained.
     * @param leaf The leaf to insert.
     */
    virtual void insert_leaf(const size_t leaf) = 0;
    
    /**
     * Removes the given leaf, if contained.
     * @param leaf The leaf to remove.
     */
    virtual void remove_leaf(const size_t leaf) = 0;
    
    /**
     * The size of this set.
     * @return The size of this set.
     */
    virtual size_t size() const = 0;
    
    /**
     * Lists all leaves in this set.
     * @return A vector containing all leaves.
     */
    virtual std::vector<size_t> get_all_leaves() const = 0;
    
    /********* constraints section ***********/
    virtual std::vector<LeafSetPtr> apply_constraints(
            const std::vector<constraint> &constraints) const = 0;
    
    /**
     * Returns a vector containing all constraints that can be applied to this LeafSet.
     * @param constraints A set of constraints.
     * @return All constraints that can be applied to this LeafSet.
     */
    virtual std::vector<constraint> filter_constraints(
            const std::vector<constraint> &constraints) const = 0;
    
    /********* other ***********/
    virtual std::tuple<LeafSetPtr, LeafSetPtr> get_nth_partition_tuple(
            const std::vector<LeafSetPtr> &partitions, const size_t n) const = 0;
};


class BitLeafSet : public LeafSet {
public:
    BitLeafSet(const size_t amount_of_leaves);
    BitLeafSet(bitset &set);
    
    bool contains(const size_t leaf) const;
    void insert_leaf(const size_t leaf);
    void remove_leaf(const size_t leaf);
    size_t size() const;
    std::vector<size_t> get_all_leaves() const;
    
    std::vector<LeafSetPtr> apply_constraints(
            const std::vector<constraint> &constraints) const;
    std::vector<constraint> filter_constraints(
            const std::vector<constraint> &constraints) const;
    
    std::tuple<LeafSetPtr, LeafSetPtr> get_nth_partition_tuple(
            const std::vector<LeafSetPtr> &partitions, const size_t n) const;
private:
    bitset set;
};

/**
 * Checks whenever the n-th bit is set in the given number
 * @param num the number to check
 * @param n the n-th bit to check whenever it is set or not
 * @return true of the n-th bit is set
 */
inline bool is_bit_set(size_t num, size_t n) {
    return 1 == ((num >> n) & 1);
}

