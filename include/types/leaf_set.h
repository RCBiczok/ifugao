/*** DO NOT INCLUDE DIRECTLY, INCLUDE types.h INSTEAD ***/
#pragma once

#include <boost/dynamic_bitset.hpp>
#include <limits>

#include "types.h"
#include "debug.h"

//Black Magic
class BitLeafSet;
typedef BitLeafSet LeafSet;

//class UnionFindLeafSet;
//typedef UnionFindLeafSet LeafSet;

/**
 * Checks whenever the n-th bit is set in the given number
 * @param num the number to check
 * @param n the n-th bit to check whenever it is set or not
 * @return true of the n-th bit is set
 */
inline bool is_bit_set(size_t num, size_t n) {
    return 1 == ((num >> n) & 1);
}

/**
 * This is the abstract superclass of the LeafSet implementations.
 *
 * A LeafSet class is used to represent a set of leafs.
 */
template <class LSClass>
class AbstractLeafSet {
public:
    AbstractLeafSet() = default;
    AbstractLeafSet(const AbstractLeafSet&) = default;
    virtual ~AbstractLeafSet() = default;

    /**
     * @brief contains checks whether a leaf is contained in the set
     * @param leaf the leaf, who's membership is tested
     * @return true iff the set contains <leaf>
     */
    virtual bool contains(const size_t leaf) const = 0;

    /**
     * @brief size returns the number of elements in this set
     * @return  the number of elements in this set
     */
    virtual size_t size() const = 0;

    /**
     * @brief pop deletes and returns an element of the set
     * @return an element from the set
     */
    virtual leaf_number pop() = 0;

    /**
     * @brief apply_constraints partitions the set into new sets according to the constraints
     * @param constraints the constraints that must be fulfilled
     */
    virtual void apply_constraints(const std::vector<constraint> &constraints) = 0;

    /**
     * @brief to_set converts the LeafSet to a std::set
     * @return a std::set that contains the leafs in this LeafSet
     */
    virtual inline std::set<leaf_number> to_set() const = 0;
};


/**
 * @brief The BitLeafSet class represents a set of leafs
 *
 * Membership to this class is indicated by a single bit in a bit-array
 */
class BitLeafSet : public AbstractLeafSet<BitLeafSet>, boost::dynamic_bitset<> {
    typedef std::vector<std::shared_ptr<BitLeafSet>> partition_list;
public:
    BitLeafSet() = default;

    /**
     * @brief BitLeafSet creates a new BitLeafSet with the given parameters
     * @param num_bits the number of bits (capacity) of the set
     * @param value the value used to initialize the elements
     */
    BitLeafSet (size_type num_bits, unsigned long value) : boost::dynamic_bitset<>(num_bits, value) {
    }

    /**
     * @brief UnionFindLeafSet creates a LeafSet from an initializer list. Only used for tests
     * @param l the initializer list
     */
    BitLeafSet (std::initializer_list<leaf_number> l)  {
        auto max_val = *std::max_element(l.begin(), l.end());
        this->resize(max_val + 1, 0);
        for(auto elem : l) {
            (*this)[elem] = 1;
        }
    }

    /**
     * @brief BitLeafSet creates a new BitLeafSet with the given size
     * @param size the size of the leafset
     */
    inline BitLeafSet (size_t size) {
        this->resize(size, 1);
    }

    inline bool contains(leaf_number leaf) const {
        return leaf < boost::dynamic_bitset<>::size() && (*this)[leaf];
    }

    inline size_t size() const {
        return this->count();
    }

    inline void insert(const leaf_number l) {
        (*this)[l] = 1;
    }

    inline std::set<leaf_number> to_set() const {
        std::set<leaf_number> set;
        leaf_number pos = this->find_first();
        while (pos != npos) {
            set.insert(pos);
            pos = this->find_next(pos);
        }
        return set;
    }

    inline leaf_number pop() {
        leaf_number pos = this->find_first();
        (*this)[pos] = false;
        return pos;
    }

