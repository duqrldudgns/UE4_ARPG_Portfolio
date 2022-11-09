// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Main.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();	// ���� �����ڸ� ����
		if (Pawn)
		{
			Main = Cast<AMain>(Pawn);	//Main�� Pawn�� ���� ���� , Pawn�� Main�� ĳ����, �������Ʈ���� main�� ������ �ֵ���
		}
	}
}

void UMainAnimInstance::UpdateAnimationProperties()	//�������Ʈ�� ����
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Main = Cast<AMain>(Pawn);
		}
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();	// ĳ������ ���ǵ带 ���
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);	//�츮�� �����ִ°� �յ��¿��� �ӵ��̱� ������ ���Ʒ��� �� Ⱦ���⺤�͸� ����
		MovementSpeed = LateralSpeed.Size();	// .Size()�� float�ӵ��� ����

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
	}
	
}