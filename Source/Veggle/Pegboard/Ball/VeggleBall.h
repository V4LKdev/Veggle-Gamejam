// Copyright (c) 2025 Nicolas Martin and Vladimir Eck. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "VeggleBallData.h"
#include "Veggle/Pegboard/Board/VegglePegboard.h"
#include "VeggleBall.generated.h"

class UGameplayEffect;
class UVeggleVeggieAttributeSet;
class UVegglePegData;

UCLASS()
class VEGGLE_API AVeggleBall : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AVeggleBall();
	virtual void Tick(float DeltaTime) override;

	void SetBallData(UVeggleBallData* InBallData) { BallData = InBallData; }
	UVeggleBallData* GetBallData() { return BallData; }
	
	void LaunchBall(const FVector& ImpulseDirection);
	TSubclassOf<UGameplayEffect> GetAccumulationEffect() const { return AccumulationEffect; }
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VeggleBall")
	void OnAttributeChanged(const FGameplayTag& AttributeTag, float NewValue);

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VeggleBall")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UVeggleVeggieAttributeSet> AttributeSet;

	/** IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

protected:
	virtual void BeginPlay() override;

	void ApplyEffectToSelf(const AVegglePegboard* Pegboard, const TSubclassOf<UGameplayEffect>& EffectClass) const;
	UFUNCTION()
	void OnBallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION(BlueprintImplementableEvent, Category = "Veggle")
	void BP_OnBallHit(const UVegglePegData* HitPegData, bool bHitActivePeg);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VeggleBall")
	TObjectPtr<UMaterialInterface> BaseBallMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VeggleBall")
	TSubclassOf<UGameplayEffect> AccumulationEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VeggleBall")
	TSubclassOf<UGameplayEffect> BoardClearedEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VeggleBall")
	float MaxVelocitySquaredToIdle = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VeggleBall")
	float IdleTimeBeforeReset = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VeggleBall")
	float IdleDisablePegsRadius = 60.f;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VeggleBall", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BallMesh;
	UPROPERTY()
	TObjectPtr<UVeggleBallData> BallData;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> BallMaterialInstance;

	FTimerHandle IdleTimerHandle;
};
