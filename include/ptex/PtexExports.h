#ifndef PtexExports_h
#define PtexExports_h
/*
PTEX SOFTWARE
Copyright 2021 Disney Enterprises, Inc.  All rights reserved

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.

  * The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
    Studios" or the names of its contributors may NOT be used to
    endorse or promote products derived from this software without
    specific prior written permission from Walt Disney Pictures.

Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/

/** @file PtexExports.h
    @brief Definitions related to exported Ptex API symbol visibility.
*/
#include "PtexPlatform.h"

#ifdef PTEX_STATIC
#   define PTEXAPI
#else
#   if defined(PTEX_PLATFORM_WINDOWS)
#       if defined(PTEX_EXPORTS)
#           define PTEXAPI __declspec(dllexport)
#       else
#           define PTEXAPI
#       endif
#   elif defined(PTEX_COMPILER_CLANG) || defined(PTEX_COMPILER_GCC) \
        || defined(PTEX_COMPILER_ICC)
#       if defined(PTEX_EXPORTS)
#           define PTEXAPI __attribute__((visibility("default")))
#       else
#           define PTEXAPI
#       endif
#   else
#       define PTEXAPI
#   endif
#endif

#endif  // PtexExports_h
