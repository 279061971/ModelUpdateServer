#pragma once

#define EXPORT_MODELINTERSERVER
#ifdef EXPORT_MODELINTERSERVER
#define EXPORTMODELINTERSERVER  __declspec( dllexport )
#else
#define EXPORTMODELINTERSERVER   __declspec( dllimport )
#endif