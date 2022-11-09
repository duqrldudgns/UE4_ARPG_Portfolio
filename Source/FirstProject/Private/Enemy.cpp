// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/AnimInstance.h"
#include "MainPlayerController.h"
#include "Math/UnrealMathUtility.h"
#include "EnemyAIController.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "AIController.h"
#include "Weapon.h"
#include "Main.h"
#include "Bow.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("WeaponSocket"));	// �ش� �̸��� ���� ���Ͽ� �ݸ����ڽ��� ����

	// Create SwordAttached
	SwordAttached = CreateDefaultSubobject<UChildActorComponent>(TEXT("SwordAttached"));
	SwordAttached->SetupAttachment(GetMesh(), "WeaponSocket");		// Attach the camera to the end of the boom and let the boom adjust to match 

	SelectDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectDecal"));
	SelectDecal->SetupAttachment(GetRootComponent());
	SelectDecal->SetVisibility(false);
	SelectDecal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	static ConstructorHelpers::FObjectFinder<UMaterial> Decal(TEXT("Material'/Game/GameplayMechanics/test/CharaSelectDecal/CharSelectDecal_MT.CharSelectDecal_MT'"));
	if (Decal.Succeeded())
	{
		SelectDecal->SetDecalMaterial(Decal.Object);
	}

	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());
	HealthBar->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	HealthBar->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBar->SetDrawSize(FVector2D(120.f, 15.f));
	HealthBar->SetVisibility(false);

	bOverlappingCombatSphere = false;
	bAttacking = false;
	bHasValidTarget = false;
	bDamagedIng = false;
	bHitOnce = false;
	bCanAttack = true;

	Health = 150.f;
	MaxHealth = 150.f;
	Damage = 10.f;

	DeathDelay = 5.0f;
	AttDelay = 5.0f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;

	DamageTypeClass = UDamageType::StaticClass();

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;	//������ ��ġ�ϰų� ���� �����Ǵ� Enemy�� EnemyAIController�� ���踦 �ްԵ�

	DetectRadius = 1300.f;

	DisplayHealthBarTime = 5.f;

	DamType = 0;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetMesh()->GetAnimInstance();		//�ִϸ��̼� �ν��Ͻ��� ������

	AIController = Cast<AAIController>(GetController());

	// �浹 ���� ����
	GetCapsuleComponent()->SetCollisionProfileName("Enemy");

	// ���� �浹 ���� ����
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // QueryOnly : ������ ��� ���� ����
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);	// ���Dynamic ��ҿ� ���� �浹
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);	// �� �浹�� ���� ������ �����ϰ�
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);	// Pawn�� ���� �浹�� Overlap���� ����

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSlowDebuff && !bSlowDebuffOnce)
	{
		bSlowDebuffOnce = true;

		FTimerHandle RecoveryHandle;
		GetWorld()->GetTimerManager().SetTimer(RecoveryHandle, FTimerDelegate::CreateLambda([&]()
			{
				bSlowDebuff = false;
				bSlowDebuffOnce = false;
			}), 0.8f, false);
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			
			FHitResult hitResult(ForceInit);

			if (DamageTypeClass)
			{
				//UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);	// ���ش��, ���ط�, ��Ʈ�ѷ�(������), ���� ������, �ջ�����
				if (DamType == 1)
					UGameplayStatics::ApplyPointDamage(Main, Damage, GetActorLocation(), hitResult, AIController, this, Main->Upper); // ����Ʈ ������
				else if (DamType == 2)
					UGameplayStatics::ApplyPointDamage(Main, Damage, GetActorLocation(), hitResult, AIController, this, Main->KnockDown); // ����Ʈ ������
				else if (DamType == 3)
					UGameplayStatics::ApplyPointDamage(Main, Damage, GetActorLocation(), hitResult, AIController, this, Main->Rush); // ����Ʈ ������
				else
					UGameplayStatics::ApplyPointDamage(Main, Damage, GetActorLocation(), hitResult, AIController, this, DamageTypeClass); // ����Ʈ ������
			}
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::MoveToTarget(class AMain* Target)	
{
	if (Alive())
	{
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	}
	GetCharacterMovement()->MaxWalkSpeed = 500.f;

	CombatTarget = nullptr;

	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);		// �̵� ��ǥ : ���
		MoveRequest.SetAcceptanceRadius(5.0f);	// ��ǥ Actor�� �������� �� ��ǥ Actor���� �Ÿ�

		FNavPathSharedPtr NavPath;				// ��ǥ�� ���󰡱� ���� ��� ����?

		AIController->MoveTo(MoveRequest, &NavPath);	//�̵�


		// Debug
		auto PathPoints = NavPath->GetPathPoints();		//��θ� ���� ���� �迭�� ����
		for (auto Point : PathPoints)
		{
			FVector Location = Point.Location;
			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 16, FLinearColor::Red, 5.f, 1.5f);
		}
	}
}
void AEnemy::UpperActivate()
{
	DamType = 1;
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // QueryOnly : ������ ��� ���� ����
}

