// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	//EMS : E Movement Status
	EMS_Normal				UMETA(DisplayName = "Normal"),		//�⺻ �޸���
	EMS_Sprinting			UMETA(DisplayName = "Sprinting"),	//��������
	EMS_Guard				UMETA(DisplayName = "Guard"),		//����
	EMS_Dead				UMETA(DisplayName = "Dead"),

	EMS_MAX					UMETA(DisplayName = "DefaultMAX")	//����ϱ� ���� ���� �ƴ����� �̸� �����ϰ� ��Ȯ�� �ִ��� ��Ÿ��
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	//ESS : E Stamina Status
	ESS_Normal				UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum		UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted			UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering	UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX					UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EArmedStatus : uint8
{
	//EAS : E Armed Status
	EAS_Normal				UMETA(DisplayName = "Normal"),
	EAS_Sword				UMETA(DisplayName = "Sword"),
	EAS_Bow					UMETA(DisplayName = "Bow"),

	ESS_MAX					UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();


	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))	// �ڵ�󿡼��� private������ ������ �󿡼��� ���� ����
	class USpringArmComponent* CameraBoom;

	/** Follow Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCameraZoomIn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bow")
	class UParticleSystemComponent* PShoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sword", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* SwordAttached;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bow", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* BowAttached;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bow")
	class UStaticMeshComponent* Quiver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bow")
	class UInstancedStaticMeshComponent* QuiverArrows;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bow")
	class USceneComponent* ArrowsVisualize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Decal")
	class UDecalComponent* SelectDecal;


	/** Armed Status*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EArmedStatus ArmedStatus;

	FORCEINLINE void SetArmedStatus(EArmedStatus Status) { ArmedStatus = Status; }
	FORCEINLINE EArmedStatus GetArmedStatus() { return ArmedStatus; }

	/** Stamina Status */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MinSprintStamina;


	/**
	* ����(Interpolation) - �� ���� �����ϴ� ���
	* ���� ���� ���ݴ������ ������ �� �� ����� ���ϵ��� ���� */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Combat")
	AEnemy* BossTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	void UpdateCombatTarget();

	// AEnemy class�� ����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	// ���� ���� ���� ������� �����Ǿ� �ִٸ� �� ���� HP�ٰ� ����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;


	/** Set movement status and running speed */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;
	
	void SetMovementStatus(EMovementStatus Status);
	FORCEINLINE EMovementStatus GetMovementStatus() { return MovementStatus; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float WalkingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;

	/** Pressed down to enable sprinting */
	void ShiftKeyDown();

	/** Released to stop sprinting */
	void ShiftKeyUp();

	virtual void Jump() override;
		
	virtual void StopJumping() override;

	/** Base turn rates to scale turning functions for the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	
	void AddInstanceQuiverArrows();


	/** Bow Init Setting */
	void BowInitSet();

	/** Quiver and arrows logic */
	void SaveArrowTransforms();

	/** Bow Equip & UnEquip */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* BowEquipMontage;

	UFUNCTION(BlueprintCallable)
	void EquipBow();
	UFUNCTION(BlueprintCallable)
	void UnEquipBow();

	bool bBowEquipKeyDown;
	void BowEquipKeyDown();
	void BowEquipKeyUp();

	/** Remove arrow from quiver */
	UFUNCTION(BlueprintCallable)
	void RemoveArrowMeshQuiver(int32 NewArrows);

	void SubtractArrows(int32 ArrowsToSubtract);

	/** Add arrow from quiver */
	UFUNCTION(BlueprintCallable)
	void AddArrowMeshQuiver(int32 NewArrows);

	UFUNCTION(BlueprintCallable)
	void AddArrows(int32 ArrowsToAdd);

	/** Aim */
	void BowAimInputPressed();
	void BowAimInputReleased();

	void CheckWantsToAim();

	void ForceAimStart();
	void ForceAimStop();

	void StartAim();
	void StopAim();

	/** Follow Camera Location Change */
	void ChangeFollowCameraZoomOut();
	void ChangeFollowCameraZoomIn();
	void ChangeFollowCamera();
	bool bCameraZoomOut;
	bool bCameraZoomIn;
	void CameraChangeInterp();

	FVector FollowCameraInitLoc;


	/** Draw */
	void BowDrawPressed();
	void BowDrawReleased();
	
	void CheckWantsToDraw();
	void CheckDrawStart();

	void PlayFireMontage();

	UFUNCTION(BlueprintCallable)
	void FireArrowEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* ArrowFireMontage;

	/** Bow */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float DefaultFieldOfView;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float DefaultArmLength;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float TraceRadius;	//50.f
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	AActor* AimAssistTarget;	//BPFocusPoint
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float DistanceToAimAssistTarget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float MaxAimAssistDistance;	//2000,f
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float MaxTraceRadius;	//100.f
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	FRotator StartRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool ActiveAimAssist;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float AimAssistPlayRate;	//1.f
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	class ABow* BowReference;	//BPBow

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	int32 ArrowMeshesInquiver;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool BowEquipped;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool WantsToAim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	int32 Arrows;	//99999
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool Aiming;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	int32 SubtractNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool Drawing;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool WantsToDraw;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool CanDraw;	// true
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool GrabArrowFromQuiver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	TArray<FTransform> ArrowTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	int32 MaxArrows;	//100
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float Draw;


	/** Foot IK */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	class UCpt_FootIK* FootIK;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	FRotator FootRotationL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	FRotator FootRotationR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float HipOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float FootOffsetL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float FootOffsetR;
	

	/**
	/*
	/* Player Stats
	/*
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 Coins;

	/** ApplyDamage �Լ��� �����Ǿ� �������� ������ ���� */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);
	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	void DecrementHealth(float Amount);
	void DecrementHealth(float Amount, AActor* DamageCauser);

	bool Alive();

	void Die();


	// �� ĳ���ʹ� ���� �� �� �Ǹ� ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;
			
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for Yaw rotation */
	void Turn(float Value);

	/** Called for Pitch rotation */
	void LookUp(float Value);

	bool bMovingForward;
	bool bMovingRight;

	bool CanMove(float Value);

	bool CanAction();

	/** Called for forwards/backwards input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called via input to turn at a given rate
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/** Called via input to look up/down at a given rate
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	*/
	void LookUpAtRate(float Rate);

	bool bLMBDown;
	void LMBDown();
	void LMBUp();

	bool bRMBDown;
	void RMBDown();
	void RMBUp();

	bool bEscDown;
	void EscDown();
	UFUNCTION(BlueprintCallable)
	void EscUp();

	bool bFirstSkillKeyDown;
	void FirstSkillKeyDown();
	void FirstSkillKeyUp();

	bool bSecondSkillKeyDown;
	void SecondSkillKeyDown();
	void SecondSkillKeyUp();

	bool bThirdSkillKeyDown;
	void ThirdSkillKeyDown();
	void ThirdSkillKeyUp();

	bool bFourthSkillKeyDown;
	void FourthSkillKeyDown();
	void FourthSkillKeyUp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float FirstSkillCoolDownTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float FirstSkillCoolDownUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool FirstSkillCoolDown;
	void StartFirstSkillCooldown();
	void FirstSkillCoolDownReturn();
	bool FirstSkillCoolDownDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float SecondSkillCoolDownTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float SecondSkillCoolDownUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool SecondSkillCoolDown;
	void StartSecondSkillCooldown();
	void SecondSkillCoolDownReturn();
	bool SecondSkillCoolDownDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float ThirdSkillCoolDownTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float ThirdSkillCoolDownUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool ThirdSkillCoolDown;
	void StartThirdSkillCooldown();
	void ThirdSkillCoolDownReturn();
	bool ThirdSkillCoolDownDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float FourthSkillCoolDownTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	float FourthSkillCoolDownUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bow")
	bool FourthSkillCoolDown;
	void StartFourthSkillCooldown();
	void FourthSkillCoolDownReturn();
	bool FourthSkillCoolDownDelay;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Test")
	bool bTestKeyDown;
	void TestKeyDown();
	void TestKeyUp();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }


	/** Equip pick up*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AItem* ActiveOverlappingItem;		// AItem���� ������ ������ �������� ������ �����̵� �Ҹ�ǰ�� �� �ֱ� ������ ��� �����ϱ� ����

	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }
	
	bool bSwordEquipKeyDown;
	void SwordEquipKeyDown();
	void SwordEquipKeyUp();


	/** Equip - Weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon;

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* WeaponEquipMontage;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Anims")
	bool bArmedBridgeIng;

	UFUNCTION(BlueprintCallable)
	void Equip();

	UFUNCTION(BlueprintCallable)
	void UnEquip();
	
	UFUNCTION(BlueprintCallable)
	void ArmedBridgeEnd();


	/** Attack */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	int ComboCnt;
	bool bComboAttackInput;

	UFUNCTION(BlueprintCallable)
	void ComboAttackInputCheck();

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void Resurrection();


	/** Jump Combat */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* JumpCombatMontage;

	void AirAttack();


	/** Defense */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* DamagedMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* GuardAcceptSound;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	bool bGuardAccept;

	FORCEINLINE void SetGuardAccept(bool Accept) { bGuardAccept = Accept; }
	FORCEINLINE bool GetGuardAccept() { return bGuardAccept; }

	UFUNCTION(BlueprintCallable)
	void GuardAcceptEnd();

	/** Save & Load Data*/
	UFUNCTION(BlueprintCallable)
	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();
	
	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	/** ������ ������ �� �⺻������ �ε�Ǵ� �� */
	void LoadGameNoSwitch();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<class AItemStorage> WeaponStorage;


	/** Skill */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Anims")
	bool bSkillCasting;

	UFUNCTION(BlueprintCallable)
	void SkillCastEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* SkillMontage;


	/** ���ظ� ���� �� �����ؾ� �ϴ� ��Ʈ�ѷ��� �ʿ� */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	AController* MainInstigator;

	/** ���ظ� ���� �� ������Ʈ�ѷ��� ������ */
	FORCEINLINE void SetInstigator(AController* Inst) { MainInstigator = Inst; }

	void PlayMontage(UAnimMontage* Montage, FName Section);

	/** FireBallSkill */
	void FireBallSkillCast();

	UFUNCTION(BlueprintCallable)
	void FireBallSkillActivation();

	UFUNCTION(BlueprintCallable)
	void PlayFireBallSkillParticle();

	UFUNCTION(BlueprintCallable)
	void PlayFireBallSkillSound();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* FireBallSkillParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* FireBallSkillSound;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AShootingSkill> FireBall;


	/** WaveSkill */
	void WaveSkillCast();

	UFUNCTION(BlueprintCallable)
	void WaveSkillActivation();

	UFUNCTION(BlueprintCallable)
	void PlayWaveSkillParticle();

	UFUNCTION(BlueprintCallable)
	void PlayWaveSkillSound();

	UFUNCTION(BlueprintCallable)
	void SkillKeyDownCheck();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* WaveSkillParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* WaveSkillSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float WaveSkillDamage;

	/** Dodge */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* DodgeMontage;

	UFUNCTION(BlueprintCallable)
	void DodgeStart();

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

	bool bDodgeIng;
	
	/** Dash Attack */
	void DashAttack();

	/** Upper Attack */
	void UpperAttack();


	/** DamageTypeClass */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> Basic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> Upper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> KnockDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> Rush;


	FTimerHandle TimerHandle;

	FVector GetFloor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	bool bGotHit;

	UFUNCTION(BlueprintCallable)
	void DamagedEnd();

	/** ������ ���� �� ĳ����(Ing)���� ��� bIng�� Ǯ�� ���� */
	UFUNCTION(BlueprintCallable)
	void ResetCasting();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	float AimPitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	float AimYaw;


	/** Camera Shake */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMatineeCameraShake> CamShake;


	/** Change Walking 8 way */
	void WalkingWayChange();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anims")
	bool bWalking8Way;

	/** Lock On Target*/
	bool bLockOnTargetKeyDown;
	void LockOnTargetKeyDown();
	void LockOnTargetKeyUp();

	void LockOnTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	AEnemy* LockOnEnemy;

	void LockOnTargetCameraInterp();

	/** Damage number generated when hitting an enemy */
	UFUNCTION(BlueprintCallable)
	void SpawnDamageNumbers(FVector InLocation, bool Headshot, float DamageNumbers);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AActor> DamageNumberAsset;

	bool bCombo1KeyDown;
	void Combo1KeyDown();
	void Combo1KeyUp();

	bool bCombo2KeyDown;
	void Combo2KeyDown();
	void Combo2KeyUp();

	/** Blackhole Skill */
	bool bBlackholeKeyDown;
	void Blackhole();
	void BlackholeSKillCast();

	class APositionDecal* BlackholeRangeDecal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Blackhole")
	TSubclassOf<APositionDecal> BlackholeRangeDecalAsset;

	void ShowBlackholeRange();
	void HoldMouseCenter();

	FHitResult MouseHitResult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Blackhole")
	FVector MousePoint;
	
	UFUNCTION(BlueprintCallable)
	void BlackholeCastEnd();
	UFUNCTION(BlueprintCallable)
	void PlayBlackholeParticle();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Blackhole")
	bool bBlackholeActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Blackhole")
	class UParticleSystem* BlackholeStartParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Blackhole")
	class USoundCue* BlackholeStartSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Blackhole")
	class UParticleSystem* BlackholeParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Blackhole")
	class USoundCue* BlackholeSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Blackhole")
	class UParticleSystem* BlackholeEndParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Blackhole")
	class USoundCue* BlackholeEndSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | Blackhole")
	TArray<FHitResult> BlackholeHitResults;

	/** Set SuperArmor */
	bool bSuperArmor;

	/** CloneAttack Skill */
	void CloneAttack();
	void CloneAttackCast();
	bool bCloneAttackKeyDown;

	FTimerHandle CloneAttackHandle;
	bool CloneAttackDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | CloneAttack")
	TSubclassOf<AMain> CloneAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | CloneAttack")
	TSubclassOf<class AWeapon> WeaponAsset;

	TArray<AMain*> Clones;
	TArray<AWeapon*> CloneWeapons;
	bool bCloneReset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | CloneAttack")
	class UParticleSystem* CloneSpawnParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | CloneAttack")
	class USoundCue* CloneSpawnSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | CloneAttack")
	class UParticleSystem* CloneRemoveParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill | CloneAttack")
	class USoundCue* CloneRemoveSound;


	/** Debug */
	TArray<FVector> PickupLocations;

	UFUNCTION(BlueprintCallable)
		void ShowPickupLocations();
};
