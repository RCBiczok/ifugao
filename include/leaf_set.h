/*** DO NOT INCLUDE DIRECTLY, INCLUDE types.h INSTEAD ***/
#pragma once

//"constraints.h" included via types.h

#include <boost/dynamic_bitset.hpp>
#include <tuple>

class LeafSet {
public:
    virtual std::vector<LeafSet> apply_constraints(
            const std::vector<constraint> &constraints) const;
    virtual bool contains(const size_t leaf) const;
    virtual std::tuple<LeafSet, LeafSet> get_nth_partition_tuple(
            const std::vector<LeafSet> &partitions, const size_t n) const;
    /**
     * Inserts the given leaf, if not already contained.
     * @param leaf The leaf to insert.
     */
    virtual void insert_leaf(const size_t leaf);
    
    /**
     * Removes the given leaf, if contained.
     * @param leaf The leaf to remove.
     */
    virtual void remove_leaf(const size_t leaf);
    virtual size_t size() const;
    virtual std::vector<size_t> get_all_nodes() const;
};

class BitLeafSet : LeafSet {
public:
    std::vector<LeafSet> apply_constraints(
            const std::vector<constraint> &constraints) const;
    bool contains(const size_t leaf) const;
    std::tuple<LeafSet, LeafSet> get_nth_partition_tuple(
            const std::vector<LeafSet> &partitions, const size_t n) const;
    void insert_leaf(const size_t leaf);
    void remove_leaf(const size_t leaf);
    size_t size() const;
    std::vector<size_t> get_all_nodes() const;
    
private:
    boost::dynamic_bitset<> set;
};
