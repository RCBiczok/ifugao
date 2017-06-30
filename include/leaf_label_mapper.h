#pragma once

#include "types.h"

/**
 * Static variables and methods to map leaf_id <->label
 */
class LeafLabelMapper {
public:
    static std::map<const char*, size_t> label_to_leaf_id;
    static char **species_names;

    char* get_label_from_leaf_id(const size_t leaf) {
        assert(species_names != nullptr);
        
        return species_names[leaf];
    };

    size_t get_leaf_id_from_label(const char* label) {
        assert(label_to_leaf_id.count(label) == 1);
        
        return label_to_leaf_id[label];
    };
};
