// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack3.h"
#include "EnemyAIController.h"
#include "BossSevarog.h"

UBTTask_Attack3::UBTTask_Attack3()
{
    bNotifyTick = true; //ƽ ��� Ȱ��ȭ
    IsAttacking = false;

}

EBTNodeResult::Type UBTTask_Attack3::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    auto BossSevarog = Cast<ABossSevarog>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == BossSevarog) return EBTNodeResult::Failed;

    BossSevarog->ThunderstrokeAttack();
    IsAttacking = true;

    BossSevarog->OnAttackEnd.AddLambda([this]() ->void {IsAttacking = false; });

    return EBTNodeResult::InProgress;      // ���� �ִϸ��̼��� ���� ������ ����ؾ� �ϴ� ���� �½�ũ�̹Ƿ� ��� ���� InProgress�� ��ȯ�ϰ�, ������ ������ �� �½�ũ�� �����ٰ� �˷���� �Ѵ�. �̸� �˷��ִ� �Լ��� FinistLatentTask �̴�.
}


void UBTTask_Attack3::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
    if (!IsAttacking)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);   // �� �Լ��� ȣ������ ������ �ൿƮ�� �ý����� ���� �½�ũ�� ��� �ӹ��� �ִ´�. �׷��� ȣ�� �Ҽ� �ֵ��� ����� Tick ����� Ȱ��ȭ�ϰ� ������ �ľ��� �� �½�ũ ���� ����� ������� �Ѵ�
    }
}
