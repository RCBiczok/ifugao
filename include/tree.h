#pragma once

#include <memory>

class Tree {
public:
    Tree(std::shared_ptr<Tree> left, std::shared_ptr<Tree> right,
        std::shared_ptr<Tree> parent, leaf leaf_id) :
            left(left), right(right), parent(parent), leaf_id(leaf_id);

    Tree(std::shared_ptr<Tree> left, std::shared_ptr<Tree> right,
        leaf leaf_id) : Tree(left, right, nullptr, leaf_id);


    Tree(std::shared_ptr<Tree> left, std::shared_ptr<Tree> right,
        std::shared_ptr<Tree> parent) : Tree(left, right, parent, 0);

    Tree(std::shared_ptr<Tree> parent, leaf leaf_id) :
        Tree(nullptr, nullptr, parent, leaf_id);

    Tree(std::shared_ptr<Tree> left, std::shared_ptr<Tree> right) :
        Tree(left, right, nullptr, 0);

    Tree(leaf leaf_id) : Tree(nullptr, nullptr, nullptr, leaf_id);

  
    Tree() : Tree(nullptr, nullptr, nullptr, 0);

    leaf leaf_id;
    std::shared_ptr<Tree> left;
    std::shared_ptr<Tree> right;
    std::shared_ptr<Tree> parent;

    inline bool is_leaf() {
        return (left == nullptr && right == nullptr);
    }

    std::string to_newick_string();
  
};

//TODO replace with get_neighbors
class UnrootedTree {
public:
    UnrootedTree(std::string p_label, std::shared_ptr<Tree> p_elem1,
            std::shared_ptr<Tree> p_elem2, std::shared_ptr<Tree> p_elem3) :
            label(p_label), elem1(p_elem1), elem2(p_elem2), elem3(p_elem3) {
    }
    UnrootedTree(std::shared_ptr<Tree> rooted_tree) {
        if (rooted_tree == nullptr) {
            return;
        }
        //Only one leaf
        if (rooted_tree->is_leaf()) {
            this->elem1 = rooted_tree;
            return;
        }
        //Only binary tree with two leafs
        if (rooted_tree->left->is_leaf() && rooted_tree->right->is_leaf()) {
            this->elem1 = rooted_tree->left;
            this->elem2 = rooted_tree->right;
            return;
        }
        if (rooted_tree->left->is_leaf()) {
            this->elem1 = rooted_tree->left;
            this->elem2 = rooted_tree->right->left;
            this->elem3 = rooted_tree->right->right;
        } else {
            this->elem1 = rooted_tree->left->left;
            this->elem2 = rooted_tree->left->right;
            this->elem3 = rooted_tree->right;
        }
    }

    leaf leaf_id;
    std::shared_ptr<Tree> elem1;
    std::shared_ptr<Tree> elem2;
    std::shared_ptr<Tree> elem3;

    std::string to_newick_string();
};

std::shared_ptr<Tree> deep_copy(std::shared_ptr<Tree> tree);

