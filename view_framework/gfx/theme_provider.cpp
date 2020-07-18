
#include "theme_provider.h"

// We have the destructor here because GCC puts the vtable in the first file
// that includes a virtual function of the class. Leaving it just in the .h file
// means that GCC will fail to link.

ThemeProvider::~ThemeProvider() {}