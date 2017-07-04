#include "leaf_label_mapper.h"

void LeafLabelMapper::init_leaf_label_mapper(
        std::map<const char*, size_t> label_to_leaf_id,
        char **species_names) {
    static bool already_init = false;
    
    if(!already_init) {
        LeafLabelMapper::species_names = species_names;
        LeafLabelMapper::label_to_leaf_id = label_to_leaf_id;
    } else {
        // only init once ever
        assert(false);
    }
}

char* LeafLabelMapper::get_label_from_leaf_id(const size_t leaf) {
    assert(LeafLabelMapper::species_names != nullptr);
    
    return LeafLabelMapper::species_names[leaf];
};

size_t LeafLabelMapper::get_leaf_id_from_label(const char* label) {
    assert(LeafLabelMapper::label_to_leaf_id.count(label) == 1);
    
    return LeafLabelMapper::label_to_leaf_id[label];
};

