#include "tree.h"

#include <sstream>

char* get_label_for_leaf(const size_t leaf) {
    assert(Node::speciesNames != nullptr);
    
    return Node::speciesNames[leaf];
}

size_t get_leaf_from_label(const char* label) {
    assert(Node::label_to_id.count(label) == 1);
    
    return label_to_id[label];
}


std::string Leaf::to_newick_string() {
    return Node::get_label_for_leaf(this->leaf_id);
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
    ss << Node::get_label_for_leaf(this->leaves[0]); // assert guarantees existance
    for(size_t i = 1; i < this->leaves.size(); ++i) {
        ss << "," << Node::get_label_for_leaf(this->leaves[i]);
    }
    ss << "}";
    return ss.str();
}
