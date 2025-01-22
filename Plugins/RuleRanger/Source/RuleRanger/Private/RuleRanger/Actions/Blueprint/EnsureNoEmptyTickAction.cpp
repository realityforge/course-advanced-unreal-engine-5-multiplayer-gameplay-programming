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
#include "EnsureNoEmptyTickAction.h"
#include "K2Node_Event.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnsureNoEmptyTickAction)

// ReSharper disable once CppMemberFunctionMayBeStatic
bool UEnsureNoEmptyTickAction::IsEmptyTick(const UK2Node_Event* Node)
{
    if (Node->IsAutomaticallyPlacedGhostNode())
    {
        return false;
    }
    else
    {
        const auto ExecThenPin = Node->FindPin(UEdGraphSchema_K2::PN_Then);
        return ExecThenPin && ExecThenPin->LinkedTo.IsEmpty();
    }
}

void UEnsureNoEmptyTickAction::Apply_Implementation(URuleRangerActionContext* ActionContext, UObject* Object)
{
    static const FName EventTickName(TEXT("ReceiveTick"));

    // ReSharper disable once CppTooWideScopeInitStatement
    const UBlueprint* Blueprint = CastChecked<UBlueprint>(Object);
    for (const auto Graph : Blueprint->UbergraphPages)
    {
        for (auto Node : Graph->Nodes)
        {
            // ReSharper disable once CppTooWideScopeInitStatement
            const auto EventNode = Cast<UK2Node_Event>(Node);
            if (EventNode && EventTickName == EventNode->EventReference.GetMemberName() && IsEmptyTick(EventNode))
            {
                ActionContext->Error(FText::FromString(TEXT("Blueprint has a Tick with no output pins connected. "
                                                            "This node still ticks (in < 5.6) and results in "
                                                            "unnecessary overhead. Please use or remove it.")));
            }
        }
    }
}

UClass* UEnsureNoEmptyTickAction::GetExpectedType()
{
    return UBlueprint::StaticClass();
}
