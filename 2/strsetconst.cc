#include <iostream>

#include "strset.h"
#include "strsetconst.h"

#ifdef NDEBUG
const bool debug{false};
#else
const bool debug{true};
#endif

namespace {

bool& const_set_already_created() {
    static bool const_set_already_created{false};
    return const_set_already_created;
}

unsigned long& const_set_id() {
    static unsigned long const_set_id{4294967295};
    return const_set_id;
}

}  // namespace

#ifdef __cplusplus
namespace jnp1 {
extern "C" {
#endif

unsigned long strset42() {
    if (!const_set_already_created()) {
        if (debug)
            std::cerr << "strsetconst init invoked\n";

        const_set_already_created() = true;
        auto created_const_set_id = strset_new();
        strset_insert(created_const_set_id, "42");
        const_set_id() = created_const_set_id;

        if (debug)
            std::cerr << "strsetconst init finished\n";
    }

    return const_set_id();
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace jnp1
#endif
