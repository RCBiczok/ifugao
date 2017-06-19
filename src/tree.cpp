#include "tree.h"

#include <string>
#include <sstream>
#include <memory>
#include <map>

std::string Leaf::to_newick_string(std::vector<std::string> &leaf_to_label) {
    return leaf_to_label[this.leaf_id];
}

std::string InnerNode::to_newick_string(std::vector<std::string> &leaf_to_label) {
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

std::string AllBinaryCombinationsNode::to_newick_string(std::vector<std::string> &leaf_to_label) {
    assert(this.leaves.size()>0);
    
    std::stringstream ss;
    ss << "{";
    ss << leaf_to_label[this.leaves[0]; // assert guarantees existance
    for(size_t i = 1; i < leaves.size(); ++i) {
        ss << "," << leaf_to_label[this.leaves[i]];
    }
    ss << "}";
    return ss.str();
}
