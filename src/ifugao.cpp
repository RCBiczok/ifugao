#include "ifugao.h"

size_t list_trees(const std::vector<constraint> &constraints,
                  const size_t &root_id,
                  LeafSetPtr &leaf_set,
                  FILE *file) {
    assert(leaf_set->contains(root_id));
    // remove root, so it is no longer in the tree
    leaf_set->remove_leaf(root_id);
    
    auto all_trees = find_all_trees(std::move(leaf_set), constraints, true, root_id);

    if (file != nullptr) { // nullptr means just counting
        for (NodePtr t : all_trees) {
            fprintf(file, "%s\n", t->to_newick_string().c_str());
        }
    }
    return all_trees.size();
}

std::vector<NodePtr> find_all_trees(
        const LeafSetPtr leaf_set,
        const std::vector<constraint> &constraints,
        const bool unrooted /* = false*/,
        const size_t root_id /* = 0*/) {
    std::vector<NodePtr> result;
    
    size_t size = leaf_set->size();
    assert(size > 0);
    
    if(size==1) {
        auto leaves = leaf_set->get_all_leaves();
        NodePtr leaf = std::make_shared<Leaf>(leaves[0]);
        result.push_back(leaf);
    } else if(size==2) {
        auto leaves = leaf_set->get_all_leaves();
        NodePtr leaf1 = std::make_shared<Leaf>(leaves[0]);
        NodePtr leaf2 = std::make_shared<Leaf>(leaves[1]);
        NodePtr node = std::make_shared<InnerNode>(leaf1, leaf2);
        result.push_back(node);
    } else if (constraints.empty()) {
        // no more constraints, create a Node for all possible combinations
        auto leaves = leaf_set->get_all_leaves();
        NodePtr tree = std::make_shared<AllBinaryCombinationsNode>(leaves);
        result.push_back(tree);
    } else {
        auto partitions = leaf_set->apply_constraints(constraints);
        size_t limit = number_partition_tuples(partitions);

        for (size_t i = 1; i <= limit; i++) {
            LeafSetPtr part_left;
            LeafSetPtr part_right;
            /*TODO compression would go here and probably look sth like:
            if(!leaf_set->isDenseEnough()) {
                std::tuple new_leaf_set = leaf_set->getCompressedSet();
                compressed=true;
            }
            // normal steps
            
            if(compressed) {
                //uncompress
            }
            
            Also a vector containing all leaves is probably helpful, so we don't 
            need to search for them in the trees but simply have a vector to
            iterate over. 
            */
            std::tie(part_left, part_right) =
                    leaf_set->get_nth_partition_tuple(partitions,i);
            
            auto constraints_left = part_left->filter_constraints(constraints);
            auto constraints_right = part_right->filter_constraints(constraints);
            
            auto subtrees_left = find_all_trees(std::move(part_left), constraints_left);
            auto subtrees_right = find_all_trees(std::move(part_right), constraints_right);
            
            std::vector<NodePtr> trees;
            if(unrooted) {
                trees = merge_unrooted_subtrees(subtrees_left, subtrees_right,
                                                root_id);
            } else {
                trees = merge_subtrees(subtrees_left, subtrees_right);
            }
            
            result.insert(result.end(), trees.begin(), trees.end());
        }
    }
    
    return result;
}

std::vector<NodePtr> merge_subtrees(
        const std::vector<NodePtr> &left,
        const std::vector<NodePtr> &right) {
    std::vector<NodePtr> merged_trees;
    merged_trees.reserve(left.size()*right.size());
    
    for (const NodePtr &l : left) {
        for (const NodePtr &r : right) {
            std::shared_ptr<Node> new_tree = std::make_shared<InnerNode>(l, r);
            merged_trees.push_back(new_tree);
        }
    }

    assert(merged_trees.size() == left.size() * right.size());
    return merged_trees;
}

std::vector<NodePtr> merge_unrooted_subtrees(
        const std::vector<NodePtr> &left,
        const std::vector<NodePtr> &right,
        const size_t root_id) {
    std::vector<NodePtr> merged_trees;
    merged_trees.reserve(left.size()*right.size());
    
    std::shared_ptr<Node> root_leaf = std::make_shared<Leaf>(root_id);
    
    for (const NodePtr &l : left) {
        for (const NodePtr &r : right) {
            std::shared_ptr<Node> new_tree = 
                    std::make_shared<UnrootedNode>(l, r, root_leaf);
            merged_trees.push_back(new_tree);   
        }
    }

    assert(merged_trees.size() == left.size() * right.size());
    return merged_trees;
}
