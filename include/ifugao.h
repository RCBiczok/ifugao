#pragma once
#include "types.h"

#include <iostream>
#include <iterator>
#include <gmpxx.h>
#include <omp.h>

#define PARALLEL_THRESHOLD 50

/**
 * Returns a vector containing all constraints that still are valid for the given set of leaves.
 *
 * @param leaves Leaves to search valid constraints for.
 * @param constraints All constraints that could still be valid.
 * @return All constraints that are still valid.
 */
std::vector<constraint> find_constraints(const LeafSet &leaves,
                                         const std::vector<constraint> &constraints);


template <typename ResultType>
class TerraceAlgorithm {
public:
    virtual ~TerraceAlgorithm() = default;

    ResultType scan_terrace(LeafSet &leaves, const std::vector<constraint> &constraints,
                            bool unrooted = false) {
        if (constraints.empty()) {
            return scan_unconstraint_leaves(leaves, unrooted);
        } else {
            leaves.apply_constraints(constraints);
            if (leaves.size() > PARALLEL_THRESHOLD) {
                return traverse_partitions_parallel(constraints, leaves, unrooted);
            } else {
                return traverse_partitions(constraints, leaves, unrooted);
            }
        }
    }

protected:
    virtual ResultType traverse_partitions(const std::vector<constraint> &constraints,
                                           LeafSet &leaves,
                                           bool unrooted = false) = 0;

    virtual ResultType traverse_partitions_parallel(
            const std::vector<constraint> &constraints, LeafSet &leaves,
            bool unrooted = false) = 0;

    virtual ResultType scan_unconstraint_leaves(LeafSet &leaves,
                                                bool unrooted = false) = 0;

    ResultType scan_terrace_internal(LeafSet &leaves,
                                     const std::vector<constraint> &constraints) {
        if (constraints.empty()) {
            return scan_unconstraint_leaves(leaves);
        } else {
            leaves.apply_constraints(constraints);
            return traverse_partitions(constraints, leaves);
        }
    }
/*
    inline
    std::tuple<ResultType, ResultType> get_nth_partition_results(
            size_t i,
            const std::vector<constraint> &constraints,
            const LeafSet &leaves) {
        std::shared_ptr<LeafSet> set_left;
        std::shared_ptr<LeafSet> set_right;
        std::tie(set_left, set_right) = leaves.get_nth_partition_tuple(i);

        auto constraints_left = find_constraints(*set_left, constraints);
        auto constraints_right = find_constraints(*set_right, constraints);

        ResultType subtrees_left =
                scan_terrace_internal(*set_left, constraints_left);
        ResultType subtrees_right =
                scan_terrace_internal(*set_right, constraints_right);

        return std::make_tuple(subtrees_left, subtrees_right);
    }*/
};






































template <typename ResultType, typename CollectType = ResultType>
class TerraceTreeAlgorithm : public TerraceAlgorithm<ResultType> {
protected:
    inline
    ResultType traverse_partitions(const std::vector<constraint> &constraints,
                                   LeafSet &leaves,
                                   bool unrooted = false) {
        CollectType found_trees;
        
        for (size_t i = 1; i <= leaves.number_partition_tuples(); i++) {
            std::shared_ptr<LeafSet> set_left;
            std::shared_ptr<LeafSet> set_right;
            std::tie(set_left, set_right) = leaves.get_nth_partition_tuple(i);

            auto constraints_left = find_constraints(*set_left, constraints);
            auto constraints_right = find_constraints(*set_right, constraints);

            ResultType subtrees_left =
                    this->scan_terrace_internal(*set_left, constraints_left);
            ResultType subtrees_right =
                    this->scan_terrace_internal(*set_right, constraints_right);

            //std::tuple<ResultType, ResultType> subtrees =
              //      this->get_nth_partition_results(i, constraints, leaves);
            //auto subtrees_left = std::get<0>(subtrees);
            //auto subtrees_right = std::get<1>(subtrees);

            auto trees = combine_left_right(subtrees_left, subtrees_right);
            aggregate_trees(found_trees, trees);
        }
        
        return finalize_collect_type(found_trees, unrooted);
    }

