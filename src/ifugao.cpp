#include "ifugao.h"

//#include <terraces.h>
#include <map>

size_t list_trees(const std::vector<constraint> &constraints,
                  const leaf &root_id,
                  const leaf_set &leaves,
                  const std::vector<std::string> &leaf_to_label,
                  FILE *file) {
    auto all_trees = find_all_unrooted_trees(leaves, constraints, root_id);

    if (file != nullptr) { // nullptr means just counting
        for (std::shared_ptr<Tree> t : all_trees) {
            fprintf(file, "%s\n", t->to_newick_string(leaf_to_label).c_str());
        }
    }
    return all_trees.size();
}

std::vector<std::shared_ptr<Tree>> find_all_unrooted_trees(
        const leaf_set &leaves, const std::vector<constraint> &constraints,
        const leaf &root_id) {
    //TODO assert(leaves.count(root_id) > 0);

    auto part_left = std::make_shared<leaf_set>();
    auto part_right = std::make_shared<leaf_set>();

    part_left->insert(root_species_name);
    part_right->insert(leaves.begin(), leaves.end());
    part_right->erase(root_species_name);

    auto constraints_left = find_constraints(*part_left, constraints);
    auto constraints_right = find_constraints(*part_right, constraints);

    auto subtrees_left = find_all_rooted_trees(*part_left, constraints_left);
    auto subtrees_right = find_all_rooted_trees(*part_right, constraints_right);
    auto trees = merge_subtrees(subtrees_left, subtrees_right);

    std::vector<std::shared_ptr<Tree> > result;
    result.reserve(trees.size());
    for (auto &t : trees) {
        result.push_back(std::make_shared<Tree>(t));
    }

    return result;
}

static std::vector<std::shared_ptr<Tree> > add_leaf_to_tree(
        std::shared_ptr<Tree> current_tree, const leaf leaf) {
    std::vector<std::shared_ptr<Tree> > result;

    if (!current_tree->is_leaf()) {
        auto result_left = add_leaf_to_tree(current_tree->left, leaf);
        result.insert(result.end(), result_left.begin(), result_left.end());
        auto result_right = add_leaf_to_tree(current_tree->right, leaf);
        result.insert(result.end(), result_right.begin(), result_right.end());
    }

    auto tree_copy = deep_copy(current_tree);

    auto to_insert = std::make_shared<Tree>(leaf);
    auto new_tree = std::make_shared<Tree>(tree_copy, to_insert);
    new_tree->parent = tree_copy->parent;
    if (tree_copy->parent != nullptr) {
        if (tree_copy->parent->left == tree_copy) {
            tree_copy->parent->left = new_tree;
        } else {
            tree_copy->parent->right = new_tree;
        }
    }
    tree_copy->parent = new_tree;
    to_insert->parent = new_tree;

    result.push_back(root(new_tree));

    return result;
}

std::vector<std::shared_ptr<Tree> > get_all_binary_trees(
        const leaf_set &leaves) {

    std::vector<std::shared_ptr<Tree> > result;
    if (leaves.size() == 0) {
        return result;
    }

    auto itr = leaves.begin();
    leaf next_leaf = *itr;
    itr++;
    leaf_set rest_leaves(itr, leaves.end());

    if (leaves.size() == 1) {
        auto t = std::make_shared<Tree>(next_leaf);
        result.push_back(t);
        return result;
    }

    for (auto t : get_all_binary_trees(rest_leaves)) {
        auto new_trees = add_leaf_to_tree(t, next_leaf);
        result.insert(result.end(), new_trees.begin(), new_trees.end());
    }

    return result;
}

/*
std::tuple<std::shared_ptr<leaf_set>,
        std::shared_ptr<leaf_set> > get_nth_partition_tuple(
        std::vector<std::shared_ptr<leaf_set> > &partitions,
        size_t n) {

    auto part_one = std::make_shared<leaf_set>();
    auto part_two = std::make_shared<leaf_set>();

    assert(n > 0 && n <= number_partition_tuples(partitions));

    for (size_t i = 0; i < partitions.size(); i++) {
        if (is_bit_set(n, i)) {
            part_one->insert(partitions[i]->begin(), partitions[i]->end());
        } else {
            part_two->insert(partitions[i]->begin(), partitions[i]->end());
        }
    }

    return std::make_tuple(part_one, part_two);
}
*/

std::vector<std::shared_ptr<Tree> > find_all_rooted_trees(
        const leaf_set &leaves,
        const std::vector<constraint> &constraints) {

    if (constraints.empty()) {
        auto result = get_all_binary_trees(leaves);
        return result;
    }

    auto partitions = apply_constraints(leaves, constraints);
    std::vector<std::shared_ptr<Tree> > result;

    for (size_t i = 1; i <= number_partition_tuples(partitions); i++) {
        std::shared_ptr<leaf_set> part_left;
        std::shared_ptr<leaf_set> part_right;
        std::tie(part_left, part_right) = get_nth_partition_tuple(partitions,
                                                                  i);

        auto constraints_left = find_constraints(*part_left, constraints);
        auto constraints_right = find_constraints(*part_right, constraints);

        auto subtrees_left = find_all_rooted_trees(*part_left, constraints_left);
        auto subtrees_right = find_all_rooted_trees(*part_right, constraints_right);
        auto trees = merge_subtrees(subtrees_left, subtrees_right);

        result.insert(result.end(), trees.begin(), trees.end());
    }

    return result;
}

std::vector<std::shared_ptr<Tree> > merge_subtrees(
        std::vector<std::shared_ptr<Tree> > &left,
        std::vector<std::shared_ptr<Tree> > &right) {

    std::vector<std::shared_ptr<Tree> > merged_trees;

    for (std::shared_ptr<Tree> &l : left) {
        for (std::shared_ptr<Tree> &r : right) {
            auto new_tree = std::make_shared<Tree>(l, r);
            l->parent = new_tree;
            r->parent = new_tree;
            merged_trees.push_back(new_tree);
        }
    }

    assert(merged_trees.size() == left.size() * right.size());
    return merged_trees;
}

static std::shared_ptr<Tree> root(std::shared_ptr<Tree> t) {
    if (t->parent == nullptr) {
        return t;
    }

    return root(t->parent);
}
