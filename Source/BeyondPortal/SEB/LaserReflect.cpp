// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/LaserReflect.h"

#include "Laser.h"
#include "LaserSpawn.h"
#include "Kismet/KismetMathLibrary.h"

ALaserReflect::ALaserReflect()
{

}

void ALaserReflect::BeginPlay()
{
	Super::BeginPlay();
}

ALaser* ALaserReflect::LaserBounce(ALaser* P_LaserHit, bool P_HitResponse, FHitResult P_HitInformation)
{
	if(nullptr == P_LaserHit)
	{
		return nullptr;
	}
	if(P_HitResponse )
	{
		
		ALaser* spawnlaser = GetWorld()->SpawnActor<ALaser>(ALaser::StaticClass(), P_HitInformation.ImpactPoint, UKismetMathLibrary::MakeRotFromZ(UKismetMathLibrary::MirrorVectorByNormal(P_HitInformation.ImpactPoint - P_HitInformation.TraceStart, P_HitInformation.ImpactNormal)));

		if(spawnlaser != nullptr )
		{
			spawnlaser->Laser_ID=P_LaserHit->UpdateId(TEXT("B"));
			spawnlaser->LaserColor=P_LaserHit->LaserColor;
			spawnlaser->SpawnReference=P_LaserHit->SpawnReference;

			LasersAttached.Add(spawnlaser);
			UE_LOG(LogTemp, Error, TEXT("spawnlaser : %s"), *spawnlaser->GetName());

			if( P_LaserHit->SpawnReference != nullptr )
				P_LaserHit->SpawnReference->ChildLasers.Add(spawnlaser);

			return spawnlaser;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn laser."));
			return nullptr;
		}
	}
	else
	{
		ClearAttachedLasers(P_LaserHit->Laser_ID);
		return nullptr;
	}
}

void ALaserReflect::ClearAttachedLasers(FString P_ID_Clear)
{
	for(int i=LasersAttached.Num()-1; i>=0; --i )
	{
		if(LasersAttached[i]!= nullptr)
		{
			if(LasersAttached[i]->Laser_ID.Contains(P_ID_Clear))
			{
				LasersAttached[i]->Destroy();
				LasersAttached.Remove(LasersAttached[i]);
			}
		}
	}
	
}
