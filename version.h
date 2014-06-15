#ifndef _VERSION_H
#define _VERSION_H

#define program_version "v0.1.3-9-g60a091fM"

#define program_name	"%s"
#ifdef PCS_API_VERSION
#  define program_api_version " (API " PCS_API_VERSION ")"
#else
#  define program_api_version
#endif
#if defined(DEBUG) || defined(_DEBUG)
#  define program_full_name program_name "(debug) " program_version program_api_version
#else
#  define program_full_name program_name " " program_version program_api_version
#endif

#endif
