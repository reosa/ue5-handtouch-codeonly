// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HandPoseComponent.h"
#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_ApplyHandPose.generated.h"

/// <summary>
/// HandPoseを姿勢に反映するAnimationBlueprintノードのランタイム挙動
/// </summary>
USTRUCT(BlueprintInternalUseOnly)
struct FAnimNode_ApplyHandPose : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

	/// <summary>
	/// 反映する掴み姿勢
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HandPose, meta = (PinShownByDefault))
	FHandPoseContainer HandPose;

	/// <summary>
	/// 親指への適用率
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Alpha, meta = (PinShownByDefault))
	float ThumbAlpha = 1.0f;
	
	/// <summary>
	/// 人差し指への適用率
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Alpha, meta = (PinShownByDefault))
	float IndexAlpha = 1.0f;
	
	/// <summary>
	/// 中指への適用率
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Alpha, meta = (PinShownByDefault))
	float MiddleAlpha = 1.0f;
	
	/// <summary>
	/// 薬指への適用率
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Alpha, meta = (PinShownByDefault))
	float RingAlpha = 1.0f;
	
	/// <summary>
	/// 小指への適用率
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Alpha, meta = (PinShownByDefault))
	float PinkyAlpha = 1.0f;

public:
	/// <summary>
	/// アニメーション評価
	/// </summary>
	/// <param name="Output"></param>
	/// <param name="OutBoneTransforms"></param>
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	
	/// <summary>
	/// 入力のバリデーション
	/// </summary>
	/// <param name="Skeleton"></param>
	/// <param name="RequiredBones"></param>
	/// <returns></returns>
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;

private:
	/// <summary>
	/// 各指の適用率を取得
	/// </summary>
	/// <param name="HandJointPose"></param>
	/// <returns></returns>
	float GetAlpha(FHandJointPose HandJointPose) const;
};
