//  Workaround - Dummy build number for Swift Package Manager (SPM)
//
//  Note: Unfortunately, the whole trick of autogenerating a pseudo build number
//        from git hash of the last commit doesn't work with SPM.
//
#ifndef BUILD_NO_H_INCLUDED
#define BUILD_NO_H_INCLUDED
#define BUILD_NO 0x1010101
#define STRINGIFY(X) #X
#define TOSTRING(X) STRINGIFY(X)
#endif
