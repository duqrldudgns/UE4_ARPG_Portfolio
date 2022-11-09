// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Cpt_FootIK.generated.h"

typedef struct ST_IK_TraceInfo
{
	float	fOffset;			
	FVector pImpactLocation;	
};

//! IK Anim Instance Value struct
USTRUCT(Atomic, BlueprintType)
struct FST_IK_AnimValue
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float		m_fEffectorLocation_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float		m_fEffectorLocation_Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator	m_pFootRotation_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator	m_pFootRotation_Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float		m_fHipOffset;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRSTPROJECT_API UCpt_FootIK : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCpt_FootIK();

	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//! Leg joing move speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values")
	float m_fIKFeetInterpSpeed;
	
	//! Max Linetrace distance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values")
	float m_fIKTraceDistance;

	//! Left foot bone name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values Socket")
	FName m_sIKSocketName_LeftFoot;
	//! Right foot bone name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values Socket")
	FName m_sIKSocketName_RightFoot;

	//! IK Anim Instance Value struct
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK Values")
	FST_IK_AnimValue m_stIKAnimValuse;

	FORCEINLINE FST_IK_AnimValue GetIKAnimValue() { return m_stIKAnimValuse; }

private:

	// IK�� �ϱ� ���ؼ��� �켱 �� ������ LineTrace�� �Ͽ� ����ġ, Normal Vector�� �˾Ƴ��� �Լ�
	ST_IK_TraceInfo IK_FootTrace(float fTraceDistance, FName sSocket);
	
	UPROPERTY()
	class AMain* m_pCharacter;

	// Owner Characters capsule height
	float m_fIKCapsuleHalkHeight;

	//! Get ground normal,  IK_FootTrace�Լ����� ���� ����ġ �� �������� �ٴ� Normal Vector�� �� �� �ִ�.
	FRotator NormalToRotator(FVector pVector);

	//! Process IK Foot rotation from ground normal,  NormalToRotator���� ���� ���� RInterpTo�Լ��� �̿��� ���� �����ϸ� ���ϰ� �ϴ� �Լ��̴�.
	void IK_Update_FootRotation(float fDeltaTime, FRotator pTargetValue, FRotator* pFootRotatorValue, float fInterpSpeed);

	//! Process IK characters capsule component Z Offset,  ���� ���̿� ���� Capsule Collider ��ġ�� ����
	void IK_Update_CapsuleHalfHeight(float fDeltaTime, float fHipsShifts, bool bResetDefault);

	//! process IK Foot,leg Z Offset,    Vector���� �����Ѵ�.  �� �Լ��� ���ؼ� �� ��ġ, Hip��ġ�� �����Ѵ�.
	void IK_Update_FootOffset(float fDeltaTime, float fTargetValue, float* fEffectorValue, float fInterpSpeed);

	//! Check owner character is moving
	bool IsMoving();
};

