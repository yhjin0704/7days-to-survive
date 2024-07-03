// Fill out your copyright notice in the Description page of Project Settings.

#include "Map/C_MapInteractionComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Map/C_ItemSourceHISMA.h"
#include "Map/C_ItemPouch.h"
#include "Player/Global/C_MapPlayer.h"
#include "Camera/CameraComponent.h"
#include "STS/C_STSMacros.h"

UC_MapInteractionComponent::UC_MapInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UC_MapInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner<AC_MapPlayer>();
	CameraComponent = Owner->GetComponentByClass<UCameraComponent>();
}

void UC_MapInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UC_MapInteractionComponent::DamageItemSource(AC_ItemSourceHISMA* _ItemSource, int _Index, int _Damage)
{
	AC_MapPlayer* HitCharacter = Cast<AC_MapPlayer>(GetOwner());
	if (nullptr == HitCharacter)
	{
		STS_FATAL("[%s] HitCharacter is NULL.", __FUNCTION__);
	}

	_ItemSource->Damage(_Index, _Damage, HitCharacter);
	//_ItemSource->GainDropItems(HitCharacter);
}

void UC_MapInteractionComponent::Server_DamageItemSource_Implementation(APlayerController* _CallingController, AC_ItemSourceHISMA* _ItemSource, int _Index, int _Damage)
{
	_ItemSource->Damage(_Index, _Damage, nullptr);
	Multicast_DamageItemSource(_CallingController, _ItemSource, _Index, _Damage);
}

void UC_MapInteractionComponent::Multicast_DamageItemSource_Implementation(APlayerController* _CallingController, AC_ItemSourceHISMA* _ItemSource, int _Index, int _Damage)
{
	// 서버는 멀티캐스트에서 제외한다.
	if (true == IsServer())
	{
		return;
	}

	// 멀티캐스트를 야기한 플레이어는 멀티캐스트에서 제외한다.
	if (true == IsValid(_CallingController) && true == _CallingController->IsLocalController())
	{
		return;
	}

	_ItemSource->Damage(_Index, _Damage, nullptr);
}

bool UC_MapInteractionComponent::IsServer() const
{
	return UKismetSystemLibrary::IsServer(GetWorld());
}

bool UC_MapInteractionComponent::IsOwnerLocallyControlled() const
{
	return Owner->IsLocallyControlled();
}

FVector UC_MapInteractionComponent::GetHpBarTraceStartPoint() const
{
	FVector CameraLocation = CameraComponent->GetComponentLocation();
	FVector CameraForward = CameraComponent->GetForwardVector();
	return CameraLocation + CameraForward * HpBarTraceStartRange;
}

FVector UC_MapInteractionComponent::GetHpBarTraceEndPoint() const
{
	FVector CameraLocation = CameraComponent->GetComponentLocation();
	FVector CameraForward = CameraComponent->GetForwardVector();
	return CameraLocation + CameraForward * HpBarTraceEndRange;
}

FRotator UC_MapInteractionComponent::GetCameraRotation() const
{
	return CameraComponent->GetComponentRotation();
}

void UC_MapInteractionComponent::ProcessItemSourceTraceResult(FHitResult _HitResult, bool _IsHit)
{
	// 아이템 소스를 보지 않게 되는 경우
	if (false == _IsHit)
	{
		if (true == IsValid(ViewingItemSource))
		{
			ViewingItemSource->HideHpBar();
		}
		ViewingItemSource = nullptr;
		return;
	}

	// 같은 아이템 소스를 계속 보는 경우
	if (ViewingItemSource == _HitResult.GetActor())
	{
		ViewingItemSource->UpdateHpBar(_HitResult.Item);
		return;
	}

	// 다른 아이템 소스를 보게 되는 경우
	if (true == IsValid(ViewingItemSource))
	{
		ViewingItemSource->HideHpBar();
	}
	ViewingItemSource = Cast<AC_ItemSourceHISMA>(_HitResult.GetActor());
	ViewingItemSource->UpdateHpBar(_HitResult.Item);
}

void UC_MapInteractionComponent::ProcessItemPouchTraceResult(FHitResult _HitResult, bool _IsHit)
{
	if (true == _IsHit)
	{
		AC_ItemPouch* ItemPouch = Cast<AC_ItemPouch>(_HitResult.GetActor());
		if (false == IsValid(ItemPouch))
		{
			STS_FATAL("[%s] Given item pouch is invalid.", __FUNCTION__);
		}
		ViewingItemPouch = ItemPouch;
		return;
	}

	ViewingItemPouch = nullptr;
}
