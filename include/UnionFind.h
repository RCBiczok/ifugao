/*
 * UnionFind.h
 *
 *  Created on: 11.08.2014
 *      Author: Marcel Radermacher
 *      Changed a bit by Henning Meyerhenke to reflect union by rank and path compression
 *        as taught in "Algorithms 1"
 */

#ifndef UNIONFIND_H_
#define UNIONFIND_H_

#include <vector>


/**
 * @ingroup structures
 * Implements the Union Find data structure to maintain disjoint sets efficiently.
 * Uses path compression and union by rank to achieve running time linear in
 * the number of elements times the inverse Ackermann function.
 */
class UnionFind {
private:
    std::vector<size_t> parent;
	std::vector<unsigned char> rank;    //TODO evtl uint verwenden?
public:
		
	/**
	 * Create a new set representation with not more the \p max_element elements.
	 * Initially every element is in its own set.
	 * @param max_element maximum number of elements 
	 */
    UnionFind(size_t max_element) : parent(max_element), rank(max_element, 0) {
		allToSingletons();
	}

	/**
	 * Assigns every element to a singleton set.
	 * Set id is equal to element id.
	 */

	void allToSingletons();

	/**
	 * Find the representative to element \p u
	 * @param u element
	 * @return representative of set containing \p u
	 */	
    size_t find(size_t u);
	
	/**
	 *  Merge the two sets contain \p u and \p v
	 *  @param u element u
	 *  @param v element v
	 */
    void merge(size_t u, size_t v);

};

#endif