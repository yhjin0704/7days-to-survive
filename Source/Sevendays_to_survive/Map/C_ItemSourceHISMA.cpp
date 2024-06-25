// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/C_ItemSourceHISMA.h"

#include "Kismet/KismetSystemLibrary.h"
#include "STS/C_STSInstance.h"
#include "Map/C_MapDataAsset.h"
#include "Map/C_ItemSourceTableRow.h"
#include "Map/C_Items.h"

AC_ItemSourceHISMA::AC_ItemSourceHISMA()
{
	PrimaryActorTick.bCanEverTick = true;

	HISMComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HierarchicalInstancedStaticMesh"));
	SetRootComponent(HISMComponent);
}

void AC_ItemSourceHISMA::BeginPlay()
{
	Super::BeginPlay();

	FString Message = FString::Printf(TEXT("%s's BeginPlay called."), *Id.ToString());
	UKismetSystemLibrary::PrintString(GetWorld(), *Message);

	if (true == Id.IsNone())
	{
		return;
	}

	UC_STSInstance* Inst = GetWorld()->GetGameInstanceChecked<UC_STSInstance>();
	UC_MapDataAsset* MapDataAsset = Inst->GetMapDataAsset();

	DropItems = MapDataAsset->GetItemSourceDropItems(Id);

	int MaxHp = MapDataAsset->GetItemSourceMaxHp(Id);
	int InstCount = HISMComponent->GetInstanceCount();
	for (int i = 0; i < InstCount; ++i)
	{
		MaxHpMap.Emplace(i, MaxHp);
		HpMap.Emplace(i, MaxHp);
	}
}

void AC_ItemSourceHISMA::Damage_Implementation(int _Index, int _Damage)
{
	if (true == Id.IsNone())
	{
		FString Message = FString::Printf(TEXT("[%s:%d] Id is not set. Can't be damaged."), *GetName(), _Index);
		UKismetSystemLibrary::PrintString(GetWorld(), Message);
		return;
	}

	HpMap[_Index] -= _Damage;

	FString Message0 = FString::Printf(TEXT("[%s:%d] damaged by %d."), *GetName(), _Index, _Damage);
	FString Message1 = FString::Printf(TEXT("[%s:%d] HP: %d/%d"), *GetName(), _Index, HpMap[_Index], MaxHpMap[_Index]);
	UKismetSystemLibrary::PrintString(GetWorld(), Message0);
	UKismetSystemLibrary::PrintString(GetWorld(), Message1);

	if (HpMap[_Index] <= 0)
	{
		bool Result = HISMComponent->RemoveInstance(_Index);

		FString Message;
		if (false == Result)
		{
			Message = FString::Printf(TEXT("[%s:%d] destroy failed."), *GetName(), _Index);
		}
		else
		{
			Message = FString::Printf(TEXT("[%s:%d] destroy success."), *GetName(), _Index);
		}
		UKismetSystemLibrary::PrintString(GetWorld(), Message);
	}

	for (TPair<const UC_Item*, int>& DropItem : DropItems)
	{
		FString ItemName = DropItem.Key->Name;
		FString Message = FString::Printf(TEXT("you've got %d %ss."), DropItem.Value, *ItemName);
		UKismetSystemLibrary::PrintString(GetWorld(), Message);
	}
}
