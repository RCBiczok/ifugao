#include "tree.h"

#include <assert.h>
#include <sstream>

#include "leaf_label_mapper.h"

std::vector<constraint> Node::extract_constraints() const {
    std::vector<constraint> constraints;
    
    this->get_constraints(constraints);
    
    return constraints;
}

std::string Leaf::to_newick_string() const {
    return LeafLabelMapper::get_label_from_leaf_id(this->leaf_id);
}

std::tuple<size_t, size_t> Leaf::get_constraints(std::vector<constraint> &constraints) const {
    // may not be called
    assert(false);
    return std::make_tuple(0, 0);
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
        right_most_leaf = std::get<1>(right_tuple);
        
        // constraint for right inner edge
        constraints.emplace_back(left_most_leaf, left_most_leaf,
                std::get<0>(right_tuple), std::get<1>(right_tuple));
    } else if(right_leaf) {
        // left edge is an inner edge
        auto left_tuple = this->left->get_constraints(constraints);
        left_most_leaf = std::get<0>(left_tuple);
        right_most_leaf = this->right->get_leaf();
        
        // constraint for left inner edge
        constraints.emplace_back(std::get<0>(left_tuple),
                std::get<1>(left_tuple), right_most_leaf, right_most_leaf);
    } else {
        // left and right edge are inner edges
        auto left_tuple = this->left->get_constraints(constraints);
        auto right_tuple = this->right->get_constraints(constraints);
        left_most_leaf = std::get<0>(left_tuple);
        right_most_leaf = std::get<1>(right_tuple);
        // constraint for left inner edge
        constraints.emplace_back(std::get<0>(left_tuple),
                std::get<1>(left_tuple), right_most_leaf, right_most_leaf);
        // constraint for right inner edge
        constraints.emplace_back(left_most_leaf, left_most_leaf,
                std::get<0>(right_tuple), std::get<1>(right_tuple));
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

std::tuple<size_t, size_t> UnrootedNode::get_constraints(std::vector<constraint> &constraints) const {
    // may not be called
    assert(false);
    return std::make_tuple(0, 0);
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

std::tuple<size_t, size_t> AllBinaryCombinationsNode::get_constraints(
        std::vector<constraint> &constraints) const {
    // may not be called
    assert(false);
    return std::make_tuple(0, 0);
}
