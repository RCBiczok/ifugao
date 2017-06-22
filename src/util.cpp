#include "util.h"

Tree generate_induced_tree(const Tree tree,
                           const missingData *missing_data,
                           std::map<std::string, unsigned char> &species_map,
                           size_t partition) {
    if (tree == nullptr) {
        return nullptr;
    }

    if (tree->is_leaf()) {
        // Leave case
        if (species_map.count(tree->label) == 1
            && getDataMatrix(missing_data, species_map[tree->label],
                             partition)) {
            Tree leave = std::make_shared<Node>(*tree);
            leave->left = nullptr;
            leave->right = nullptr;
            leave->parent = nullptr;
            return tree;
        }
    } else {
        // Inner node case
        assert(tree->left != nullptr);
        auto left = generate_induced_tree(tree->left, missing_data, species_map,
                                          partition);
        assert(tree->right != nullptr);
        auto right = generate_induced_tree(tree->right, missing_data,
                                           species_map, partition);
        //Left and right subtrees are included -> common ancestor is included
        if (left != nullptr && right != nullptr) {
            auto inner_node = std::make_shared<Node>(*tree);
            left->parent = inner_node;
            right->parent = inner_node;
            inner_node->left = left;
            inner_node->right = right;
            return inner_node;
        } else if (left != nullptr) {
            return left;
        } else if (right != nullptr) {
            return right;
        }
    }
    return nullptr;
}

Tree root_tree(ntree_t *nwk_tree, const missingData *missing_data,
                  size_t &root_species_id) {
    assert(nwk_tree != nullptr);
    
    bool root_species_found = false;
    for (size_t i = 0; i < missing_data->numberOfSpecies; i++) {
        bool contains_all_data = true;
        for (size_t k = 0; k < missing_data->numberOfPartitions; k++) {
            if (getDataMatrix(missing_data, i, k) == 0) {
                contains_all_data = false;
                break;
            }
        }
        if (contains_all_data) {
            root_species_found = true;
            root_species_id = i;
            break;
        }
    }
    if (root_species_found) {
        ntree_t *future_root = 
                get_leaf_by_id(nwk_tree,
                               missing_data->speciesNames[root_species_id]);
        if (future_root == nullptr) {
            std::cout << "In root_tree(...): label "
                      << missing_data->speciesNames[root_species_id]
                      << " not found in tree" << std::endl;
            return nullptr;
        } else {
            return root_at(future_root);
        }
    } else {
        //tree cannot be rooted consistently
        return nullptr;
    }
}

ntree_t *get_leaf_by_name(ntree_t *tree, const char *label) {
    assert(tree != nullptr);
    assert(label != nullptr);
    if (tree->label != nullptr && strcmp(tree->label, label) == 0) {
        return tree;
    } else {
        for (int i = 0; i < tree->children_count; i++) {
            assert(tree->children[i]->parent != nullptr);
            if (tree != tree->children[i]->parent) {
                if (tree->children[i]->label != nullptr) {
                    std::cout << "parent von " << tree->children[i]->label
                              << " ist falsch\n";
                } else {
                    std::cout << "parent von " << tree->children[i]
                              << "ist falsch\n";
                }

            }
            //assert(tree == tree->children[i]->parent);
            ntree_t *temp = get_leaf_by_name(tree->children[i], label);
            if (temp != nullptr) {
                return temp;
            }
        }
    }
    return nullptr; //label not found
}

Tree root_at(ntree_t *leaf) {
    //if leaf->parent is null, leaf is the root => the tree is not binary, or the node is no leaf
    assert(leaf != nullptr);
    assert(leaf->parent != nullptr);
    assert(leaf->children_count == 0);  //should be a leaf
    ntree_t *neighbour = leaf->parent;
    NodePtr root = std::make_shared<Node>();
    NodePtr new_leaf = std::make_shared<Node>();
    NodePtr new_neighbour = std::make_shared<Node>();

    //initialize root
    root->label = "";
    root->left = new_leaf;
    root->right = new_neighbour;
    root->label = "";

    //initialize new_leaf and new_neighbour
    new_leaf->parent = root;
    new_neighbour->parent = root;

    recursive_root(new_leaf, leaf, neighbour);
    recursive_root(new_neighbour, neighbour, leaf);
    return root;
}

void recursive_root(Tree current, ntree_t *current_ntree,
                    ntree_t *parent) {

    assert(current != nullptr);
    assert(current_ntree != nullptr);

    //when the children_count is 3, we are at the pseudoroot of the unrooted binary tree
    assert(current_ntree->children_count == 2
           || current_ntree->children_count == 0
           || (current_ntree->children_count == 3
               && current_ntree->parent == nullptr));

    if (current_ntree->label == nullptr) {
        current->label = "";
    } else {
        current->label = current_ntree->label;
    }

    if (current_ntree->children_count == 0) {   //is leaf
        return;
    }

    current->left = std::make_shared<Node>();
    current->left->parent = current;
    current->right = std::make_shared<Node>();
    current->right->parent = current;

    ntree_t *left_ntree;
    ntree_t *right_ntree;

    if (current_ntree->children_count == 3) {   //we are at the pseudoroot
        //set the children, leave out the parent
        if (current_ntree->children[0] == parent) {
            left_ntree = current_ntree->children[1];
            right_ntree = current_ntree->children[2];
        } else if (current_ntree->children[1] == parent) {
            left_ntree = current_ntree->children[0];
            right_ntree = current_ntree->children[2];
        } else if (current_ntree->children[2] == parent) {
            left_ntree = current_ntree->children[0];
            right_ntree = current_ntree->children[1];
        } else {
            assert(0);
        }
    } else if(current_ntree->parent == nullptr) { //we are at the regular root
        if(current_ntree->children[0]->label != nullptr) {
            current->label = current_ntree->children[0]->label;
        }
        if(current_ntree->children[1]->label != nullptr) {
            current->label = current_ntree->children[1]->label;
        }
        current->left = nullptr;
        current->right = nullptr;
        return;
    } else {
        assert(current_ntree->children_count == 2);
        //set the children, leave out the parent
        if (current_ntree->children[0] == parent) {
            left_ntree = current_ntree->children[1];
            right_ntree = current_ntree->parent;
        } else if (current_ntree->children[1] == parent) {
            left_ntree = current_ntree->children[0];
            right_ntree = current_ntree->parent;
        } else if (current_ntree->parent == parent) {
            left_ntree = current_ntree->children[0];
            right_ntree = current_ntree->children[1];
        } else {
            assert(0);
        }
    }

    recursive_root(current->left, left_ntree, current_ntree);
    recursive_root(current->right, right_ntree, current_ntree);
}

bool check_tree(ntree_t *tree) {
    for (int i = 0; i < tree->children_count; i++) {
        if (tree != tree->children[i]->parent) {
            if (tree->children[i]->label != nullptr) {
                std::cout << tree << ": parent of " << tree->children[i]->label
                          << " is wrong: " << tree->children[i]->parent
                          << std::endl;
            } else {
                std::cout << tree << ": parent of " << tree->children[i]
                          << " is wrong: " << tree->children[i]->parent
                          << std::endl;
            }
        }
        check_tree(tree->children[i]);
    }
    return true;
}

void fix_tree(ntree_t *tree) {
    assert(tree != nullptr);
    int x = tree->children_count;
    for (int i = 0; i < x; i++) {
        tree->children[i]->parent = tree;
        fix_tree(tree->children[i]);
    }
}

