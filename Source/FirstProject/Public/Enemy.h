// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking		UMETA(DisplayName = "Attacking"),
	EMS_Dead			UMETA(DisplayName = "Dead"),

	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

	/** �÷��̾��� ������ ����Ǹ� ���� �½�ũ���� �ش� �˸��� ���� �� �ֵ��� ��������Ʈ�� ���� ���� */
	DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

public:
	// Sets default values for this character's properties
	AEnemy();

	/** ������ ����� �� ��������Ʈ�� ȣ�� */
	FOnAttackEndDelegate OnAttackEnd;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetEnemyMovementStatus(EEnemyMovementStatus Status);
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Decal")
	class UDecalComponent* SelectDecal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetComponent* HealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sword", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* SwordAttached;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class AAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USoundCue* SwingSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	UAnimMontage* DamagedMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	UAnimMontage* LightDamagedMontage;

	// Enemy���� Ÿ�� �� �����Ϸ��� ��ƼŬ ����(�� ���� ��)�� �ٸ��� ������ Enemy�� ��ƼŬ�ý����� �ް�����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UParticleSystem* HitParticles;

	FTimerHandle AttackTimer;

	
	/** �ջ� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMain* Target);

	/** �������Ʈ �귱ġ�� ��� */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	bool bOverlappingCombatSphere;	

	/** Anim_BP���� Target(Main)�� ���� ������ ���� */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AMain* CombatTarget;	
	
	/** ����� ���� ���ο� ���� ���� ���� ���� */
	bool bHasValidTarget;

	FTimerHandle TimerHandle;
	float TimerRemaining;

	int32 DamType;
	UFUNCTION(BlueprintCallable)
	void UpperActivate();
	UFUNCTION(BlueprintCallable)
	void KnockDownActivate();
	UFUNCTION(BlueprintCallable)
	void RushActivate();
	UFUNCTION(BlueprintCallable)
	void ActivateCollision();
	UFUNCTION(BlueprintCallable)
	void DeActivateCollision();

	/** Attack */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();


	/** ApplyDamage �Լ��� �����Ǿ� �������� ������ ���� */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	bool Alive();

	void Die();

	bool DecrementHealth(float Amount);

	void Disappear();
	
	UFUNCTION(BlueprintCallable)
	void AirHitEnd();

	UFUNCTION(BlueprintCallable)
	void DamagedDownEnd();

	UFUNCTION(BlueprintCallable)
	void DamagedEnd();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bDamagedIng;

	
	float DetectRadius;

	void PlayMontage(UAnimMontage* MontageName);

	class UAnimInstance* AnimInstance;

	
	FTimerHandle DisplayHealthBarHandle;
	float DisplayHealthBarTime;
	
	void DisplayHealthBar();
	void RemoveHealthBar();

	/** �ѹ� ���� �� �ߺ��ؼ� �ȸµ��� */
	bool bHitOnce;

	void ResetHitOnce();

	void ResetCasting();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bCanAttack;

	void AttackDelay();
	float AttDelay;

	void StopAnim();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bFreezing;

	bool bDie;

	bool bSlowDebuff;
	bool bSlowDebuffOnce;

	
};
