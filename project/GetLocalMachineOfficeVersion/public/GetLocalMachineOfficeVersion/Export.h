#ifndef _OCLS_EXPORT_H
#define _OCLS_EXPORT_H
#ifdef  GETLOCALMACHINEOFFICEVERSION_EXPORTS
#define GETLOCALMACHINEOFFICEVERSION_API __declspec(dllexport)
#else
#define GETLOCALMACHINEOFFICEVERSION_API __declspec(dllimport)
#endif

#endif