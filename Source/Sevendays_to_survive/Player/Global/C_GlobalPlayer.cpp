// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Global/C_GlobalPlayer.h"
#include "STS/C_STSInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Player/MainController/C_MainPlayerController.h"
#include "Player/Global/C_PlayerEnum.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AC_GlobalPlayer::AC_GlobalPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	//PlayerDT = STSInstance->GetPlayerDataTable();
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	
	//GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera"));
	SpringArm->SetupAttachment(RootComponent);
	//SpringArm->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	Camera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(Camera);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

// Called when the game starts or when spawned
void AC_GlobalPlayer::BeginPlay()
{
	Super::BeginPlay();
	UC_STSInstance* STSInstance = GetWorld()->GetGameInstanceChecked<UC_STSInstance>();
	CameraDT = STSInstance->GetPlayerDataTable()->CameraValue;
	PlayerDT = STSInstance->GetPlayerDataTable()->PlayerValue;
	
	// 카메라 데이터 테이블 값 가져오기
	{
		SpringArm->TargetArmLength = CameraDT.TargetArmLength;
		CameraRotSpeed = CameraDT.CameraRotSpeed;
	}

	
	// 플레이어 데이터 테이블 값 가져오기
	{
		GetCharacterMovement()->JumpZVelocity = PlayerDT.JumpZVelocity;
	}
	
	//STSInstance=GetWorld()->GetGameInstanceChecked<UC_STSInstance>();
	//Add Input Mapping Context
	if (AC_MainPlayerController* PlayerController = Cast<AC_MainPlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void AC_GlobalPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AC_GlobalPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AC_GlobalPlayer::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AC_GlobalPlayer::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>() * UGameplayStatics::GetWorldDeltaSeconds(this) * CameraRotSpeed;

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(-LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AC_GlobalPlayer::IdleStart()
{
}

void AC_GlobalPlayer::MoveStart()
{
}

void AC_GlobalPlayer::JumpStart()
{
}

void AC_GlobalPlayer::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AC_GlobalPlayer::GetHasRifle()
{
	return bHasRifle;
}

