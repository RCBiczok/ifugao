#pragma once

#include<assert.h>          // for assert
#include <cstddef>
typedef std::size_t size_t; // for size_t
#include <memory>           // for std::shared_ptr
#include <vector>           // for std::vector

#include "tree.h"       // needs no includes
#include "constraints.h"// needs leaf_set, but includes it itself
#include "leaf_set.h"   // needs constraints

