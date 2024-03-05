// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PKH/Interface/CanGrab.h"
#include "GrabCube.generated.h"

UCLASS()
class BEYONDPORTAL_API AGrabCube : public AActor, public ICanGrab
{
	GENERATED_BODY()
	
public:	
	AGrabCube();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

// Component
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBoxComponent> BoxComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UStaticMeshComponent> MeshComp;

// Interface
public:
	virtual void Grab(ACharacter* NewOwner) override;

	virtual void Drop() override;

// Grab
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class APlayerCharacter> OwnPlayer;

	UPROPERTY(EditDefaultsOnly)
	float DropVelocity = 250.0f;

	UPROPERTY(EditDefaultsOnly)
	float VelocityCut = 0.5f;

	void TickGrab();

// Success
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture2D> DefaultTexture;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture2D> SuccessTexture;

public:
	void ChangeMaterial(bool Success);

// Dissolve
protected:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<class UMaterialInstanceDynamic>> DMArray;

	void InitDynamicMaterials();

public:
	void OnDisappear();

protected:
	UPROPERTY(EditDefaultsOnly)
	float DissolveSeconds=4.0f;

	float DissolveCount=0.15f;

	bool IsDissolving=false;

	void TickDisappear(float DeltaSeconds);

// Network
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_CubeLocationChanged)
	FVector Net_CubeLocation;

	UPROPERTY(ReplicatedUsing = OnRep_CubeRotationChanged)
	FRotator Net_CubeRotation;

	UPROPERTY(ReplicatedUsing = OnRep_OwnPlayerChanged)
	TObjectPtr<class APlayerCharacter> Net_OwnPlayer;

	UFUNCTION()
	void OnRep_CubeLocationChanged();

	UFUNCTION()
	void OnRep_CubeRotationChanged();

	UFUNCTION()
	void OnRep_OwnPlayerChanged();

	// RPC
	UFUNCTION(Server, Unreliable)
	void RPC_SetCubeOwner(class APlayerCharacter* NewOwner);
};
