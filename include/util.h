#ifndef UTIL_H
#define UTIL_H

//this file contains useful function that don't belong anywhere else

#include "newick-tools/newick-tools.h"
#include "input_parser.h"
#include "terraces.h"

#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <iostream>
#include <vector>

/**
 * This funktion generates the trees T|G_i, which is exactly the subtree of T induced by partition G_i
 * @param node the rooted binary supertree
 * @param missing_data the data matrix representing the missing partition data.
 * @param partition the index i used to extract the subtree T|G_i. In other words, it is the i-th
 * column of the missing data matrix
 * @return the subtree T|G_i.
 */
Tree generate_induced_tree(const NodePtr node,
                           const missingData *missing_data,
                           const std::map<std::string, leaf_number> &first,
                           const label_mapper &id_to_label,
                           const size_t partition);

/**
 * @param missing_data the missing data matrix
 * @return a list of all species that have data for all partitions on the given missing data matrix
 */
std::vector<std::string> get_root_species(const missingData *missing_data);

/**
 * This function roots the tree at an appropriate position according to the missing data array
 *
 * @param tree the tree to be rooted
 * @param missing_data the data for the missing sequences on each partition
 * @return the new root of the tree, or NULL if the tree cannot be rooted
 * (e.g. if there is no species that has data for every partition)
 */
Tree root_tree(ntree_t *tree,
               const missingData *missing_data,
               label_mapper &id_to_label);

/**
 * Returns a pointer to the leaf that has the label <label>
 *
 * @param tree a pointer to the root of the tree
 * @return a pointer to the leaf
 */
ntree_t *get_leaf_by_name(ntree_t *tree, const char *label);

/**
 * private function that roots the tree at a given leaf-edge
 * @param root the leaf-edge
 * @return pointer to the new root
 */
Tree root_at(ntree_t *root, label_mapper &id_to_label);

/**
 * private function
 * @param current_ntree The current ntree_t node. From this we get the children.
 * @param previous The ntree_t represention of the previous node. This is needed
 *                 to avoid calling the recursion in the wrong direction.
 * @param id_to_label The label mapper.
 * @return A rooted tree.
 */
Tree root_recursive(ntree_t *current_ntree,
                    ntree_t *previous,
                    label_mapper &id_to_label);

/**
 * @brief check_tree do a dfs on the tree and check, if the childs parent is the current node.
 * @param tree the tree to check
 * @return true if the tree is correct
 */
bool check_tree(ntree_t *tree);

/**
 * @brief fix_tree makes sure, that tree->child[i]->parent == tree
 * @param tree the tree to fix
 */
void fix_tree(ntree_t *tree);

/**
 * @brief get_neighbours returns the neighbours of a given node in the tree.
 * This can be used for easier usage of rooted trees as unrooted trees
 * @param node the node of which we want to get the adjacent nodes
 * @return a vector containing the adjacent nodes.
 */
std::vector<Tree> get_neighbours(Tree node);

/**
 * @brief get_leafs return a list the list of all leafs of the tree
 * @param leaf_list the list where the leafs will be inserted
 * @param current_node the node, where the dfs is at the moment
 * @param parent the node where we come from. to avoid endles recursion
 */
/*
void get_leafs(std::vector<Tree> &leaf_list, Tree current_node,
               Tree parent);
*/

/**
 * Checks whether a given tree is a binary tree and whether the labels at 
 * the leafs are consistent with the missing_data structure.
 *
 * @param tree The tree to check.
 * @param missing_data The missing data to check consistency against.
 * @return 0 if the tree is binary and consistent with the missing_data
 *         -1 if the tree is not binary
 *         -2 if the tree is binary and inconsistent with the missing_data
 */
short isBinaryAndConsistent(ntree_t *tree, missingData* missing_data);

/**
 * Converts a given newick tree to string
 * @param strm the output stream
 * @param tree the tree whose string representation is requested
 * @return a textual representation of the given tree
 */
std::ostream &operator<<(std::ostream &strm, const ntree_t *tree);

#endif //include guard
