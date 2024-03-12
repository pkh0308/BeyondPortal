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

public:
	FORCEINLINE void SetIsLinked(bool NewIsLinked) { IsLinked = NewIsLinked; }
	FORCEINLINE bool GetIsLinked() const { return IsLinked; }
	FORCEINLINE APortal* GetLinkedPortal() const { return LinkedPortal; }

	void LinkPortal(APortal* NewLinkedPortal);
	void UnlinkPortal();

	FVector GetTargetLocation() const;
	FRotator GetTargetRotation() const;
	FVector GetTargetDirection() const;

// Exception handle
protected:
	bool IsInDelay=false;

	FTimerHandle DelayHandle;

// Capture
protected:
	UPROPERTY()
	TObjectPtr<class APlayerCharacter> Player;

	UPROPERTY(EditAnywhere)
	float FOVOffset = 80.0f;

	UPROPERTY(EditAnywhere)
	float FOVDivider=300.0f;

	void UpdateCaptureCamera();

	void SetCaptureFOV();
	void SetCaptureRotation();

public:
	FORCEINLINE void SetCapturePlayer(class APlayerCharacter* NewPlayer) { Player=NewPlayer; };

	void SetCaptureRotation(FRotator NewRotation);
	void SetCaptureRotation(FMatrix NewMatrix);

// Material
protected:
	UPROPERTY(EditDefaultsOnly, Category=Portal)
	TObjectPtr<class UMaterialInterface> DefaultMaterial;

	UPROPERTY(EditDefaultsOnly, Category=Portal)
	TObjectPtr<class UMaterialInterface> LinkedMaterial;

// Network
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Portal Transform
	UPROPERTY(ReplicatedUsing=OnRep_PortalTransformChanged)
	FTransform Net_PortalTransform;

	UFUNCTION()
	void OnRep_PortalTransformChanged();

	// Portal Scale
	UPROPERTY(ReplicatedUsing=OnRep_PortalScaleChanged)
	FVector Net_PortalScale;

	UFUNCTION()
	void OnRep_PortalScaleChanged();

// RPC
	UFUNCTION(Server, Unreliable)
	void RPC_PortalTransformChanged();

	UFUNCTION(Server, Unreliable)
	void RPC_Server_MaterialChange(class UMaterialInterface* NewMaterial);

	UFUNCTION(NetMulticast, Unreliable)
	void RPC_Multi_MaterialChange(class UMaterialInterface* NewMaterial);
};
