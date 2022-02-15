#pragma once

// Pragmas to override compiler options on online judges.

#ifndef _CPLIB_LEGACY_ARCH_
#pragma GCC target("abm,bmi,bmi2")
#endif