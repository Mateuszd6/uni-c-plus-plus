#include <iostream>
#include <set>
#include <string>
#include <unordered_map>

#include "strset.h"
#include "strsetconst.h"

#ifdef NDEBUG
const bool debug{false};
#else
const bool debug{true};
#endif

namespace {

using Strset = std::set<std::string>;
using Strsets_map = std::unordered_map<unsigned long, Strset>;

// Returns the next free id and increments the coutner. Every time this
// function is called the different value is returned (untill the integer
// value overlaps, at least). This function correctly staticly initializes
// the next free id.
size_t get_and_increment_next_free_id() {
    static size_t next_free_id{0};
    return next_free_id++;
}

// Correctly staticly initializes the strsets map.
Strsets_map& get_strsets_map() {
    static Strsets_map strsets_map{};
    return strsets_map;
}
}  // namespace

#ifdef __cplusplus
namespace jnp1 {
extern "C" {
#endif

unsigned long strset_new() {
    int retval = get_and_increment_next_free_id();
    get_strsets_map().insert({retval, Strset()});

    if (debug) {
        std::cerr << "strset_new()\n"
                  << "strset_new: set " << retval << " created\n";
    }

    return retval;
}

void strset_delete(unsigned long id) {
    if (debug)
        std::cerr << "strset_delete(" << id << ")\n";

    if (id == strset42()) {
        if (debug)
            std::cerr << "strset_delete: attempt to remove the 42 Set\n";

        return;
    }

    auto elements_erased = get_strsets_map().erase(id);
    if (debug)
        std::cerr << "strset_delete: set " << id
                  << (elements_erased ? " deleted\n" : " does not exist\n");

}

size_t strset_size(unsigned long id) {
    if (debug)
        std::cerr << "strset_size(" << id << ")\n";

    auto find = get_strsets_map().find(id);
    if (find != get_strsets_map().end()) {
        auto retval = find->second.size();

        if (debug) {
            std::cerr << "strset_size: ";
            if (id == strset42())
                std::cerr << "the 42 Set";
            else
                std::cerr << "set " << id;
            std::cerr << " contains " << retval << " element(s)\n";
        }

        return retval;
    }
    else {
        if (debug)
            std::cerr << "strset_size: set " << id << " does not exist\n";

        return 0;
    }
}

void strset_insert(unsigned long id, const char* value) {
    if (debug) {
        std::cerr << "strset_insert(" << id << ", ";
        if (value != nullptr)
            std::cerr << "\"" << value << "\"";
        else
            std::cerr << "NULL";
        std::cerr << ")\n";
    }

    if (id == strset42()) {
        if (debug)
            std::cerr << "strset_insert: attempt to insert into the 42 Set\n";

        return;
    }

    if (value == nullptr) {
        if (debug)
            std::cerr << "strset_insert: invalid value (NULL)\n";

        return;
    }

    auto find = get_strsets_map().find(id);
    if (find != get_strsets_map().end()) {
        auto insert_suceeded = find->second.emplace(value).second;
        if (debug)
            std::cerr << "strset_insert: set " << id << ", element \"" << value
                       << (insert_suceeded ? "\" inserted\n"
                                           : "\" was already present\n");

    }
    else if (debug) {
        std::cerr << "strset_insert: set " << id << " does not exist\n";
    }
}

void strset_remove(unsigned long id, const char* value) {
    if (debug) {
        std::cerr << "strset_remove(" << id << ", \""
                   << (value == nullptr ? "NULL" : value) << "\")\n";
    }

    if (id == strset42()) {
        if (debug)
            std::cerr << "strset_remove: attempt to remove from the 42 Set\n";
        return;
    }

    if (value == nullptr) {
        if (debug)
            std::cerr << "strset_remove: invalid value (NULL)\n";
        return;
    }

    auto find = get_strsets_map().find(id);
    if (find != get_strsets_map().end()) {
        auto erase_str = std::string(value);
        auto elements_removed = find->second.erase(erase_str);

        if (debug) {
            if (elements_removed) {
                std::cerr << "strset_remove: set " << id << ", element \""
                           << value << "\" removed\n";
            }
            else {
                std::cerr << "strset_remove: set " << id
                           << " does not contain the element \"" << value
                           << "\"\n";
            }
        }
    }
}

int strset_test(unsigned long id, const char* value) {
    if (debug)
        std::cerr << "strset_test(" << id << ", \""
                   << (value == nullptr ? "NULL" : value) << "\")\n";

    if (value == nullptr) {
        if (debug)
            std::cerr << "strset_test: invalid value (NULL)\n";
        return 0;
    }

    auto test_str = std::string(value);
    auto find = get_strsets_map().find(id);

    // If the set was found, we search for the value.
    if (find != get_strsets_map().end()) {
        auto set_ref = find->second;
        auto set_find = set_ref.find(test_str);

        auto retval = set_find == set_ref.end() ? 0 : 1;

        if (debug) {
            std::cerr << "strset_test: ";
            if (id == strset42())
                std::cerr << "the 42 Set";
            else
                std::cerr << "set " << id;
            std::cerr << (retval == 0 ? " does not contain " : " contains ")
                       << "the element \"" << value << "\"\n";
        }

        return retval;
    }

    if (debug)
        std::cerr << "strset_test: set " << id << " does not exist\n";

    return 0;
}

void strset_clear(unsigned long id) {
    if (debug)
        std::cerr << "strset_clear(" << id << ")\n";

    if (id == strset42()) {
        if (debug)
            std::cerr << "strset_clear: attempt to clear the 42 Set\n";
        return;
    }

    auto find = get_strsets_map().find(id);
    if (find != get_strsets_map().end()) {
        find->second.clear();
    }

    if (debug)
        std::cerr << "strset_clear: set " << id << " cleared\n";
}

int strset_comp(unsigned long id1, unsigned long id2) {
    if (debug)
        std::cerr << "strset_comp(" << id1 << ", " << id2 << ")\n";

    // We create a dummy empty and use for all non-existing ids, so that it is
    // treated the same as if it was empty. I made it static so that it is only
    // initialized once and we dont waste cycles every time we compare two
    // strsets.
    static Strset empty_set{};

    auto find1 = get_strsets_map().find(id1);
    auto set1_missing = (find1 == get_strsets_map().end());

    auto find2 = get_strsets_map().find(id2);
    auto set2_missing = (find2 == get_strsets_map().end());

    Strset& set1{ set1_missing ? empty_set : (*find1).second };
    Strset& set2{ set2_missing ? empty_set : (*find2).second };

    // This looks naive, because we traverse the containter twice, but GCC
    // somehow is able to optimize this perfectly.
    auto retval = (set1 < set2 ? -1 : (set2 < set1 ? 1 : 0));

    if (debug) {
        std::cerr << "strset_comp: result of comparing ";
        if (id1 == strset42())
            std::cerr << "the 42 Set";
        else
            std::cerr << "set " << id1;
        std::cerr << " to ";
        if (id2 == strset42())
            std::cerr << "the 42 Set";
        else
            std::cerr << "set " << id2;
        std::cerr << " is " << retval << "\n";

        if (set1_missing)
            std::cerr << "strset_comp: set " << id1 << " does not exist\n";
        if (set2_missing)
            std::cerr << "strset_comp: set " << id2 << " does not exist\n";
    }

    return retval;
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace jnp1
#endif
