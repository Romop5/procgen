#include <procgen/interpret/types.h>
namespace ProcGen {
// Register all built in C++ types
FORALL_ATOMICTYPES(KEYWORD);
}
