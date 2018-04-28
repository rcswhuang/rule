#ifndef HRULEEXPORT_H
#define HRULEEXPORT_H

#if defined (_WIN32) || defined (WIN32)
    #if defined(RULE_DLL)
        #define RULE_EXPORT __declspec(dllexport)
    #else
        #define RULE_EXPORT __declspec(dllimport)
    #endif
#else
    #define RULE_EXPORT

#endif
#endif // HRULEEXPORT_H

