#include "ifugao.h"

//#include <terraces.h>
#include <map>

size_t list_trees(const std::vector<constraint> &constraints,
                  const size_t &root_id,
                  const LeafSet &leaf_set,
                  const std::vector<std::string> &leaf_to_label,
                  FILE *file) {
    /*//unroot tree
    leaf_set.remove_leaf(root_id);
    auto subtrees = find_all_rooted_trees(leaf_set, constraints);*/
    auto all_trees = find_all_unrooted_trees(leaf_set, constraints, root_id);

    if (file != nullptr) { // nullptr means just counting
        for (std::shared_ptr<Tree> t : all_trees) {
            fprintf(file, "%s\n", t->to_newick_string(leaf_to_label).c_str());
        }
    }
    return all_trees.size();
}

std::vector<std::shared_ptr<Tree>> find_all_unrooted_trees(
        const LeafSet &leaf_set, const std::vector<constraint> &constraints,
        const size_t &root_id) {
    //TODO assert(leaf_set.count(root_id) > 0);

    LeafSet part_left;
    LeafSet part_right;
    
    part_left.insert_leaf(root_id);
    part_right = part_left.get_complementing_leaf_set_to_base(leaf_set);
    
    auto constraints_left = find_constraints(part_left, constraints);
    auto constraints_right = find_constraints(part_right, constraints);
    
    auto subtrees_left = find_all_rooted_trees(part_left, constraints_left);
    auto subtrees_right = find_all_rooted_trees(part_right, constraints_right);
    
    return merge_subtrees(subtrees_left, subtrees_right);
}

std::vector<std::shared_ptr<Node> > find_all_rooted_trees(
        const LeafSet &leaf_set,
        const std::vector<constraint> &constraints) {
    std::vector<std::shared_ptr<Tree> > result;
    if (constraints.empty()) {
        // no more constraints, create a Node for all possible combinations
        auto leaves = leaf_set.getAllLeaves();
        auto tree = make_shared<AllBinaryCombinationsNode>(leaves);
        result.push_back(tree);
    } else {
        auto partitions = apply_constraints(leaf_set, constraints);

        for (size_t i = 1; i <= number_partition_tuples(partitions); i++) {
            std::shared_ptr<LeafSet> part_left;
            std::shared_ptr<LeafSet> part_right;
            //TODO compression would go here and probably look sth like:
            /*if(!leaf_set.isDenseEnough()) {
                std::tuple new_leaf_set = leaf_set.getCompressedSet();
                compressed=true;
            }
            // normal steps
            
            if(compressed) {
                //uncompress
            }
            
            Also a vector containing all leaves is probably helpful, so we don't 
            need to search for them in the trees but simply have a vector. 
            
            
            */
            std::tie(part_left, part_right) =
                    leaf_set.get_nth_partition_tuple(partitions,i);

            auto constraints_left = find_constraints(part_left, constraints);
            auto constraints_right = find_constraints(part_right, constraints);

            auto subtrees_left = find_all_rooted_trees(part_left, constraints_left);
            auto subtrees_right = find_all_rooted_trees(part_right, constraints_right);
            auto trees = merge_subtrees(subtrees_left, subtrees_right);

            result.insert(result.end(), trees.begin(), trees.end());
        }

    }
    return result;
}

std::vector<std::shared_ptr<Tree> > merge_subtrees(
        std::vector<std::shared_ptr<Node> > &left,
        std::vector<std::shared_ptr<Node> > &right) {

    std::vector<std::shared_ptr<Tree> > merged_trees;

    for (std::shared_ptr<Node> &l : left) {
        for (std::shared_ptr<Node> &r : right) {
            auto new_tree = std::make_shared<Tree>(l, r);
            merged_trees.push_back(new_tree);
        }
    }

    assert(merged_trees.size() == left.size() * right.size());
    return merged_trees;
}
