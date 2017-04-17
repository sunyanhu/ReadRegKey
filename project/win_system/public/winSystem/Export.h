#ifndef _OCLS_WIN_SYSTEM_EXPORT_H_
#define _OCLS_WIN_SYSTEM_EXPORT_H_
#include <osal/Osal.h>
#if defined (MAKINGDLL_WINSYSTEM)
#define OCLS_WINSYSTEM_API OSALEXPORT
#elif defined (USINGDLL_WINSYSTEM)
#define OCLS_WINSYSTEM_API OSALIMPORT
#else
#define OCLS_WINSYSTEM_API
#endif
#endif