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
#include "EnsureEveryStaticMeshComponentReferencesAStaticMeshAction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnsureEveryStaticMeshComponentReferencesAStaticMeshAction)

void UEnsureEveryStaticMeshComponentReferencesAStaticMeshAction::Apply_Implementation(
    URuleRangerActionContext* ActionContext,
    UObject* Object)
{
    // ReSharper disable once CppTooWideScopeInitStatement
    const auto Actor = CastChecked<AActor>(Object);
    TArray<UStaticMeshComponent*> Components;
    Actor->GetComponents<UStaticMeshComponent>(Components);
    for (const auto Component : Components)
    {
        if (!Component->GetStaticMesh())
        {
            const auto OutMessage =
                FString::Printf(TEXT("StaticMeshComponent named '%s' does not reference a valid StaticMesh"),
                                *Component->GetName());
            ActionContext->Error(FText::FromString(OutMessage));
        }
    }
}

UClass* UEnsureEveryStaticMeshComponentReferencesAStaticMeshAction::GetExpectedType()
{
    return AActor::StaticClass();
}
