// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/C_ItemSourceHISMA.h"

#include "Kismet/KismetSystemLibrary.h"
#include "STS/C_STSInstance.h"
#include "Map/C_MapDataAsset.h"
#include "Map/C_ItemSourceTableRow.h"
#include "Map/C_Items.h"
#include "Player/Global/C_GlobalPlayer.h"
#include "Inventory/C_InventoryComponent.h"

AC_ItemSourceHISMA::AC_ItemSourceHISMA()
{
	PrimaryActorTick.bCanEverTick = true;

	HISMComponent = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HierarchicalInstancedStaticMesh"));
	SetRootComponent(HISMComponent);
}

void AC_ItemSourceHISMA::BeginPlay()
{
	Super::BeginPlay();

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

void AC_ItemSourceHISMA::Damage_Implementation(APlayerController* _CallingController, int _Index, int _Damage)
{
	if (true == Id.IsNone())
	{
		FString Message = FString::Printf(TEXT("[%s:%d] Id is not set. Can't be damaged."), *GetName(), _Index);
		UKismetSystemLibrary::PrintString(GetWorld(), Message);
		return;
	}

	HpMap[_Index] -= _Damage;

	UPlayer* DamagePlayer = _CallingController->GetNetOwningPlayer();
	FString PlayerName = DamagePlayer->GetName();

	FString Message0 = FString::Printf(TEXT("[%s:%d] %s dealt %d damage."), *Id.ToString(), _Index, *PlayerName, _Damage);
	FString Message1 = FString::Printf(TEXT("[%s:%d] HP: %d/%d"), *Id.ToString(), _Index, HpMap[_Index], MaxHpMap[_Index]);
	UKismetSystemLibrary::PrintString(GetWorld(), Message0);
	UKismetSystemLibrary::PrintString(GetWorld(), Message1);

	if (HpMap[_Index] <= 0)
	{
		RemoveInstance_Internal(_Index);
		RemoveInstanceMulticast(_Index);

		FString	Message = FString::Printf(TEXT("[%s:%d] destroyed successfully."), *GetName(), _Index);
		UKismetSystemLibrary::PrintString(GetWorld(), Message);
	}

	// 플레이어 캐릭터의 인벤토리 컴포넌트에 아이템 추가
	AC_GlobalPlayer* PlayerCharacter = Cast<AC_GlobalPlayer>(_CallingController->GetPawn());
	if (nullptr == PlayerCharacter)
	{
		UE_LOG(LogTemp, Fatal, TEXT("[%s] Can't find PlayerCharacter."), __FUNCTION__);
	}

	UC_InventoryComponent* InventoryComponent = PlayerCharacter->GetComponentByClass<UC_InventoryComponent>();
	if (nullptr == InventoryComponent)
	{
		UE_LOG(LogTemp, Fatal, TEXT("[%s] Can't find InventoryComponent."), __FUNCTION__);
	}

	for (FC_ItemAndCount& DropItem : DropItems)
	{
		FString ItemName = DropItem.Item->Name;
		FString Message = FString::Printf(TEXT("[%s:%d] %s got %d %s."), *Id.ToString(), _Index, *PlayerName, DropItem.Count, *ItemName);
		InventoryComponent->AddItem(DropItem.Item, DropItem.Count);
		UKismetSystemLibrary::PrintString(GetWorld(), Message);
	}
}

void AC_ItemSourceHISMA::RemoveInstanceMulticast_Implementation(int _Index)
{
	RemoveInstance_Internal(_Index);
}

void AC_ItemSourceHISMA::RemoveInstance_Internal(int _Index)
{
	bool Result = HISMComponent->RemoveInstance(_Index);
	if (false == Result)
	{
		UE_LOG(LogTemp, Fatal, TEXT("[%s:%d] remove instance failed."), *GetName(), _Index);
		return;
	}
}
