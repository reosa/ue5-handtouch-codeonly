// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "HandPoseComponent.generated.h"


UENUM(BlueprintType)
enum class FingerGroup : uint8
{
	None,
	Thumb,
	Index,
	Middle,
	Ring,
	Pinky,
};

/// <summary>
/// 各関節ごとの姿勢
/// </summary>
USTRUCT(BlueprintType)
struct FHandJointPose
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FName BoneName;

	UPROPERTY(EditAnywhere)
	FTransform Transform;

	UPROPERTY(EditAnywhere)
	FingerGroup FingerGroup;
};

/// <summary>
/// 掴み姿勢を統括する構造体
/// </summary>
USTRUCT(BlueprintType)
struct FHandPoseContainer
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FHandJointPose> FingerPoseArray;
};

/// <summary>
/// Actorの掴み方を定義するコンポーネント
/// </summary>
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UHandPoseComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

	static constexpr const char* C_THUMB = "thumb";
	static constexpr const char* C_INDEX = "index";
	static constexpr const char* C_MIDDLE = "middle";
	static constexpr const char* C_RING = "ring";
	static constexpr const char* C_PINKY = "pinky";
	
	/// <summary>
	/// 掴み姿勢を取得（Blueprintから呼出し可能）
	/// </summary>
	/// <param name="HandPoseContainer">掴み姿勢</param>
	/// <param name="FingerBoneNames">必要なボーン名一覧</param>
	/// <param name="Time">Animationアセットから取得したい時間</param>
	UFUNCTION(BlueprintCallable, Category = "RBF")
	void GetHandPose(FHandPoseContainer& HandPoseContainer, const TArray<FName> FingerBoneNames, const float Time = 0.0f) const;

private:
	/// <summary>
	/// ボーン名を元にグループ値を分類
	/// </summary>
	/// <param name="BoneName"></param>
	/// <returns></returns>
	static FingerGroup ClassifyFingerGroup(FName BoneName);
};
