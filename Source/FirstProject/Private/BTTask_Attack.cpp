// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "EnemyAIController.h"
#include "Enemy.h"

UBTTask_Attack::UBTTask_Attack()
{
    bNotifyTick = true; //ƽ ��� Ȱ��ȭ
    IsAttacking = false;

}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if (nullptr == Enemy) return EBTNodeResult::Failed;

    Enemy->Attack();
    IsAttacking = true;

    Enemy->OnAttackEnd.AddLambda([this]() ->void {IsAttacking = false; });

    return EBTNodeResult::InProgress;      // ���� �ִϸ��̼��� ���� ������ ����ؾ� �ϴ� ���� �½�ũ�̹Ƿ� ��� ���� InProgress�� ��ȯ�ϰ�, ������ ������ �� �½�ũ�� �����ٰ� �˷���� �Ѵ�. �̸� �˷��ִ� �Լ��� FinistLatentTask �̴�.
}


void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
    if (!IsAttacking)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);   // �� �Լ��� ȣ������ ������ �ൿƮ�� �ý����� ���� �½�ũ�� ��� �ӹ��� �ִ´�. �׷��� ȣ�� �Ҽ� �ֵ��� ����� Tick ����� Ȱ��ȭ�ϰ� ������ �ľ��� �� �½�ũ ���� ����� ������� �Ѵ�
    }
}