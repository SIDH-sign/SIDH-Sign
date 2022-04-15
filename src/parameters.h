#ifndef SIDH_POK_PARAMETERS_H
#define SIDH_POK_PARAMETERS_H
/*
 * Functions with implementation depending on prime and optimization options
 */
#if defined(_P377_)
#include "P377/parameters_P377.h"
#elif defined(_P434_)
#include "P434/parameters_P434.h"
#elif defined(_P503_)
#include "P503/parameters_P503.h"
#elif defined(_P546_)
#include "P546/parameters_P546.h"
#elif defined(_P610_)
#include "P610/parameters_P610.h"
#elif defined(_P697_)
#include "P697/parameters_P697.h"
#elif defined(_P751_)
#include "P751/parameters_P751.h"
#else
#include "P434/parameters_P434.h"
#endif
#endif //SIDH_POK_PARAMETERS_H
