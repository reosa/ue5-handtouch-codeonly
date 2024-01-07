// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "RBF/RBFSolver.h"
#include "HandPoseComponent.h"
#include "RBFInterpolationBPLibrary.generated.h"

UCLASS()
class URBFInterpolationBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/// <summary>
	/// RBF補間の検証サンプルメソッド（RGBカラー値を返却）
	/// </summary>
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute RBF Interpolation", Keywords = "RBF Interpolation"), Category = "RBF")
	FColor RBFInterpolationSampleFunction(const FRBFParams& Params, const TArray<FTransform>& TransformArray, const TArray<FColor>& Inputs, const FTransform& SelfTransform);

	/// <summary>
	/// 手の掴み姿勢をRBF補間で計算
	/// </summary>
	/// <param name="Result"></param>
	/// <param name="Params"></param>
	/// <param name="PlotTransforms"></param>
	/// <param name="PlotHandPoses"></param>
	/// <param name="CurrentTransform"></param>
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "RBF Interpolation For Hand Pose", Keywords = "Hand Pose Interpolation"), Category = "RBF")
	static void InterpolateHandPose(FHandPoseContainer& Result, const FRBFParams& Params, const TArray<FTransform>& PlotTransforms, const TArray<FHandPoseContainer>& PlotHandPoses, const FTransform& CurrentTransform);
};
