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

#include "RuleRangerObjectBase.h"
#include "Logging/StructuredLog.h"
#include "RuleRangerLogging.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RuleRangerObjectBase)

void URuleRangerObjectBase::LogInfo(const UObject* const Object, const FString& Message) const
{
    UE_LOGFMT(RuleRanger,
              VeryVerbose,
              "{Type}:{Instance}: {Message}",
              GetClass()->GetName(),
              Object ? Object->GetName() : TEXT("-"),
              Message);
}

void URuleRangerObjectBase::LogError(const UObject* const Object, const FString& Message) const
{
    UE_LOGFMT(RuleRanger,
              Error,
              "{Type}:{Instance}: {Message}",
              GetClass()->GetName(),
              Object ? Object->GetName() : TEXT("-"),
              Message);
}
