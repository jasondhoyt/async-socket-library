// Copyright (c) 2025-present, Jason Hoyt
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#if defined(ASL_SHARED_LIB)
#if defined(_WIN32)
#if defined(asl_EXPORTS)
#define ASL_API __declspec(dllexport)
#else
#define ASL_API __declspec(dllimport)
#endif
#else
#define ASL_API __attribute__((visibility("default)))
#endif
#else
#define ASL_API
#endif