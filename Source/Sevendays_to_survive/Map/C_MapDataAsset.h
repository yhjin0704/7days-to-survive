// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Map/C_MapEnums.h"
#include "C_MapDataAsset.generated.h"

struct FC_ItemSourceTableRow;
class UC_STSInstance;
class UC_Item;
class AC_ItemPouch;

UCLASS()
class SEVENDAYS_TO_SURVIVE_API UC_MapDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	void Init(UC_STSInstance* _Inst);

	UFUNCTION(BlueprintPure)
	int GetItemSourceMaxHp(FName _Id);

	UFUNCTION(BlueprintPure)
	const UC_Item* FindItem(FName _Id);

	TMap<FName, int> GetItemSourceDropItems(FName _Id);

	UFUNCTION(BlueprintPure)
	TSubclassOf<AC_ItemPouch> GetItemPouchClass() const;

	UFUNCTION(BlueprintPure)
	TArray<FC_ItemAndCount> GetRandomDropItems();

	UFUNCTION(BlueprintCallable)
	int GetItemBoxMaxHp() const;

	UFUNCTION(BlueprintPure)
	TArray<FName> GetCraftItems(int _Page, int _PageSize);

	UFUNCTION(BlueprintPure)
	int GetCraftListMaxPage(int _PageSize);

private:
	FC_ItemSourceTableRow* FindItemSourceRow(FName _Id);

	int BisectRight(TArray<int>& _Arr, int _Value);

private:
	UC_STSInstance* Inst = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	int ItemBoxMaxHp = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* ItemSourceTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
	UDataTable* ItemTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
	UDataTable* MaterialTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
	UDataTable* ConsumableTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable", meta = (AllowPrivateAccess = "true"))
	UDataTable* BuildingPartTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TMap<FName, UC_Item*> Items;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TArray<FName> CraftItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AC_ItemPouch> ItemPouchClass = nullptr;

	// 가중치 기반 아이템 드롭
	TMap<EItemType, TArray<FName>> Type_To_AccDropIds;
	TMap<EItemType, TArray<int>> Type_To_AccDropWeights;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TMap<EItemType, int> Type_To_DropPouchCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TMap<EItemType, int> Type_To_DropItemMinCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TMap<EItemType, int> Type_To_DropItemMaxCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TMap<EItemType, int> NoneDropWeights;
};
