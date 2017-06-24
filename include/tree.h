/*** DO NOT INCLUDE DIRECTLY, INCLUDE types.h INSTEAD ***/
#pragma once

#include <memory> // for shared_ptr
#include "types.h"



class Node {
public:
    virtual ~Node() = default;
    virtual std::string to_newick_string(const std::vector<std::string> &leaf_number_to_label);
    virtual bool is_leaf();
    
// TODO these could be moved , but for now I'll just leave it here
    static char* get_label_for_leaf(const size_t leaf);
    static size_t get_leaf_from_label(const char* label);

    static std::map<const char*, size_t> label_to_id;
    static char **speciesNames;
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
            left(left), right(right) { };

    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    std::string to_newick_string();
    bool is_leaf() {
        return false;
    }
};

class UnrootedNode : public Node {
public:
    UnrootedNode(std::shared_ptr<Node> elem1, std::shared_ptr<Node> elem2,
                 std::shared_ptr<Node> elem3) :
            elem1(elem1), elem2(elem2), elem3(elem3) {};

    std::shared_ptr<Node> elem1;
    std::shared_ptr<Node> elem2;
    std::shared_ptr<Node> elem3;

    std::string to_newick_string();
    bool is_leaf() {
        return false;
    }
};

class AllBinaryCombinationsNode : public Node {
public:
    AllBinaryCombinationsNode(std::vector<size_t> leaves) : leaves(leaves) {
        assert(leaves.size() > 0);
    };

    std::vector<size_t> leaves;
    
    std::string to_newick_string();
    bool is_leaf() {
        return false;
    }
};

typedef InnerNode Tree;

//TODO std::shared_ptr<Tree> deep_copy(std::shared_ptr<Tree> tree);

typedef std::shared_ptr<Node> Tree;
