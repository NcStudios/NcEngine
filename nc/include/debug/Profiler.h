#pragma once

/** Profiling with Optick requires the Optick GUI application.
 *  The shared library must also be copied to the executable's
 *  directory (nc/lib/libOptickCore.dll). */
#ifdef NC_PROFILING_ENABLED
#include "optick/optick.h"
#endif