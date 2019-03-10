#ifndef STRSETCONST_H
#define STRSETCONST_H

#ifdef __cplusplus
  #ifndef NDEBUG
    #include <iostream>
  #endif
#endif

#ifdef __cplusplus
namespace jnp1 {
extern "C" {
#endif

// Zwraca identyfikator zbioru, którego nie można modyfikować i który zawiera
// jeden element: napis "42". Zbiór jest tworzony przy pierwszym wywołaniu tej
// funkcji i wtedy zostaje ustalony jego numer.
unsigned long strset42();

#ifdef __cplusplus
} // extern "C"
} // namespace jnp1
#endif

#endif
