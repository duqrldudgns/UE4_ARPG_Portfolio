#include "BTService_Detect.h"
#include "EnemyAIController.h"
#include "Main.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Enemy.h"
#include "BossSevarogAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BossSevarog.h"

UBTService_Detect::UBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 0.5f;
}

void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AEnemy* ControllingPawn = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ControllingPawn) return;
	ABossSevarog* Boss = Cast<ABossSevarog>(OwnerComp.GetAIOwner()->GetPawn());

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World) return;

	FVector Center = ControllingPawn->GetActorLocation();

	// ��ü�� ���� ������Ʈ�� �����Ѵ�.
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel4,
		FCollisionShape::MakeSphere(ControllingPawn->DetectRadius),
		CollisionQueryParam
	);
	//UE_LOG(LogTemp, Warning, TEXT("%d"), bResult);
	// ������Ʈ�� ������ �Ǹ�, �� ������Ʈ�� Main���� �˻��Ѵ�.
	if (bResult)
	{
		for (FOverlapResult OverlapResult : OverlapResults)
		{
			//DrawDebugLine(World, ControllingPawn->GetActorLocation(), OverlapResult.GetActor()->GetActorLocation(), FColor::Red, false, 1.f);

			AMain* Main = Cast<AMain>(OverlapResult.GetActor());
			if (Main/* && Main->GetController()->IsPlayerController()*/)	//�÷��̾� ��Ʈ�ѷ����� �ľ�
			{
				FVector TargetLoc = Main->GetFloor();
				//UE_LOG(LogTemp, Warning, TEXT("%s"), *TargetLoc.ToString());
				// Main��, �����忡 �����Ѵ�.
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(AEnemyAIController::TargetKey, Main);
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(ABossSevarogAIController::TargetLocKey, TargetLoc);
				if (nullptr == Boss && !ControllingPawn->bSlowDebuff) ControllingPawn->GetCharacterMovement()->MaxWalkSpeed = 400.f;

				// ����� ��.
				//DrawDebugSphere(World, Center, ControllingPawn->DetectRadius, 16, FColor::Green, false, 0.2f);
				//DrawDebugPoint(World, Main->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				//DrawDebugLine(World, ControllingPawn->GetActorLocation(), Main->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(AEnemyAIController::TargetKey, nullptr);
		if (nullptr == Boss) ControllingPawn->GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}

	//DrawDebugSphere(World, Center, ControllingPawn->DetectRadius, 16, FColor::Red, false, 0.2f);
	/*
	1. Selector�� �ڽ� �� �ϳ��� ������ ������, ���� ��忡�� ������ ���� �����Ѵ�.
	2. ���� ������ ����. Target�� �����Ƿ� ����.
	3. ������ ������ ����. ������ ����.
	4. ������ ������ ���� ��尡 ���̳���, �ٽ� 1������ �����Ѵ�.
	5. ���� ������ ����. Target�� �����Ƿ� ����. ���� ��带 �����Ѵ�. 
	*/
}
