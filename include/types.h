#pragma once

typedef size_t leaf_number;
typedef boost::dynamic_bitset<> leaf_set;

#include "tree.h"       // needs no includes
#include "constraints.h"// needs leaf_set, but includes it itself
#include "leaf_set.h"   // needs constraints

