// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class BEYONDPORTAL_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	APortal();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

// Component
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBoxComponent> BoxComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USceneCaptureComponent2D> CaptureComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UArrowComponent> ArrowComp;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

// Activate
protected:
	bool IsActivated=false;
	bool IsCreating=false;

	UPROPERTY(EditDefaultsOnly, Category = Portal)
	float PortalCreateSpeed=1.5f;

	float SizeAlpha=0.0f;

	FVector PortalSize;

public:
	void Activate(const bool ActiveSelf);
	FORCEINLINE bool GetIsActivated() const { return IsActivated; }

// Link
protected:
	UPROPERTY(EditDefaultsOnly, Category=Portal)
	TObjectPtr<APortal> LinkedPortal;

	bool IsLinked=false;

	UPROPERTY(EditDefaultsOnly)
	float AccelMultiplier=1.5f;

public:
	FORCEINLINE void SetIsLinked(bool NewIsLinked) { IsLinked = NewIsLinked; }
	FORCEINLINE bool GetIsLinked() const { return IsLinked; }
	FORCEINLINE APortal* GetLinkedPortal() const { return LinkedPortal; }

	void LinkPortal(APortal* NewLinkedPortal);
	void UnlinkPortal();

	FVector GetTargetLocation() const;
	FRotator GetTargetRotation() const;
	FVector GetTargetDirection() const;

// Capture
protected:
	UPROPERTY()
	TObjectPtr<class ACharacter> Player;

	UPROPERTY(EditAnywhere)
	float FOVOffset = 80.0f;

	UPROPERTY(EditAnywhere)
	float FOVDivider=300.0f;

	void UpdateCaptureCamera();

public:
	void SetCaptureRotation(FRotator NewRotation);

// Material
protected:
	UPROPERTY(EditDefaultsOnly, Category=Portal)
	TObjectPtr<class UMaterialInterface> DefaultMaterial;

	UPROPERTY(EditDefaultsOnly, Category=Portal)
	TObjectPtr<class UMaterialInterface> LinkedMaterial;
};