    inline
    ResultType traverse_partitions_parallel(
            const std::vector<constraint> &constraints,
            LeafSet &leaves,
            bool unrooted = false) {
        CollectType found_trees;
        // holds the aggregated result for each thread
        std::vector<CollectType> aggregations;
        #pragma omp parallel
        {
            #pragma omp master
            {
                // initialize the shared array (once)
                size_t thread_amount = omp_get_num_threads();
                aggregations = std::vector<CollectType>(thread_amount);
            }
            size_t thread_number = omp_get_thread_num();
            
            size_t amount_loops = leaves.number_partition_tuples();
            #pragma omp for
            for (size_t i = 1; i <= amount_loops; i++) {
                std::shared_ptr<LeafSet> set_left;
                std::shared_ptr<LeafSet> set_right;
                std::tie(set_left, set_right) = leaves.get_nth_partition_tuple(i);

                auto constraints_left = find_constraints(*set_left, constraints);
                auto constraints_right = find_constraints(*set_right, constraints);

                ResultType subtrees_left =
                        this->scan_terrace_internal(*set_left, constraints_left);
                ResultType subtrees_right =
                        this->scan_terrace_internal(*set_right, constraints_right);

                //auto subtrees =
                  //      this->get_nth_partition_results(i, constraints, leaves);
                //auto subtrees_left = std::get<0>(subtrees);
                //auto subtrees_right = std::get<1>(subtrees);

                const auto trees = combine_left_right(subtrees_left,
                                                      subtrees_right);
                aggregate_trees(aggregations[thread_number], trees);
            }
            
            #pragma omp critical
            {
                combine_multiple_results(found_trees, aggregations[thread_number]);
            }
        }
        return finalize_collect_type(found_trees, unrooted);
    }

    virtual ResultType scan_unconstraint_leaves(LeafSet &leaves,
                                                bool unrooted = false) = 0;

    virtual ResultType combine_left_right(const ResultType &left_part,
                                            const ResultType &right_part) = 0;

    virtual void aggregate_trees(CollectType &aggregation,
                                 const ResultType &new_result) = 0;

    virtual void combine_multiple_results(CollectType &aggregation_1,
                                          const CollectType &aggregation_2) = 0;

    virtual ResultType finalize_collect_type(CollectType &aggregation,
                                             bool unrooted) = 0;
};

typedef std::vector<InnerNodePtr> InnerNodeList;
class FindCompressedTree : public TerraceTreeAlgorithm<Tree, InnerNodeList> {
protected:

    inline
    Tree scan_unconstraint_leaves(LeafSet &leaves,
                                  bool unrooted = false) override {
        // TODO reconsider the whole std::set usage vs std::vector
        auto set = leaves.to_set();
        std::vector<leaf_number> leaves_vec(set.begin(), set.end());
        const auto result =
                std::make_shared<AllLeafCombinationsNode>(leaves_vec);
        if(unrooted) {
            return std::make_shared<UnrootedNode>(result);
        } else {
            return result;
        }
    }

    inline
    Tree combine_left_right(const Tree &left_part,
                              const Tree &right_part) override {
        return std::make_shared<InnerNode>(left_part, right_part);
    }

    inline
    void aggregate_trees(InnerNodeList &aggregation,
                         const Tree &new_result) override {
        aggregation.push_back(std::static_pointer_cast<InnerNode>(new_result));
    }

    inline
    void combine_multiple_results(InnerNodeList &aggregation_1,
                                  const InnerNodeList &aggregation_2) {
        aggregation_1.insert(aggregation_1.end(), aggregation_2.begin(),
                             aggregation_2.end());
    }

    inline
    Tree finalize_collect_type(InnerNodeList &aggregation,
                               bool unrooted = false) override {
        assert(aggregation.size() > 0);
        
        if (aggregation.size() == 1) {
            return aggregation[0];
            // One leaf can not be a root, no unrooted check required
            // (scan_unconstraint_leaves() would have been called)
        } else {
            AllTreeCombinationsNodePtr combi_node =
                    std::make_shared<AllTreeCombinationsNode>(aggregation);
            if (unrooted) {
                return std::make_shared<UnrootedNode>(combi_node);
            } else {
                return combi_node;
            }
        }
    }
};

typedef std::vector<Tree> TreeList;
class FindAllRootedTrees : public TerraceTreeAlgorithm<std::vector<Tree>> {
protected:
    inline
    std::vector<Tree> scan_unconstraint_leaves(
            LeafSet &leaves, bool unrooted = false) override {
        std::vector<Tree> binary_trees = get_all_binary_trees(leaves);
        
        if(unrooted) {
            for (size_t i = 0; i < binary_trees.size() ; ++i) {
                const auto inner =
                    std::static_pointer_cast<InnerNode>(binary_trees[i]);
                binary_trees[i] = std::make_shared<UnrootedNode>(inner);
            }
        }
        
        return binary_trees;
    }

    inline
    std::vector<Tree> combine_left_right(const std::vector<Tree> &left_part,
                                         const std::vector<Tree> &right_part
                                         ) override {
        return merge_subtrees(left_part, right_part);
    }

    inline
    void aggregate_trees(std::vector<Tree> &aggregation,
                         const std::vector<Tree> &new_result) override {
        aggregation.insert(aggregation.end(), new_result.begin(),
                           new_result.end());
    }

