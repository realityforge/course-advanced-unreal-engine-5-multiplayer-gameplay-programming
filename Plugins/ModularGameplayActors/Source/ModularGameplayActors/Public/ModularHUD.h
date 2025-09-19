// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/HUD.h"

#include "ModularHUD.generated.h"

namespace EEndPlayReason { enum Type : int; }

/** Minimal class that supports extension by game feature plugins */
UCLASS(Abstract, Blueprintable)
class AModularHUD : public AHUD
{
	GENERATED_BODY()

protected:

	//~UObject interface
	virtual void PreInitializeComponents() override;
	//~End of UObject interface

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of AActor interface
};
