#include "tree.h"

#include <string>
#include <sstream>
#include <memory>
#include <map>

std::string Leaf::to_newick_string(std::vector<std::string> &leaf_to_label) {
    return leaf_to_label[this.leaf_id];
}

std::string Node::to_newick_string(std::vector<std::string> &leaf_to_label) {
    std::stringstream ss;
    ss << "(";
    ss << this.left.to_newick_string(leaf_to_label);
    ss << ",";
    ss << this.right.to_newick_string(leaf_to_label);
    ss << ")";
    return ss.str();
}

std::string UnrootedNode::to_newick_string(std::vector<std::string> &leaf_to_label) {
    std::stringstream ss;
    ss << "(";
    ss << this.elem1.to_newick_string(leaf_to_label);
    ss << ",";
    ss << this.elem2.to_newick_string(leaf_to_label);
    ss << ",";
    ss << this.elem3.to_newick_string(leaf_to_label);
    ss << ")";
    return ss.str();
}

//TODO maybe still needed
/*static std::shared_ptr<Tree> deep_copy(std::shared_ptr<Tree> tree,
        std::map<std::shared_ptr<Tree>, std::shared_ptr<Tree>> &cover_map) {
    if (tree == nullptr) {
        return nullptr;
    }
    if (cover_map.count(tree) > 0) {
        return cover_map[tree];
    }

    auto node = std::make_shared<Tree>();
    node->label = tree->label;
    cover_map[tree] = node;
    cover_map[node] = node;

    node->parent = deep_copy(tree->parent, cover_map);
    node->left = deep_copy(tree->left, cover_map);
    node->right = deep_copy(tree->right, cover_map);

    return node;
}

std::shared_ptr<Tree> deep_copy(std::shared_ptr<Tree> tree) {
    std::map<std::shared_ptr<Tree>, std::shared_ptr<Tree>> cover_map;
    return deep_copy(tree, cover_map);
}*/