    inline
    void combine_multiple_results(std::vector<Tree> &aggregation_1,
                                  const std::vector<Tree> &aggregation_2) {
        aggregation_1.insert(aggregation_1.end(), aggregation_2.begin(),
                             aggregation_2.end());
    }

    inline
    std::vector<Tree> finalize_collect_type(std::vector<Tree> &aggregation,
                                   bool unrooted = false) override {
        if(unrooted) {
            for (size_t i = 0; i < aggregation.size(); ++i) {
                // Guaranteed to work, otherwise no constraints would exist and
                // this method wouldn't get called.
                auto node = std::static_pointer_cast<InnerNode>(aggregation[i]);
                aggregation[i] = std::make_shared<UnrootedNode>(node);
            }
        }
        return aggregation;
    }

public:
    /**
     * Returns all possible binary trees that can be combined by using the
     * leaves in the LeafSet.
     */
    static std::vector<Tree> get_all_binary_trees(LeafSet &leaves);

    /**
     * Adds a leaf to a tree at all possible positions, leading to a list of
     * resulting trees.
     */
    static std::vector<Tree> add_leaf_to_tree(const Tree &current_tree,
                                     const LeafPtr &leaf);

    /** merges two sub-trees */
    static std::vector<Tree> merge_subtrees(const std::vector<Tree> &left, const std::vector<Tree> &right);
};

class CountAllRootedTrees : public TerraceAlgorithm<mpz_class> {
protected:
    inline
    mpz_class traverse_partitions(const std::vector<constraint> &constraints,
                                  LeafSet &leaves,
                                  bool unrooted = false) override {
        mpz_class count = mpz_class(0);
        
        for (size_t i = 1; i <= leaves.number_partition_tuples(); ++i) {
            std::shared_ptr<LeafSet> set_left;
            std::shared_ptr<LeafSet> set_right;
            std::tie(set_left, set_right) = leaves.get_nth_partition_tuple(i);

            auto constraints_left = find_constraints(*set_left, constraints);
            auto constraints_right = find_constraints(*set_right, constraints);

            mpz_class amount_possible_trees_left =
                    this->scan_terrace_internal(*set_left, constraints_left);
            mpz_class amount_possible_trees_right =
                    this->scan_terrace_internal(*set_right, constraints_right);

            //const std::tuple<mpz_class, mpz_class> subtrees =
              //      this->get_nth_partition_results(i, constraints, leaves);
            //mpz_class amount_possible_trees_left = std::get<0>(subtrees);
            //mpz_class amount_possible_trees_right = std::get<1>(subtrees);
            count += amount_possible_trees_left * amount_possible_trees_right;
        }
        
        return count;
    }

    inline
    mpz_class traverse_partitions_parallel(
            const std::vector<constraint> &constraints, LeafSet &leaves,
            bool unrooted = false) override {
        // TODO parallelization
        return traverse_partitions(constraints, leaves, unrooted);
    }

    inline
    mpz_class scan_unconstraint_leaves(LeafSet &leaves,
                                       bool) override {
        return Node::number_of_binary_trees(leaves.size());
    }
};

class CheckIfTerrace : public TerraceAlgorithm<bool> {
protected:
    inline
    bool traverse_partitions(const std::vector<constraint> &constraints,
                             LeafSet &leaves,
                             bool unrooted = false) override {
        if(leaves.number_partition_tuples() > 1) {
            return true;
        }
        bool found_terrace = false;
        
        for (size_t i = 1; !found_terrace 
                           && i <= leaves.number_partition_tuples(); ++i) {
            std::shared_ptr<LeafSet> set_left;
            std::shared_ptr<LeafSet> set_right;
            std::tie(set_left, set_right) = leaves.get_nth_partition_tuple(i);

            auto constraints_left = find_constraints(*set_left, constraints);
            auto constraints_right = find_constraints(*set_right, constraints);

            bool subtrees_left =
                    this->scan_terrace_internal(*set_left, constraints_left);
            bool subtrees_right =
                    this->scan_terrace_internal(*set_right, constraints_right);

            //const std::tuple<bool, bool> subtrees =
              //      this->get_nth_partition_results(i, constraints, leaves);
            //found_terrace |= std::get<0>(subtrees) || std::get<1>(subtrees);
            found_terrace |= subtrees_left || subtrees_right;
        }
        
        return found_terrace;
    }

    inline
    bool traverse_partitions_parallel(
            const std::vector<constraint> &constraints, LeafSet &leaves,
            bool unrooted = false) override {
        // TODO parallelization
        return traverse_partitions(constraints, leaves, unrooted);
    }

    inline
    bool scan_unconstraint_leaves(LeafSet &leaves, bool) override {
        return leaves.size() >= 3;
    }
};
