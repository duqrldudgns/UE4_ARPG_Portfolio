// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "particles/ParticleSystemComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Enemy.h"
#include "Main.h"

AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	bWeaponParticles = false;

	WeaponState = EWeaponState::EWS_Pickup;

	Damage = 35.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// �浹 ���� ����
	CombatCollision->SetCollisionProfileName("Sword");

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);
}


void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			Main->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::Equip(AMain* Character)
{
	if (Character)
	{
		WeaponOwner = Cast<AMain>(Character);

		// ��� ������ �� �� ĳ������ ��Ʈ�ѷ��� �����ؾ� ���� ���ظ� �ִ��� �� �� �ְ� ���ظ� �� �� ����
		SetInstigator(Character->GetController());

		// ī�޶�� �� ĳ���Ϳ� ���� �浹 ����
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);	// ������ �ùķ��̼��� �ϰ� �ִٸ� �ߴ��ϴ� ��.. ??

		// "RightHandSocket" �̶�� �̸��� ������ ������
		const USkeletalMeshSocket* RightHandSocket = Character->GetMesh()->GetSocketByName("RightHandSocket");
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(this, Character->GetMesh());	// �� ĳ������ ���Ͽ� �� ���⸦(this) ����
			bRotate = false;

			// ���� ��� �ı�
			Character->SetEquippedWeapon(this);	// Main���� �� ���⸦ �ٷ� �� �ְ� ��
			Character->SetActiveOverlappingItem(nullptr);	// ���� ������ ��� overlap ó�� ������

			WeaponState = EWeaponState::EWS_Equipped;
		}

		if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);

		if (!bWeaponParticles) IdleParticlesComponent->Deactivate();

	}
}

void AWeapon::UnEquip()
{
	if (WeaponOwner) {
		const USkeletalMeshSocket* SwordBack = WeaponOwner->GetMesh()->GetSocketByName("Sword_Back");
		if (SwordBack)
		{
			SwordBack->AttachActor(this, WeaponOwner->GetMesh());
			
			WeaponOwner->SetEquippedWeapon(nullptr);
			
			WeaponState = EWeaponState::EWS_Pickup;
		}

		if (OnEquipSound) UGameplayStatics::PlaySound2D(this, OnEquipSound);
	}
}

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy && !Enemy->bHitOnce)
		{
			Enemy->bHitOnce = true;

			if (Enemy->HitParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, Enemy->GetMesh()->GetSocketLocation("spine_02"), FRotator(0.f), false);
			}
			if (Enemy->HitSound)	// ������ �� Enemy���� ���� �Ҹ�
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}
			if (DamageTypeClass)
			{
				UAnimInstance* AnimInstance = WeaponOwner->GetMesh()->GetAnimInstance();
				if (AnimInstance) 
				{
					FName SectionName = AnimInstance->Montage_GetCurrentSection();
					
					FHitResult HitResult(ForceInit);
					HitResult.Location = GetActorLocation();

					if (SectionName == "Combo01")
					{
						UGameplayStatics::ApplyPointDamage(Enemy, Damage, GetActorLocation(), HitResult, WeaponInstigator, this, WeaponOwner->Basic);
					}
					else if (SectionName == "Combo02")
					{
						UGameplayStatics::ApplyPointDamage(Enemy, Damage, GetActorLocation(), HitResult, WeaponInstigator, this, WeaponOwner->KnockDown);
					}
					else if (SectionName == "DashAttack")
					{
						UGameplayStatics::ApplyPointDamage(Enemy, Damage, GetActorLocation(), HitResult, WeaponInstigator, this, WeaponOwner->Rush);
					}
					else if (SectionName == "UpperAttack")
					{
						UGameplayStatics::ApplyPointDamage(Enemy, Damage, GetActorLocation(), HitResult, WeaponInstigator, this, WeaponOwner->Upper);
					}
					else 
					{
						UGameplayStatics::ApplyPointDamage(Enemy, Damage, GetActorLocation(), HitResult, WeaponInstigator, this, WeaponOwner->Basic);
					}

					//Enemy->DisplayHealthBar();
					WeaponOwner->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(WeaponOwner->CamShake);

					Enemy->ResetHitOnce();
				}
			}
			
		}
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AWeapon::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // QueryOnly : ������ ��� ���� ����
}

void AWeapon::DeActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}