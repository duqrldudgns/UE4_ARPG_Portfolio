// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"

// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);		//����ġ�� ���� �����̱� ������ root�� �ڽ��� �Ǹ� �����ϹǷ� Ʈ���Źڽ��� root

	//�浹 ���� ����
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //QueryOnly : ������ ��� ���� ����
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);	// ���Static ��ҿ� ���� �浹
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);	// �� �浹�� ���� ������ �����ϰ�
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);	// Pawn�� ���� �浹�� Overlap���� ����

	TriggerBox->SetBoxExtent(FVector(62.f, 62.f, 32.f));

	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));
	FloorSwitch->SetupAttachment(GetRootComponent());
	
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(GetRootComponent());

	SwitchTime = 2.f;
	bCharacterOnSwitch = false;

}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();
	
	//������ ��� Beginplay�� �δ� ���� : ��ü(������)�� �����ǰ� ��� ���� �Ϸ�ǰ� ��� �غ� �Ϸ�Ǿ��� �� �߻��ϵ��� �ϴ� Ȯ���� ���
	//��ġ�ų� ������ �̺�Ʈ �߻�(�ش� �Լ� ȣ��)
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin);	//�Լ��κ��� ���� �Ű������� ���޵�
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);

	InitialDoorLocation = Door->GetComponentLocation();
	InitialFloorSwitchLocation = FloorSwitch->GetComponentLocation();

}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin."));
	if (!bCharacterOnSwitch) bCharacterOnSwitch = true;
	RaiseDoor();
	LowerFloorSwitch();
}

void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap End."));
	if (bCharacterOnSwitch) bCharacterOnSwitch = false;
	GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFloorSwitch::CloseDoor, SwitchTime);
}

void AFloorSwitch::UpdateDoorLocation(float Z)
{
	FVector NewLocation = InitialDoorLocation;
	NewLocation.Z += Z;
	Door->SetWorldLocation(NewLocation);
}

void AFloorSwitch::UpdateFloorSwitchLocation(float Z)
{
	FVector NewLocation = InitialFloorSwitchLocation;
	NewLocation.Z += Z;
	FloorSwitch->SetWorldLocation(NewLocation);
}

void AFloorSwitch::CloseDoor()
{
	if (!bCharacterOnSwitch)
	{
		LowerDoor();
		RaiseFloorSwitch();
	}
}