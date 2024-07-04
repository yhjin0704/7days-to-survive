// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "STSBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SEVENDAYS_TO_SURVIVE_EDITOR_API USTSBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static void GetAllFileName();

	UFUNCTION(BlueprintCallable)
	static void GetOpenDirectoryDialog(FString DirectoryPath, FString& Directory, bool& IsSelet);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TArray<FString> FoundFileNames;
	
	

	
};
