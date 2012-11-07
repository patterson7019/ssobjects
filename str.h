#ifndef STR_H
#define STR_H

// This is provided as a helper till I have converted the CStr class to 
// Str. I originally used CStr to avoid naming conflicts, but discovered
// namespaces shortly afterwards. I couldn't use String (& string.h) as 
// this would conflict with the std string.h that ships with compilers.
// So I have settled with Str, as this follows my naming convention. 

#include "cstr.h"

#define Str CStr

#endif //STR_H
