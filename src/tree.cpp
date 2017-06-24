#include "tree.h"

#include <string>
#include <sstream>
#include <memory>
#include <map>

std::string Leaf::to_newick_string(const std::vector<std::string> &leaf_to_label) {
    return leaf_to_label[this.leaf_id];
}

std::string Node::to_newick_string(const std::vector<std::string> &leaf_to_label) {
    std::stringstream ss;
    ss << "(";
    ss << this.left.to_newick_string(leaf_to_label);
    ss << ",";
    ss << this.right.to_newick_string(leaf_to_label);
    ss << ")";
    return ss.str();
}

std::string UnrootedNode::to_newick_string(const std::vector<std::string> &leaf_to_label) {
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
