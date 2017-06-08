
std::vector<constraint> extract_constraints_from_tree(
        const std::shared_ptr<Tree> supertree) {
    std::vector<constraint> constraints;

    extract_constraints_from_tree_rec(supertree, constraints);

    return constraints;
}

std::vector<constraint> find_constraints(const leaf_set &leaves,
                                         const std::vector<constraint> &constraints) {
    std::vector<constraint> valid_constraints;

    for (constraint cons : constraints) {
        if (cons.smaller_left == cons.bigger_left) {
            if (leaves.find(cons.smaller_left) != leaves.end()
                && leaves.find(cons.smaller_right) != leaves.end()
                && leaves.find(cons.bigger_right) != leaves.end()) {
                // constraint is valid on leaf set
                valid_constraints.push_back(cons);
            }
        } else {
            // smaller_right == bigger_right
            if (leaves.find(cons.smaller_left) != leaves.end()
                && leaves.find(cons.smaller_right) != leaves.end()
                && leaves.find(cons.bigger_left) != leaves.end()) {
                // constraint is valid on leaf set
                valid_constraints.push_back(cons);
            }
        }
    }
    return valid_constraints;
}


static std::tuple<leaf, leaf> extract_constraints_from_tree_rec(
        const std::shared_ptr<Tree> node,
        std::vector<constraint> &constraints) {

    assert(node != nullptr);

    if (node->is_leaf()) {
        auto this_leaf = node->label;
        return std::make_tuple(this_leaf, this_leaf);
    }

    // (l,r) of the left child node
    leaf l_left_most;
    leaf l_right_most;
    std::tie(l_left_most, l_right_most) = extract_constraints_from_tree_rec(
            node->left, constraints);

    // (l,r) of the right child node
    leaf r_left_most;
    leaf r_right_most;
    std::tie(r_left_most, r_right_most) = extract_constraints_from_tree_rec(
            node->right, constraints);

    if (l_left_most != l_right_most) {
        constraint c;
        c.smaller_left = l_left_most;
        c.smaller_right = l_right_most;
        c.bigger_left = l_left_most;
        c.bigger_right = r_right_most;
        constraints.push_back(c);
    }

    if (r_left_most != r_right_most) {
        constraint c;
        c.smaller_left = r_left_most;
        c.smaller_right = r_right_most;
        c.bigger_left = l_left_most;
        c.bigger_right = r_right_most;
        constraints.push_back(c);
    }

    return std::make_tuple(l_left_most, r_right_most);
}
