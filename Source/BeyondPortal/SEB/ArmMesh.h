// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArmMesh.generated.h"

UCLASS()
class BEYONDPORTAL_API AArmMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArmMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* armMesh;

	UPROPERTY(Replicated)
	UAnimationAsset* openMeshAnim;

	void openMesh();

	//Network
	UFUNCTION(Server, Reliable)
	void RPC_Server_OpenMesh();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_OpenMesh();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
