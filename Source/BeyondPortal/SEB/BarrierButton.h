// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BarrierButton.generated.h"

UCLASS()
class BEYONDPORTAL_API ABarrierButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABarrierButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//충돌처리
	UFUNCTION()
	void OnMyCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnMyCompEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxComp;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* barrierButton;

	UPROPERTY(Replicated)
	int32 cnt=0;


	UFUNCTION(Server, Reliable)
	void RPC_Server_ActiveButton();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_ActiveButton();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<AActor> spawnCube;
	void Spawn();

	UFUNCTION(Server, Reliable)
	void RPC_Server_Spawn();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
