/*** DO NOT INCLUDE DIRECTLY, INCLUDE types.h INSTEAD ***/
#pragma once


class Node {
public:
    virtual std::string to_newick_string(
            const std::vector<std::string> &leaf_number_to_label);
    virtual bool is_leaf();
};

class Leaf : public Node {
public:
    Leaf(size_t leaf_id) : leaf_id(leaf_id) {}

    size_t leaf_id;
    
    std::string to_newick_string(
            const std::vector<std::string> &leaf_number_to_label);
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

    std::string to_newick_string(
            const std::vector<std::string> &leaf_number_to_label);
    bool is_leaf() {
        return false;
    }
};

typedef InnerNode Tree;

class UnrootedNode : public Node {
public:
    UnrootedNode(std::shared_ptr<Node> elem1, std::shared_ptr<Node> elem2,
                 std::shared_ptr<Node> elem3) :
            elem1(elem1), elem2(elem2), elem3(elem3) {}

    std::shared_ptr<Node> elem1;
    std::shared_ptr<Node> elem2;
    std::shared_ptr<Node> elem3;

    std::string to_newick_string(
            const std::vector<std::string> &leaf_number_to_label);
    bool is_leaf() {
        return false;
    }
};

class AllBinaryCombinationsNode : public Node {
public:
    AllBinaryCombinationsNode(std::vector<size_t> leaves) : leaves(leaves) {
        assert(leaves.size() > 0);
    }

    std::vector<size_t> leaves;
    
    std::string to_newick_string(
            const std::vector<std::string> &leaf_number_to_label);
    bool is_leaf() {
        return false;
    }
};

typedef std::shared_ptr<Node> NodePtr;
typedef std::shared_ptr<Leaf> LeafPtr;
typedef std::shared_ptr<InnerNode> InnerNodePtr;
typedef std::shared_ptr<UnrootedNode> UnrootedNodePtr;
typedef std::shared_ptr<AllBinaryCombinationsNode> AllBinaryCombinationsNodePtr;
