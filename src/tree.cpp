#include "tree.h"

#include <assert.h>
#include <sstream>

#include "leaf_label_mapper.h"

std::string Leaf::to_newick_string() {
    return LeafLabelMapper::get_label_from_leaf_id(this->leaf_id);
}

std::string InnerNode::to_newick_string() {
    std::stringstream ss;
    ss << "(";
    ss << this->left->to_newick_string();
    ss << ",";
    ss << this->right->to_newick_string();
    ss << ")";
    return ss.str();
}

std::string UnrootedNode::to_newick_string() {
    std::stringstream ss;
    ss << "(";
    ss << this->elem1->to_newick_string();
    ss << ",";
    ss << this->elem2->to_newick_string();
    ss << ",";
    ss << this->elem3->to_newick_string();
    ss << ")";
    return ss.str();
}

std::string AllBinaryCombinationsNode::to_newick_string() {
    assert(this->leaves.size()>0);
    
    std::stringstream ss;
    ss << "{";
    // assert guarantees that label exists
    ss << LeafLabelMapper::get_label_from_leaf_id(this->leaves[0]);
    for(size_t i = 1; i < this->leaves.size(); ++i) {
        ss << "," << LeafLabelMapper::get_label_from_leaf_id(this->leaves[i]);
    }
    ss << "}";
    return ss.str();
}
