#include "ifugao.h"

//#include <terraces.h>
#include <map>
#include "leaf_set.h"

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
        for (const std::shared_ptr<Tree> t : all_trees) {
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
    
    // TODO abstract into leaf_set.h
    //part_left.set.set(root_id);
    //part_right = part_left.get_complementing_leaf_set_to_base(leaf_set);
    
    auto constraints_left = find_constraints(part_left, constraints);
    auto constraints_right = find_constraints(part_right, constraints);
    
    auto subtrees_left = find_all_rooted_trees(part_left, constraints_left);
    auto subtrees_right = find_all_rooted_trees(part_right, constraints_right);
    
    return merge_subtrees(subtrees_left, subtrees_right);
}

//static std::vector<std::shared_ptr<Tree> > add_leaf_to_tree(
//        std::shared_ptr<Node> current_node, const size_t leaf) {
//    std::vector<std::shared_ptr<Tree> > result;
//    /* this is not what we want. no copying, only pointer reusing is desired
//TODO rewrite
//    if (!current_node->is_leaf()) {
//        auto result_left = add_leaf_to_tree(current_node->left, leaf);
//        result.insert(result.end(), result_left.begin(), result_left.end());
//        auto result_right = add_leaf_to_tree(current_node->right, leaf);
//        result.insert(result.end(), result_right.begin(), result_right.end());
//    }

//    auto tree_copy = deep_copy(current_node);

//    auto to_insert = std::make_shared<Tree>(leaf);
//    auto new_tree = std::make_shared<Tree>(tree_copy, to_insert);
//    new_tree->parent = tree_copy->parent;
//    if (tree_copy->parent != nullptr) {
//        if (tree_copy->parent->left == tree_copy) {
//            tree_copy->parent->left = new_tree;
//        } else {
//            tree_copy->parent->right = new_tree;
//        }
//    }
//    tree_copy->parent = new_tree;
//    to_insert->parent = new_tree;

//    result.push_back(root(new_tree));
//*/
//    return result;
//}

std::vector<std::shared_ptr<Node> > get_all_binary_trees(
        const LeafSet &leaf_set) {
    std::vector<std::shared_ptr<Node> > result;
    if (leaf_set.size() == 0) {
        return result;
    }
/* TODO
    auto itr = leaf_set.begin();
    size_t next_leaf = *itr;
    itr++;
    LeafSet rest_leaf_set(itr, leaf_set.end());

    if (leaf_set.size() == 1) {
        auto t = std::make_shared<Tree>(next_leaf);
        result.push_back(t);
        return result;
    }

    for (auto t : get_all_binary_trees(rest_leaf_set)) {
        auto new_trees = add_leaf_to_tree(t, next_leaf);
        result.insert(result.end(), new_trees.begin(), new_trees.end());
    }
*/
    return result;
}

std::vector<std::shared_ptr<Node> > find_all_rooted_trees(
        LeafSet &leaf_set,
        const std::vector<constraint> &constraints) {
    if (constraints.empty()) {
        // no more constraints, return all possible combinations
        return get_all_binary_trees(leaf_set);
    }

    leaf_set.apply_constraints(constraints);
    std::vector<std::shared_ptr<Node> > result;

    for (size_t i = 1; i <= leaf_set.number_partition_tuples(); i++) {
        LeafSet part_left;
        LeafSet part_right;
        std::tie(part_left, part_right) =
                leaf_set.get_nth_partition_tuple(i);

        auto constraints_left = find_constraints(part_left, constraints);
        auto constraints_right = find_constraints(part_right, constraints);

        auto subtrees_left = find_all_rooted_trees(part_left, constraints_left);
        auto subtrees_right = find_all_rooted_trees(part_right, constraints_right);
        auto trees = merge_subtrees(subtrees_left, subtrees_right);

        result.insert(result.end(), trees.begin(), trees.end());
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

//static std::shared_ptr<Tree> root(std::shared_ptr<Tree> t) {
//    if (t->parent == nullptr) {
//        return t;
//    }

//    return root(t->parent);
//}
