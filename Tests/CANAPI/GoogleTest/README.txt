Google Test Framework (GoogleTest) for CAN API V3 C++ Testing
=============================================================

GoogleTest (aka gtest) is a unit testing library for the C++ programming language,
released under the BSD 3-clause license and based on the xUnit architecture.

CAN API V3 is a wrapper specification to have a multi-vendor, cross-platform CAN API.
GoogleTest is used for testing of CAN API V3 C++ wrapper implementations.

Source Code Repositiory of GoogleTest
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
URL: https://github.com/google/googletest

Current Version used by CAN API V3 C++ Testing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
v1.14.0 (https://github.com/google/googletest/releases/tag/v1.14.0)

Installation and Usage of GoogleTest
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
1. Clone the GoogleTest main branch or download the sources from a tag
   e.g. into '~/Projects/gtest'
2. Copy all header files from '~/Projects/gtest\googletest\include'
   into '$(PROJROOT)/Tests/CANAPI/GoogleTest/include'
3. Build static libraries as Universal macOS Binary for macOS 11.0 with CMake
   note: GoogleMock (aka gmock) does not need to be created for this project.
4. Copy all files from '~/Projects/gtest/googletest/build/lib'
   into '$(PROJROOT)/Tests/CANAPI/GoogleTest/macOS/lib'
 
Important Notes
~~~~~~~~~~~~~~~
- Adapt the architecture and deployment settings according to the root project
- Since version 1.13.x Googletest requires at least C++14

Last Updated
~~~~~~~~~~~~
September 13, 2023
