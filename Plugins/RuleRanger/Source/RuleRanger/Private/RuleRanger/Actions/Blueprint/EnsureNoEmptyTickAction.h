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

#include "CoreMinimal.h"
#include "RuleRangerAction.h"
#include "EnsureNoEmptyTickAction.generated.h"

class UK2Node_Event;

/**
 * Action to check that Blueprints assets have no empty tick nodes.
 */
UCLASS(DisplayName = "Ensure Blueprints have No Empty Tick Nodes")
class RULERANGER_API UEnsureNoEmptyTickAction final : public URuleRangerAction
{
    GENERATED_BODY()

    bool IsEmptyTick(const UK2Node_Event* Node);

public:
    virtual void Apply_Implementation(URuleRangerActionContext* ActionContext, UObject* Object) override;

    virtual UClass* GetExpectedType() override;
};
