#ifndef _DECLARATIONS_H_
#define _DECLARATIONS_H_

#ifdef DEVELOPMENT_FEATURES
    #define DEVELOPMENT_PARAMETER(parameter)    , parameter
#else
    #define DEVELOPMENT_PARAMETER(parameter)
#endif

#endif
