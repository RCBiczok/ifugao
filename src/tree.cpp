#include "tree.h"

#include <assert.h>
#include <sstream>

#include "leaf_label_mapper.h"

std::string Leaf::to_newick_string() const {
    return LeafLabelMapper::get_label_from_leaf_id(this->leaf_id);
}

constraint Leaf::get_constraints(std::vector<constraint> &constraints) const {
    // may not be called
    assert(false);
    constraint c;
    return c;
}

std::string InnerNode::to_newick_string() const {
    std::stringstream ss;
    ss << "(";
    ss << this->left->to_newick_string();
    ss << ",";
    ss << this->right->to_newick_string();
    ss << ")";
    return ss.str();
}

std::tuple<size_t, size_t> InnerNode::get_constraints(
        std::vector<constraint> &constraints) const {
    size_t left_most_leaf;
    size_t right_most_leaf;
    
    bool left_leaf = this->left->is_leaf();
    bool right_leaf = this->right->is_leaf(); 
    
    if (left_leaf && right_leaf) {
        // no inner edge, just return values
        left_most_leaf = this->left->get_leaf();
        right_most_leaf = this->right->get_leaf();
    } else if(left_leaf) {
        // right edge is an inner edge
        left_most_leaf = this->left->get_leaf();
        auto right_tuple = this->right->get_constraints(constraints);
        right_most_leaf = right_tuple.second;
        
        constraint c;
        c.smaller_left = left_most_leaf;
        c.bigger_left = left_most_leaf;
        c.smaller_right = right_tuple.first;
        c.bigger_right = right_tuple.second;        
        constraints.push_back(c);
    } else if(right_leaf) {
        // left edge is an inner edge
        auto left_tuple = this->left->get_constraints(constraints);
        left_most_leaf = left_tuple.first;
        right_most_leaf = this->right->get_leaf();
        
        constaint c;
        c.smaller_left = left_tuple.first;
        c.bigger_left = left_tuple.sectond;
        c.smaller_right = right_most_leaf;
        c.bigger_right = right_most_leaf;
        constraints.push_back(c);
    } else {
        // left and right edge are inner edges
        constraint c_left = this->left->get_constraints(constraints);
        constraint c_right = this->right->get_constraints(constraints);
        left_most_leaf = c_left.first;
        right_most_leaf = c_right.second;
        
        constraint left_c;
        left_c.smaller_left = left_most_leaf;
        left_c.bigger_left = left_most_leaf;
        left_c.smaller_right = right_tuple.first;
        left_c.bigger_right = right_tuple.second;
        constraints.push_back(left_c);
                
        constraint right_c;
        right_c.smaller_left = left_most_leaf;
        right_c.bigger_left = left_most_leaf;
        right_c.smaller_right = right_tuple.first;
        right_c.bigger_right = right_tuple.second;
        constraints.push_back(right_c);
    }
    
    return std::make_tuple(left_most_leaf, right_most_leaf);
}

std::string UnrootedNode::to_newick_string() const {
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

constraint UnrootedNode::get_constraints(std::vector<constraint> &constraints) const {
    // may not be called
    assert(false);
    constraint c;
    return c;
}

std::string AllBinaryCombinationsNode::to_newick_string() const {
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

constraint AllBinaryCombinationsNode::get_constraints(
        std::vector<constraint> &constraints) const {
    // may not be called
    assert(false);
    constraint c;
    return c;
}
