#pragma once


#define EXPORT_MODELUPDATESERVER
#ifdef EXPORT_MODELUPDATESERVER
#define EXPORTMODELUPDATESERVER  __declspec( dllexport )
#else
#define EXPORTMODELUPDATESERVER   __declspec( dllimport )
#endif
