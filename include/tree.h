/*** DO NOT INCLUDE DIRECTLY, INCLUDE types.h INSTEAD ***/
#pragma once

#include "types.h"

class Node {
public: 
    virtual std::string to_newick_string() const;
    virtual bool is_leaf() const;
    virtual size_t get_leaf() const;
    
    /**
     * Returns a vector containing all constraints infered from this tree.
     *
     * @return All constraints of this tree.
     */
    std::vector<constraint> extract_constraints() const;
    
    virtual std::tuple<size_t, size_t> get_constraints(
            std::vector<constraint> &constraints) const;
};

class Leaf : public Node {
public:
    Leaf(size_t leaf_id) : leaf_id(leaf_id) {};

    size_t leaf_id;
    
    std::string to_newick_string() const;
    bool is_leaf() const { return true; }
    size_t get_leaf() const { return leaf_id; }
    
    std::tuple<size_t, size_t> get_constraints(std::vector<constraint> &constraints) const;
};

class InnerNode : public Node {
public:
    InnerNode(std::shared_ptr<Node> left, std::shared_ptr<Node> right) :
            left(left), right(right) { };

    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    std::string to_newick_string() const;
    bool is_leaf() const { return false; }
    size_t get_leaf() const {
        // may not be called, use is_leaf() to prevent calling it
        assert(false);
        return 0;
    }
    
    std::tuple<size_t, size_t> get_constraints(std::vector<constraint> &constraints) const;
};

class UnrootedNode : public Node {
public:
    UnrootedNode(std::shared_ptr<Node> elem1, std::shared_ptr<Node> elem2,
                 std::shared_ptr<Node> elem3) :
            elem1(elem1), elem2(elem2), elem3(elem3) {};

    std::shared_ptr<Node> elem1;
    std::shared_ptr<Node> elem2;
    std::shared_ptr<Node> elem3;

    std::string to_newick_string() const;
    bool is_leaf() const { return false; }
    size_t get_leaf() const {
        // may not be called, use is_leaf() to prevent calling it
        assert(false);
        return 0;
    }
    
    std::tuple<size_t, size_t> get_constraints(std::vector<constraint> &constraints) const;
};

class AllBinaryCombinationsNode : public Node {
public:
    AllBinaryCombinationsNode(std::vector<size_t> leaves) : leaves(leaves) {
        assert(leaves.size() > 0);
    };

    std::vector<size_t> leaves;
    
    std::string to_newick_string() const;
    bool is_leaf() const { return false; }
    size_t get_leaf() const {
        // may not be called, use is_leaf() to prevent calling it
        assert(false);
        return 0;
    }
    
    std::tuple<size_t, size_t> get_constraints(std::vector<constraint> &constraints) const;
};

typedef std::shared_ptr<Node> NodePtr;
typedef std::shared_ptr<Leaf> LeafPtr;
typedef std::shared_ptr<InnerNode> InnerNodePtr;
typedef std::shared_ptr<UnrootedNode> UnrootedNodePtr;
typedef std::shared_ptr<AllBinaryCombinationsNode> AllBinaryCombinationsNodePtr;

typedef std::shared_ptr<Node> Tree;
