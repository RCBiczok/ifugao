#pragma once

#include <cstddef>
typedef std::size_t size_t;     // for size_t
#include <assert.h>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

/**
 * A simple constraint structure. It is meant to be used as:
 * lca(smaller_left, smaller_right) < lca(bigger_left, bigger_right)
 */
struct constraint {
    size_t smaller_left;
    size_t bigger_left;
    size_t smaller_right;
    size_t bigger_right;
    
    constraint();
    
    constraint(size_t smaller_left,
               size_t bigger_left,
               size_t smaller_right,
               size_t bigger_right) : smaller_left(smaller_left),
                                      bigger_left(bigger_left),
                                      smaller_right(smaller_right),
                                      bigger_right(bigger_right) {}
};

#include "missing_data.h"       // needs no other includes
#include "leaf_label_mapper.h"  // needs no other includes
#include "tree.h"               // needs defined constraint struct
#include "leaf_set.h"           // needs defined constraint struct

