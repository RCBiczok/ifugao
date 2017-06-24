#pragma once

#include <memory> // for shared_ptr
#include "types.h"



class Node {
public:
    virtual ~Node() = default;
    virtual std::string to_newick_string(const std::vector<std::string> &leaf_number_to_label);
    virtual bool is_leaf();
};

class Leaf : public Node {
public:
    Leaf(leaf_number leaf_id) : leaf_id(leaf_id) {}

    leaf_number leaf_id;

    bool is_leaf() {
        return true;
    }
};

class InnerNode : public Node {
public:
    InnerNode(std::shared_ptr<Node> left, std::shared_ptr<Node> right) :
            left(left), right(right) { }

    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    bool is_leaf() {
        return false;
    }
};

class UnrootedNode : public Node {
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

typedef InnerNode Tree;

//TODO std::shared_ptr<Tree> deep_copy(std::shared_ptr<Tree> tree);

