#pragma once

#include <memory> // for shared_ptr

typedef InnerNode Tree;

class Node {
public:
    virtual std::string to_newick_string(std::vector<std::string> &leaf_number_to_label);
    virtual bool is_leaf();
};

class Leaf : Node {
public:
    Leaf(leaf leaf_id) : leaf_id(leaf_id) {}

    leaf leaf_id;

    bool is_leaf() {
        return true;
    }
};

class InnerNode : Node {
public:
    InnerNode(std::shared_ptr<Node> left, std::shared_ptr<Node> right) :
            left(left), right(right) { }

    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    bool is_leaf() {
        return false;
    }
};

class UnrootedNode : Node {
public:
    UnrootedNode(std::shared_ptr<Node> elem1, std::shared_ptr<Node> elem2,
                 std::shared_ptr<Node> elem3) :
            elem1(elem1), elem2(elem2), elem3(elem3) {}

    std::shared_ptr<Node> elem1;
    std::shared_ptr<Node> elem2;
    std::shared_ptr<Node> elem3;


    bool is_leaf() {
        return false;
    }
};

//TODO std::shared_ptr<Tree> deep_copy(std::shared_ptr<Tree> tree);