    std::tuple<std::shared_ptr<BitLeafSet>,
            std::shared_ptr<BitLeafSet> > get_nth_partition_tuple(const size_t n) {

        assert(list_of_partitions.size() > 0);

        auto part_one = this->create_empty();
        auto part_two = this->create_empty();

        assert(n > 0 && n <= number_partition_tuples());

        for (size_t i = 0; i < list_of_partitions.size(); i++) {
            if (is_bit_set(n, i)) {
                part_one->merge(*list_of_partitions[i]);
            } else {
                part_two->merge(*list_of_partitions[i]);
            }
        }

        return std::make_tuple(part_one, part_two);
    }


    /**
     * Returns the number of partition tuples that can be formed by combining the
     * given list of partitions. The formular is 2^(n-1) - 1, where n is the size of
     * the list.
     * @return the number of partition tuples that can be formed from the given list
     */
    inline size_t number_partition_tuples() {
        assert(list_of_partitions.size() > 1);
        return (1 << (list_of_partitions.size() - 1)) - 1;
    }

    void apply_constraints(const std::vector<constraint> &constraints) {
        list_of_partitions = create_partition_list();

        for (constraint cons : constraints) {
            bool found_left_constraint = false;
            bool found_right_constraint = false;
            size_t index_containing_left_constraint = 0;
            size_t index_containing_right_constraint = 0;
             for (size_t i = 0; i < list_of_partitions.size(); i++) {
                if (list_of_partitions[i]->contains(cons.smaller_left)) {
                    // set contains the left constraint
                    found_left_constraint = true;
                    index_containing_left_constraint = i;
                    if(found_right_constraint) {
                        break;
                    }
                }
                if (list_of_partitions[i]->contains(cons.smaller_right)) {
                    // set contains the right constraint
                    found_right_constraint = true;
                    index_containing_right_constraint = i;
                    if(found_left_constraint) {
                        break;
                    }
                }
            }
            assert(found_left_constraint
                    && index_containing_left_constraint < list_of_partitions.size());
            assert(
                    found_right_constraint
                    && index_containing_right_constraint < list_of_partitions.size());
            if (index_containing_left_constraint != index_containing_right_constraint) {
                // sets need to be merged
                list_of_partitions[index_containing_left_constraint]->merge(*list_of_partitions[index_containing_right_constraint]);
                list_of_partitions.erase(list_of_partitions.begin() + static_cast<partition_list::difference_type>(
                                                  index_containing_right_constraint));
            }
        }
    }

    const partition_list& get_list_of_partitions() const {
        return list_of_partitions;
    }

private:
    partition_list list_of_partitions;

    inline
    partition_list create_partition_list() const {
        partition_list sets;
        sets.reserve(this->size());

        leaf_number pos = this->find_first();
        while (pos != npos) {
            // create an empty set for each leaf
            auto set = std::make_shared<BitLeafSet>(boost::dynamic_bitset<>::size(), 0);
            set->insert(pos);
            sets.push_back(set);
            pos = this->find_next(pos);
        }

        return sets;
    }
    inline
    std::shared_ptr<BitLeafSet> create_empty() {
        return std::make_shared<BitLeafSet>(boost::dynamic_bitset<>::size(), 0);
    }

    inline
    void merge(const BitLeafSet  &other) {
        boost::dynamic_bitset<>::operator|=(other);
    }
};

/**
 * @brief The UnionFindLeafSet class is used as a representation of a set of leafs
 *
 * This class internally uses the union find datastructre to efficently implement
 * find and merge operations.
 */
class UnionFindLeafSet  {
    typedef std::vector<std::shared_ptr<UnionFindLeafSet>> partition_list;
    static const size_t invalid_entry = std::numeric_limits<size_t>::max();
public:
    /**
     * @brief UnionFindLeafSet copy constructor
     * @param obj the object
     */
    UnionFindLeafSet(const UnionFindLeafSet& obj) {
        data_structure = obj.data_structure;
        repr = obj.repr;
        repr_valid = obj.repr_valid;
    }

    /**
     * @brief UnionFindLeafSet creates a new LeafSet with the given parameters
     * @param data_structure the union-find datastructre
     * @param repr the number of the leaf that is the representative of the set
     *          in the union-find datastructre
     */
    UnionFindLeafSet(std::shared_ptr<UnionFind> data_structure, size_t repr)
        : data_structure(data_structure), repr(repr), repr_valid(true) {
        assert(repr != invalid_entry);
    }