void AEnemy::KnockDownActivate()
{
	DamType = 2;
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // QueryOnly : ������ ��� ���� ����
}

void AEnemy::RushActivate()
{
	DamType = 3;
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // QueryOnly : ������ ��� ���� ����
}

void AEnemy::ActivateCollision()
{
	DamType = 0;
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // QueryOnly : ������ ��� ���� ����
}

void AEnemy::DeActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	if (!bAttacking && Alive())// && bHasValidTarget)
	{
		bAttacking = true;
		bCanAttack = false;
		bDamagedIng = false;

		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);

		AttackDelay();

		if (AIController)
		{
			AIController->StopMovement();
		}

		int32 Num = FMath::RandRange(0, 2);
		TArray<FName> Section = {"Attack01", "Attack03", "Attack04"};

		if (AnimInstance && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(Section[Num], CombatMontage);
		}
	}
}

void AEnemy::AttackEnd()
{
	bAttacking = false;

	OnAttackEnd.Broadcast();	// ��������Ʈ�� ȣ��
	
}

void AEnemy::PlaySwingSound()
{
	if (SwingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SwingSound, GetActorLocation());
	}
}

bool AEnemy::DecrementHealth(float Amount)
{
	Health -= Amount;

	if (Health <= 0.f)
	{
		Die();
		return false;
	}

	return true;
}

void AEnemy::Die()
{
	bDie = true;
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	AMain* Main = Cast<AMain>(CombatTarget);
	if (Main)
	{
		Main->UpdateCombatTarget();
	}

	UAnimInstance* Montage = GetMesh()->GetAnimInstance();
	if (Montage->GetCurrentActiveMontage())
	{
		Montage->StopAllMontages(0.f);
	}

	if (AnimInstance && DamagedMontage)
	{
		AnimInstance->Montage_Play(DamagedMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"), DamagedMontage);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (DecrementHealth(DamageAmount))
	{
		AController* Coontroller = Cast<AController>(EventInstigator);
		if (Coontroller)	//���� ������ ���� �׳� �¾ƾ���, ��Ʈ�ѷ� �����ϴ� �����̶� �Ұ� �����غ�����
		{
			AMain* Main = Cast<AMain>(Coontroller->GetPawn());
			if (Main && AnimInstance && DamagedMontage && LightDamagedMontage)
			{
				DisplayHealthBar();

				// Show DamageNumbers
				if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))	// PointDamage �ޱ�
				{
					const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
					float DamageNumbers = PointDamageEvent->Damage;
					bool Headshot = false;
					if (0 == (PointDamageEvent->HitInfo.BoneName).Compare(FName(TEXT("Head"))))
					{
						DamageNumbers *= 1.5f;
						Headshot = true;
					}
					Main->SpawnDamageNumbers(PointDamageEvent->HitInfo.Location, Headshot, DamageNumbers);
					
					UE_LOG(LogTemp, Warning, TEXT("PointDamageTest"));

					//��弦�ϰ�� �߰� ������
					if (!DecrementHealth(DamageNumbers - PointDamageEvent->Damage)) return DamageNumbers;
				}

				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Main->GetActorLocation());
				LookAtRotation.Pitch = 0.f;
				LookAtRotation.Roll = 0.f;		// (0.f, LookAtRotation.Yaw, 0.f);
				//UE_LOG(LogTemp, Warning, TEXT("%s"), *LookAtRotation.ToString());
				
				FRotator HitRotation = UKismetMathLibrary::NormalizedDeltaRotator(LookAtRotation, GetActorRotation());
				UE_LOG(LogTemp, Warning, TEXT("%s"), *HitRotation.ToString());

				UE_LOG(LogTemp, Warning, TEXT("%d"), bAttacking);
				if (DamageEvent.DamageTypeClass == Main->Basic  && !bAttacking)
				{
					bDamagedIng = true;

					if (GetCharacterMovement()->IsWalking())
					{
						if (-45.f <= HitRotation.Yaw && HitRotation.Yaw < 45.f)
						{
							AnimInstance->Montage_Play(LightDamagedMontage, 1.0f);
							AnimInstance->Montage_JumpToSection(FName("FrontHit"), LightDamagedMontage);
						}
						else if (-135.f <= HitRotation.Yaw && HitRotation.Yaw < -45.f)
						{
							AnimInstance->Montage_Play(LightDamagedMontage, 1.0f);
							AnimInstance->Montage_JumpToSection(FName("LeftHit"), LightDamagedMontage);
						}
						else if (45.f <= HitRotation.Yaw && HitRotation.Yaw < 135.f)
						{
							AnimInstance->Montage_Play(LightDamagedMontage, 1.0f);
							AnimInstance->Montage_JumpToSection(FName("RightHit"), LightDamagedMontage);
						}
						else
						{
							AnimInstance->Montage_Play(LightDamagedMontage, 1.0f);
							AnimInstance->Montage_JumpToSection(FName("BackHit"), LightDamagedMontage);
						}
					}
					else	// ���� ���� ���� ��
					{

						GetCharacterMovement()->SetMovementMode(MOVE_Flying);
						GetCharacterMovement()->Velocity = FVector(0.f);

						AnimInstance->Montage_Play(DamagedMontage, 1.0f);
						AnimInstance->Montage_JumpToSection(FName("UpperHit"), DamagedMontage);
					}
				}
				else if (DamageEvent.DamageTypeClass == Main->KnockDown)
				{
					bDamagedIng = true;
					bAttacking = false;
					DeActivateCollision();

					AnimInstance->Montage_Play(DamagedMontage, 1.0f);
					AnimInstance->Montage_JumpToSection(FName("KnockDown"), DamagedMontage);

					FVector LaunchVelocity = GetActorUpVector() * -1500.f;
					LaunchCharacter(LaunchVelocity, false, false);
				}
				else if (DamageEvent.DamageTypeClass == Main->Upper)
				{
					bDamagedIng = true;
					bAttacking = false;
					DeActivateCollision();

					AnimInstance->Montage_Play(DamagedMontage, 1.0f);
					AnimInstance->Montage_JumpToSection(FName("Upper"), DamagedMontage);

					SetActorRotation(LookAtRotation);
					FVector LaunchVelocity = GetActorUpVector() * 750.f;
					LaunchCharacter(LaunchVelocity, false, false);
				}
				else if (DamageEvent.DamageTypeClass == Main->Rush)
				{
					bDamagedIng = true;
					bAttacking = false;
					DeActivateCollision();

					AnimInstance->Montage_Play(DamagedMontage, 1.0f);
					AnimInstance->Montage_JumpToSection(FName("Rush"), DamagedMontage);

					SetActorRotation(LookAtRotation);
					FVector LaunchVelocity = GetActorForwardVector() * -1500.f + GetActorUpVector() * 40.f;
					LaunchCharacter(LaunchVelocity, false, false);
				}
			}
		}
	}

	return DamageAmount;
}

