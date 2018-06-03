//
// This header contains macros for automatically defining vanilla values
//

#define VANILLA_DVAR(NAME, ADDRESS) static dvar_s*& NAME = *(dvar_s **)ADDRESS;
#define VANILLA_VALUE(NAME, TYPE, ADDRESS) static TYPE& NAME = *(TYPE*)ADDRESS;

#define VANILLA_FUNC(NAME, PTYPE, ADDRESS) static auto NAME = (PTYPE)ADDRESS; namespace func_types { using NAME##_t = PTYPE; }