    /**
     * @brief UnionFindLeafSet creates a new LeafSet with the given union-find structre
     * @param data_structure the union-find datastructre
     */
    UnionFindLeafSet(std::shared_ptr<UnionFind> data_structure) : data_structure(data_structure) {}

    /**
     * @brief UnionFindLeafSet creates a new LeafSet with <num_elems> elements
     * @param num_elems the size of the structure
     */
    UnionFindLeafSet(size_t num_elems) {
        data_structure = std::make_shared<UnionFind>(num_elems);
        for (size_t i = 0; i < data_structure->size(); i++) {
            data_structure->get_parent().at(i) = 0;
        }
        repr = 0;
        repr_valid = true;
    }

    /**
     * @brief UnionFindLeafSet creates a LeafSet from an initializer list. Only used for tests
     * @param l the initializer list
     */
    UnionFindLeafSet (std::initializer_list<leaf_number> l)  {
        auto max_val = *std::max_element(l.begin(), l.end());
        data_structure = std::make_shared<UnionFind>(max_val + 1);

        repr = *(l.begin());
        for(size_t i = 1; i < l.size(); i++) {
            data_structure->get_parent().at(i) = repr;
        }
        repr_valid = true;
    }

    bool contains(size_t leaf) const {
        assert(repr_valid);
        assert(leaf != invalid_entry);

        return data_structure->find(leaf) == repr;
    }

    inline size_t size() const {
        assert(repr != invalid_entry);

        if (!repr_valid) {  //in this case we dont really represent a set
            return 0;
        } else {
            size_t count = 0;
            for (size_t i = 0; i < data_structure->size(); i++) {
                if (data_structure->find(i) == repr) {
                    count++;
                }
            }
            return count;
        }
    }

    inline std::set<leaf_number> to_set() const {
        assert(repr != invalid_entry);

        std::set<leaf_number> set;
        if (!repr_valid) {
            return set;
        }
        for (size_t i = 0; i < data_structure->size(); i++) {
            if (data_structure->find(i) == repr) {
                set.insert(i);
            }
        }
        return set;
    }

    inline leaf_number pop() {
        assert(repr != invalid_entry);
        assert(repr_valid);

        //Possible Imrpovement: Use pointers that point from the parent to its childs
        if (this->size() == 1) {
            repr_valid = false;
            data_structure->get_parent().at(repr) = invalid_entry;
            return repr;
        }

        //this is needed to make sure the chain of parent pointers is not broken, if we delete elements
        //this works, because find has path compression and sets the parent to the representative
        //Possible Improvement: we dont have to do this for every call of pop(). We can do this once and remember that we already did it.
        //  but we would have to be careful to forget it, e.g. if we merge to sets.
        for (size_t i = 0; i < data_structure->size(); i++) {
            data_structure->find(i);
        }

        for (size_t i = 0; i < data_structure->size(); i++) {
            if (i == repr) {
                continue;
            }
            else if (data_structure->find(i) == repr) {
                data_structure->get_parent().at(i) = invalid_entry;
                assert(i != invalid_entry);
                return i;
            }
        }
        assert(0);  //this means that we have a valid set without any element
        return 0;
    }


    /**
     * Returns the number of partition tuples that can be formed by combining the
     * given list of partitions. The formular is 2^(n-1) - 1, where n is the size of
     * the list.
     * @return the number of partition tuples that can be formed from the given list
     */
    inline size_t number_partition_tuples() const {
        assert(repr != invalid_entry);
        assert(list_of_partitions.size() > 1);

        return (1 << (list_of_partitions.size() - 1)) - 1;
    }

