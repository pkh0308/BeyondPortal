// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/Laser.h"

#include "Lasers.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void ALaser::BeginPlay()
{
	Super::BeginPlay();
	ChangeColor();

}

void ALaser::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SetEndPoint();
}

void ALaser::ChangeColor()
{
	/*UMaterial* mat =  UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(),laserColor);
	SM_Parent->SetMaterial(0, mat);
	SM_Parent->CreateDynamicMaterialInstance(0, laserColor, TEXT("None"));*/
}

void ALaser::SetEndPoint()
{

	FVector start=this->GetActorLocation() + this->GetActorUpVector() * 2.0f;
	FVector end = start + this->GetActorUpVector() * L_Range;

	UKismetSystemLibrary::BoxTraceSingle(GetWorld(), start, end, FVector(0.5f), FRotator(0), UEngineTypes::ConvertToTraceType(ECC_Visibility), false, TArray<AActor*>(), EDrawDebugTrace::None, L_HitInfo, true);

	SM_Parent->SetWorldScale3D(FVector(0.05, 0.05, UKismetMathLibrary::SelectFloat((L_HitInfo.Distance + 1) / 100, L_Range / 100, L_HitInfo.bBlockingHit)));

	//L_HitActor=L_HitInfo.GetActor();
	L_HitActor=Cast<AActor>(L_HitInfo.GetActor());

	
	

	if ( LastHitActor != L_HitActor )
	{
		
		if ( LastHitActor != nullptr &&  UKismetSystemLibrary::DoesImplementInterface(LastHitActor, laserInterface) )
		{
			
			Cast<ILasers>(LastHitActor)->LaserBounce(this, true, L_HitInfo);

		}
	}
	else
	{

		
		if ( L_HitActor != nullptr && UKismetSystemLibrary::DoesImplementInterface(L_HitActor, laserInterface) )
		{

			laserI=Cast<ILasers>(L_HitActor);
			laserI->LaserBounce(this, true, L_HitInfo);
		}
	}

	if ( L_HitActor != nullptr )
	{
		LastHitActor=L_HitActor;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("없다면"));
	}

}

FString ALaser::UpdateId(FString P_Append)
{
	return Laser_ID.Append(P_Append);
}