void AEnemy::AirHitEnd()
{
	if (GetCharacterMovement()->IsFlying())
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
}

void AEnemy::DamagedDownEnd()
{
	if (GetCharacterMovement()->IsWalking())	// ���鿡 ��Ҵٸ�
	{
		if (AnimInstance && DamagedMontage)
		{
			AnimInstance->Montage_Play(DamagedMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("UpperEnd"), DamagedMontage);
		}
	}
}

void AEnemy::DamagedEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("DamagedEnd Test"));
	bDamagedIng = false;
	ResetCasting();
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;			// �ִϸ��̼� ����
	GetMesh()->bNoSkeletonUpdate = true;	// ���̷��� ������Ʈ ����

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

bool AEnemy::Alive()	// ��������� true
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear()
{
	Destroy();
}

void AEnemy::SetEnemyMovementStatus(EEnemyMovementStatus Status)
{
	EnemyMovementStatus = Status;
}

void AEnemy::PlayMontage(UAnimMontage* MontageName)
{
	AnimInstance->Montage_Play(MontageName, 1.0f);
}

void AEnemy::DisplayHealthBar()
{
	if (!HealthBar->GetVisibleFlag())
	{
		HealthBar->SetVisibility(true);
		GetWorldTimerManager().SetTimer(DisplayHealthBarHandle, this, &AEnemy::RemoveHealthBar, DisplayHealthBarTime);
	}
	else 
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		GetWorldTimerManager().SetTimer(DisplayHealthBarHandle, this, &AEnemy::RemoveHealthBar, DisplayHealthBarTime);
	}
}

void AEnemy::RemoveHealthBar()
{
	HealthBar->SetVisibility(false);
}

void AEnemy::ResetCasting()	//���� ���� �߿� �ٸ� ������ �䱸�� ��
{
	AttackEnd();
	AirHitEnd();

	UAnimInstance* Montage = GetMesh()->GetAnimInstance();
	if (Montage->GetCurrentActiveMontage())
	{
		Montage->StopAllMontages(0.f);
	}
}

void AEnemy::AttackDelay()
{
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			bCanAttack = true;
		}), AttDelay, false);
}

void AEnemy::ResetHitOnce()
{
	FTimerHandle WaitHandle;
	float WaitTime = 0.2f;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			bHitOnce = false;
		}), WaitTime, false);
}

void AEnemy::StopAnim()
{
	UAnimInstance* Montage = GetMesh()->GetAnimInstance();
	if (!Montage->GetCurrentActiveMontage())
	{
		bFreezing = true;
	}
}
