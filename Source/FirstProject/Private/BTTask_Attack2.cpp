// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack2.h"
#include "EnemyAIController.h"
#include "BossSevarog.h"

UBTTask_Attack2::UBTTask_Attack2()
{
    bNotifyTick = true; //ƽ ��� Ȱ��ȭ
    IsAttacking = false;

}

EBTNodeResult::Type UBTTask_Attack2::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    auto BossSevarog = Cast<ABossSevarog>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == BossSevarog) return EBTNodeResult::Failed;

    BossSevarog->RushAttack();
    IsAttacking = true;

    BossSevarog->OnAttackEnd.AddLambda([this]() ->void {IsAttacking = false; });

    return EBTNodeResult::InProgress;      // ���� �ִϸ��̼��� ���� ������ ����ؾ� �ϴ� ���� �½�ũ�̹Ƿ� ��� ���� InProgress�� ��ȯ�ϰ�, ������ ������ �� �½�ũ�� �����ٰ� �˷���� �Ѵ�. �̸� �˷��ִ� �Լ��� FinistLatentTask �̴�.
}


void UBTTask_Attack2::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
    if (!IsAttacking)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);   // �� �Լ��� ȣ������ ������ �ൿƮ�� �ý����� ���� �½�ũ�� ��� �ӹ��� �ִ´�. �׷��� ȣ�� �Ҽ� �ֵ��� ����� Tick ����� Ȱ��ȭ�ϰ� ������ �ľ��� �� �½�ũ ���� ����� ������� �Ѵ�
    }
}