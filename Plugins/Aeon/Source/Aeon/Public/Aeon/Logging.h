/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "Logging/LogCategory.h"

// Category usage:
//	Warning/Error - Unexpected state or problem occured. Should be addressed.
//	Log - What occurred? Aeon users care about this.
//	Verbose - Why did it occur? Aeon developers care about this.
//  VeryVerbose - What didn't occur and why did it NOT occur? Aeon developers care about this.
//

// Log category to use within the Aeon plugin
AEON_API DECLARE_LOG_CATEGORY_EXTERN(Aeon, Display, All);

// Log categories that are primarily used when debugging or understanding Aeon interactions and
// are not expected to be used at runtime.
#ifdef AEON_DEBUG
AEON_API DECLARE_LOG_CATEGORY_EXTERN(AeonTagRelationship, Display, All);
#else
AEON_API DECLARE_LOG_CATEGORY_EXTERN(AeonTagRelationship, Display, Warning);
#endif

// A simplified set of macros for logging

#define AEON_LOG(Verbosity, Format, ...)                \
    {                                                   \
        UE_LOG(Aeon, Verbosity, Format, ##__VA_ARGS__); \
    }

#define AEON_SHOW_MESSAGE(Color, Format, ...)                                                         \
    {                                                                                                 \
        if (GEngine)                                                                                  \
        {                                                                                             \
            GEngine->AddOnScreenDebugMessage(-1, 7.f, Color, FString::Printf(Format, ##__VA_ARGS__)); \
        }                                                                                             \
    }

#define AEON_SHOW_INFO_MESSAGE(Format, ...) AEON_SHOW_MESSAGE(FColor::White, Format, ##__VA_ARGS__)

#define AEON_ERROR_LOG(Format, ...) \
    AEON_LOG(Error, Format, ##__VA_ARGS__) AEON_SHOW_MESSAGE(FColor::Red, Format, ##__VA_ARGS__)
#define AEON_WARNING_LOG(Format, ...) \
    AEON_LOG(Warning, Format, ##__VA_ARGS__) AEON_SHOW_MESSAGE(FColor::Yellow, Format, ##__VA_ARGS__)
#define AEON_INFO_LOG(Format, ...) AEON_LOG(Log, Format, ##__VA_ARGS__)
#define AEON_VERBOSE_LOG(Format, ...) AEON_LOG(Verbose, Format, ##__VA_ARGS__)
#define AEON_VERY_VERBOSE_LOG(Format, ...) AEON_LOG(VeryVerbose, Format, ##__VA_ARGS__)

#define AEON_ERROR_ALOG(Format, ...) AEON_ERROR_LOG(TEXT(Format), ##__VA_ARGS__)
#define AEON_WARNING_ALOG(Format, ...) AEON_WARNING_LOG(TEXT(Format), ##__VA_ARGS__)
#define AEON_INFO_ALOG(Format, ...) AEON_INFO_LOG(TEXT(Format), ##__VA_ARGS__)
#define AEON_VERBOSE_ALOG(Format, ...) AEON_VERBOSE_LOG(TEXT(Format), ##__VA_ARGS__)
#define AEON_VERY_VERBOSE_ALOG(Format, ...) AEON_VERY_VERBOSE_LOG(TEXT(Format), ##__VA_ARGS__)
