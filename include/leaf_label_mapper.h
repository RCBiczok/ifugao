#pragma once

#include "terraces.h" // for missingData

/**
 * Struct to compare strings (used for label comparison)
 */
struct label_compare {
   bool operator()(char const *label1, char const *label2) const {
      return std::strcmp(label1, label2) < 0;
   }
};

/**
 * Static variables and methods to map leaf_id <->label
 */
class LeafLabelMapper {
public:
    /**
     * Initialize the LeafLabelMapper.
     * @param missingData A pointer to a missingData struct, which will be used
     *                    for initialization.
     */
    static void init_leaf_label_mapper(const missingData *m);
    /**
     * Deinitialize the LeafLabelMapper.
     */
    static void deinit_leaf_label_mapper();
    
    /**
     * Returns the label of the leaf with the given ID.
     * @param leaf Leaf ID for which to return the label.
     * @return The label of the leaf with the given ID.
     */
    static char* get_label_from_leaf_id(const size_t leaf);
    /**
     * Returns the leaf ID of the leaf with the given label.
     * @param label Leaf label for which to return the leaf ID.
     * @return The leaf IDof the leaf with the given label.
     */
    static size_t get_leaf_id_from_label(const char* label);
    
    /**
     * Returns the amount of known leaves with labels.
     * @return The amount of known leaves with labels.
     */
    static size_t size();
private:
    /**
     * Mapping of leaf labels to their IDs
     */
    static std::map<const char*, size_t, label_compare> label_to_leaf_id;
    
    /**
     * Mapping of leaf IDs to their labels
     */
    static char **species_names;
    
    /**
     * Whether LeafLabelMapper currently is initialized. Used only internal.
     */
    static bool already_init;
};