    void apply_constraints(const std::vector<constraint> &constraints) {
        assert(list_of_partitions.size() == 0);
        assert(repr != invalid_entry);

        //Possible Improvement: Implement pointers that point from a parent to its child.
            //this might speed up some things asymptotically, but has of course a constant slow down.
        //set all elements that are not in this set to invalid
        for (size_t i = 0; i < data_structure->size(); i++) {
            if (data_structure->find(i) != repr) {
                data_structure->get_parent().at(i) = invalid_entry;
            }
        }

        data_structure->allToSingletons();
        repr_valid = false;

        for (constraint cons : constraints) {
            assert(cons.smaller_left < data_structure->size() && cons.smaller_right < data_structure->size());
            assert(cons.smaller_left != invalid_entry && cons.smaller_right != invalid_entry);
            repr = data_structure->merge(cons.smaller_left, cons.smaller_right);
        }

        //Possible improvement: use bool-arrays for this
        //create a list of all distinct sets
        std::set<leaf_number> sets;
        for (size_t i = 0; i < data_structure->size(); i++) {
            if (data_structure->get_parent().at(i) != invalid_entry) {
                leaf_number set_repr = data_structure->find(i);
                assert(set_repr != invalid_entry);
                sets.insert(set_repr);
            }
        }
        for (std::set<leaf_number>::iterator it=sets.begin(); it != sets.end(); ++it) {
            list_of_partitions.push_back(*it);
        }

    }

    /**
     * @brief get_nth_partition_tuple creates two new LeafSets, that represent the nth partition of the set of sets
     * @param n the number of the required partition
     * @return two new LeafSets that represent the nth partition
     */
    std::tuple<std::shared_ptr<UnionFindLeafSet>, std::shared_ptr<UnionFindLeafSet>>
    get_nth_partition_tuple(size_t n) const {
        assert(repr != invalid_entry);

        //Possible improvement: it may be possible to copy the datastructure only once, since the two sets are disjoint
        assert(n > 0 && n <= number_partition_tuples());
        assert(list_of_partitions.size() > 1);  //we need at least 2 sets, otherwise creating partitions does not make sense

        std::shared_ptr<UnionFind> uf_1 = std::make_shared<UnionFind>(data_structure->get_parent(), data_structure->get_rank());
        std::shared_ptr<UnionFind> uf_2 = std::make_shared<UnionFind>(data_structure->get_parent(), data_structure->get_rank());

        std::shared_ptr<UnionFindLeafSet> part_one = std::make_shared<UnionFindLeafSet>(uf_1);
        std::shared_ptr<UnionFindLeafSet> part_two = std::make_shared<UnionFindLeafSet>(uf_2);

        bool part_one_first_set = false;
        bool part_two_first_set = false;

        for (size_t i = 0; i < list_of_partitions.size(); i++) {
          if (is_bit_set(n, i)) { //put the set into part_one
              if (!part_one_first_set) {
                  part_one->repr = list_of_partitions.at(i);
                  assert(part_one->repr != invalid_entry);
                  part_one->repr_valid = true;
                  part_one_first_set = true;
              } else {
                  part_one->repr = part_one->data_structure->merge(part_one->repr, list_of_partitions.at(i));
                  assert(part_one->repr != invalid_entry);
              }
          } else {    //put the set into part_two
              if (!part_two_first_set) {
                  part_two->repr = list_of_partitions.at(i);
                  assert(part_two->repr != invalid_entry);
                  part_two->repr_valid = true;
                  part_two_first_set = true;
              } else {
                  part_two->repr = part_two->data_structure->merge(part_two->repr, list_of_partitions.at(i));
                  assert(part_two->repr != invalid_entry);
              }
          }
        }

        return std::tuple<std::shared_ptr<UnionFindLeafSet>, std::shared_ptr<UnionFindLeafSet>>(part_one, part_two);
    }

    partition_list get_list_of_partitions() const {
        assert(repr != invalid_entry);
        partition_list ret;

        for (size_t elem : list_of_partitions) {
            std::shared_ptr<UnionFindLeafSet> temp = std::make_shared<UnionFindLeafSet>(this->data_structure, elem);
            ret.push_back(temp);
        }

        return ret;
    }


private:
    std::shared_ptr<UnionFind> data_structure;  //pointer to the UnionFind datastructure
    size_t repr; //this is the number of the representative of this set. its an index to the array of the union find data structure
    bool repr_valid = false; //true iff the representative is valid
    std::vector<leaf_number> list_of_partitions;  //contains the id's of the representatives of the sets
};
