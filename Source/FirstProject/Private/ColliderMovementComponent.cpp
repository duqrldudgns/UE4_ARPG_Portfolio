// Fill out your copyright notice in the Description page of Project Settings.


#include "ColliderMovementComponent.h"

void UColliderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) 
	{
		return;
	}

	FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f);	//�츮�� �����̰��� �ϴ� �������� �� ,  ������ ���� ��ȯ�� ���� ����(0���� ����?) . �ִ� 1�� ����

	if (!DesiredMovementThisFrame.IsNearlyZero())	// ! ���� 0�� ������ = 0��������������, ���� 0�̾ƴ϶��
	{
		FHitResult Hit;
		
		//������Ʈ�� ������Ʈ�� �ű� , desired �츮�� �� �������� �󸶳� �����̰� �������� �����ִ� ����� ũ�� , ������Ʈ���� ��Ҹ� �������� ���Ͽ� ����Ͽ� ���߰���� ����
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);		

		if (Hit.IsValidBlockingHit())	//������ �ƴٸ� ���������� �̲�����
		{
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);	//time : �浹 �� �����ð�,  normal : ǥ���� ������ ġ�� ����
			UE_LOG(LogTemp, Warning, TEXT("Valid Blocking Hit"));
		}
	}
}