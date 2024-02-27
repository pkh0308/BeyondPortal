// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Components/ActorComponent.h"
#include "PlayerInputComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEYONDPORTAL_API UPlayerInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerInputComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

// Owner
protected:
	virtual void InitializeComponent() override;

	UPROPERTY()
	TObjectPtr<class APlayerCharacter> Owner;

	UPROPERTY()
	TObjectPtr<class UCharacterMovementComponent> MoveComp;

// Input
public:
	void SetupInput(class UEnhancedInputComponent* PlayerInputComponent);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UInputMappingContext> IMC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UInputAction> IA_Jump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UInputAction> IA_Crouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UInputAction> IA_FireLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UInputAction> IA_FireRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UInputAction> IA_Interaction;

	// Bind Functions
	UFUNCTION()
	void OnIAMove(const FInputActionValue& Value);

	UFUNCTION()
	void OnIALook(const FInputActionValue& Value);

	UFUNCTION()
	void OnIAJump(const FInputActionValue& Value);

	UFUNCTION()
	void OnIACrouch(const FInputActionValue& Value);

	UFUNCTION()
	void OnIAFireLeft(const FInputActionValue& Value);

	UFUNCTION()
	void OnIAFireRight(const FInputActionValue& Value);

	UFUNCTION()
	void OnIAInteraction(const FInputActionValue& Value);

	// Values
	UPROPERTY(EditAnywhere)
	float MouseSensitivity=1.0f;

	UPROPERTY(EditAnywhere)
	float InteractionDistance=200.0f;

// LineTrace
protected:
	bool TrySpawnPortal(FHitResult& InHitResult) const;
};
