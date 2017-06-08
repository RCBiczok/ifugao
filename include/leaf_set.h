#include <boost/dynamic_bitset.hpp>
#include <tuple>
#include <vector>

public class LeafSet {
public:
    virtual std::vector<LeafSet> apply_constraints(const std::vector<constraint> &constraints);
    virtual bool contains(size_t leaf);
    virtual static std::tuple<LeafSet, LeafSet> get_nth_partition_tuple(
            std::vector<LeafSet> &partitions, size_t n);
};

public class BitLeafSet : LeafSet {
public:
    std::vector<BitLeafSet> apply_constraints(const std::vector<constraint> &constraints);
    bool contains(size_t leaf);
    static std::tuple<BitLeafSet, BitLeafSet> get_nth_partition_tuple(
            std::vector<BitLeafSet> &partitions, size_t n);

private:
    boost::dynamic_bitset<> set;
};
