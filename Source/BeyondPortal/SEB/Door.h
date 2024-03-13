// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class BEYONDPORTAL_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//충돌처리
	UFUNCTION()
	void OnMyCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	
	UFUNCTION()
	void OnMyCompEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
public:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxComp;
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* door;

	UPROPERTY(Replicated)
	int32 cnt=0;

	bool isOpened=false;

	UPROPERTY()
	class APortalButton* portalButton;

	UPROPERTY(Replicated)
	UAnimationAsset* openDoorAnim;

	UFUNCTION(BlueprintCallable)
	void OpenDoor();

	//NetWork

	UFUNCTION(Server, Reliable)
	void RPC_Server_OpenDoor();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_OpenDoor();


	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
