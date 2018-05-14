/**
 * @file 'interpret/types.cpp'
 * @brief Registration of types 
 * @copyright The MIT license 
 */
#include <procgen/interpret/types.h>
namespace ProcGen {
// Register all built in C++ types
FORALL_ATOMICTYPES(KEYWORD);
}
