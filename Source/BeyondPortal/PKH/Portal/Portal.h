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

public:
	void Activate(const bool ActiveSelf);
	FORCEINLINE bool GetIsActivated() const { return IsActivated; }

// Link
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<APortal> LinkedPortal;

	bool IsLinked=false;

public:
	FORCEINLINE void SetIsLinked(bool NewIsLinked) { IsLinked = NewIsLinked; }
	FORCEINLINE bool GetIsLinked() const { return IsLinked; }
	FORCEINLINE APortal* GetLinkedPortal() const { return LinkedPortal; }

	void LinkPortal(APortal* NewLinkedPortal);

	FVector GetTargetLocation() const;
	FRotator GetTargetRotation() const;
	FVector GetTargetDirection() const;

// Capture
protected:
	FVector CaptureLocation;
	FRotator CaptureRotation;

	FORCEINLINE FVector GetCaptureLocation() const { return CaptureLocation; }
	FORCEINLINE FRotator GetCaptureRotation() const { return CaptureRotation; }

public:
	void ResetCaptureCamera();

// Material
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UMaterialInterface> DefaultMaterial;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UMaterialInterface> LinkedMaterial;
};
