// BuildInfo.h
//
#define BUILDINFO_COMPANY "RIGamortis Software"
#define BUILDINFO_YEAR "2022"
#define BUILDINFO_COPYRIGHT "© Maetrim"
#define BUILDINFO_PRODUCT "DDOBuilder"

// Version number changes to the appropriate level(s) should be made here
// ----------------------------------------------------------------------
#define BUILDINFO_VERSION_MAJOR 1
#define BUILDINFO_VERSION_MINOR 0
#define BUILDINFO_VERSION_MODIF 0
#define BUILDINFO_VERSION_BUILD 170

// Macro used to make combination strings automatically from the above
// -------------------------------------------------------------------
#define BLDSTR(X) DO_BLDSTR(X)
#define DO_BLDSTR(X) #X

// The X.X format is the headline value and is used in various places
// ------------------------------------------------------------------
#define BUILDINFO_VERSION_VV \
            BLDSTR(BUILDINFO_VERSION_MAJOR) "." BLDSTR(BUILDINFO_VERSION_MINOR)


// The X.X.X.X format is used in a few result doc tags, and the winapp version
// ---------------------------------------------------------------------------
#define BUILDINFO_VERSION BLDSTR(BUILDINFO_VERSION_MAJOR) "." \
                          BLDSTR(BUILDINFO_VERSION_MINOR) "." \
                          BLDSTR(BUILDINFO_VERSION_MODIF) "." \
                          BLDSTR(BUILDINFO_VERSION_BUILD)
