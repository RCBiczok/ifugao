#include "util.h"

Tree generate_induced_tree(const Tree node,
                           const missingData *missing_data,
                           size_t partition) {
    // TODO what does this method do actually and how can it achieve it easier
    assert(node != nullptr);
    
    if (node->is_leaf()) {
        // Leaf case
        const size_t leaf_id = const dynamic_cast<LeafPtr>(node).leaf_id;
        const char* label = Node::get_label_for_leaf(leaf_id);
        if (true //TODO why is this here?: species_map.count(label) == 1
            && getDataMatrix(missing_data, leaf_id, partition)) {
            return std::make_shared<Leaf>(leaf_id);
        }
    } else {
        // Inner node case
        //TODO dynamic_cast to either InnerNode or UnrootedNode
        /*auto left = generate_induced_tree(node->children[0], missing_data, partition);
        auto right = generate_induced_tree(node->children[1], missing_data,
                                           partition);
        //Left and right subtrees are included -> common ancestor is included
        if (left != nullptr && right != nullptr) {
            auto inner_node = std::make_shared<Node>(*node);
            left->parent = inner_node;
            right->parent = inner_node;
            inner_node->children[0] = left;
            inner_node->children[1] = right;
            return inner_node;
        } else if (left != nullptr) {
            return left;
        } else if (right != nullptr) {
            return right;
        }*/
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
                get_leaf_by_name(nwk_tree,
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
    
    return root_recursive(leaf->parent, leaf);
}

Tree root_recursive(ntree_t *current_ntree, ntree_t *parent) {
    assert(current_ntree != nullptr);
    assert(parent != nullptr);
    //when the children_count is 3, we are at the pseudoroot of the unrooted
    // binary tree, so there is no parent node
    assert(current_ntree->children_count == 0
           || current_ntree->children_count == 2
           || (current_ntree->children_count == 3
               && current_ntree->parent == nullptr));
               
    if(current_ntree->children_count == 0) {
        size_t leaf_id = Node::get_leaf_from_label(current_ntree->label);
        return std::make_shared<Leaf>(leaf_id);
    } else if(current_ntree->children_count == 2) {
        assert(current_ntree->children[0] != nullptr);
        assert(current_ntree->children[1] != nullptr);
        if(current_ntree->parent == nullptr) {
            // root of a rooted tree
            if(parent == current_ntree->children[0]) {
                return root_recursive (current_ntree->children[1], parent);
            } else if(parent == current_ntree->children[1]) {
                return root_recursive (current_ntree->children[0], parent);
            } else {
                assert(false);
            }
        } else {
            ntree_t *left; 
            ntree_t *right; 
            if(parent == current_ntree->parent) {
                left = current_ntree->children[0];
                right = current_ntree->children[1];
            } else if(parent == current_ntree->children[0]) {
                left = current_ntree->children[1];
                right = current_ntree->parent;
            } else if(parent == current_ntree->children[1]) {
                left = current_ntree->parent;
                right = current_ntree->children[0];
            } else {
                assert(false);
            }
            auto left_tree = root_recursive(left, current_ntree);
            auto right_tree = root_recursive(right, current_ntree);
            
            return std::make_shared<InnerNode>(left_tree, right_tree);
        }
        
    } else if(current_ntree->children_count == 3) {
        assert(current_ntree->parent == nullptr); // root may not have parent
        
        auto tree1 = root_recursive(current_ntree->children[0], current_ntree);
        auto tree2 = root_recursive(current_ntree->children[1], current_ntree);
        auto tree3 = root_recursive(current_ntree->children[2], current_ntree);
        
        return std::make_shared<UnrootedNode>(tree1, tree2, tree3);
    }
    
    assert(false);
    return nullptr;
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

