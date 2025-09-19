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
//	Warning/Error - Unexpected state or problem occurred. Should be addressed.
//	Log - What occurred? Aeon users care about this.
//	Verbose - Why did it occur? Aeon developers care about this.
//  VeryVerbose - What didn't occur and why did it NOT occur? Aeon developers care about this.
//

// Log category to use within the Aeon plugin
AEON_API DECLARE_LOG_CATEGORY_EXTERN(LogAeon, Display, All);

// Log categories that are primarily used when debugging or understanding Aeon interactions and
// are not expected to be used at runtime.
#ifdef AEON_DEBUG
AEON_API DECLARE_LOG_CATEGORY_EXTERN(LogAeonTagRelationship, Display, All);
#else
AEON_API DECLARE_LOG_CATEGORY_EXTERN(LogAeonTagRelationship, Display, Warning);
#endif
