/***
 * 
 * Version Data (Minimal Library Version)
 * 
 *****************************************************************************/

#define LIB_ID "DUSK"
#define RES_LIB_ID "$LIB " LIB_ID

#ifndef VERSION_ID_DEF
#define VERSION_ID_DEF "unknown"
#endif
#define RES_LIB_VERSION (RES_LIB_ID " $VER " VERSION_ID_DEF)

#ifndef LIB_BUILD_ID_DEF
#define LIB_BUILD_ID_DEF "unknown"
#endif
#define RES_LIB_BUILD (RES_LIB_ID " $BUILD " LIB_BUILD_ID_DEF)

__attribute__((used)) const char* LIB_VERSION = RES_LIB_VERSION;
__attribute__((used)) const char* LIB_BUILD = RES_LIB_BUILD;
