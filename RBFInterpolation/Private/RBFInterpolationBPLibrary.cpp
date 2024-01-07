// Copyright Epic Games, Inc. All Rights Reserved.

#include "RBFInterpolationBPLibrary.h"
#include "RBF/RBFSolver.h"
#include "RBFInterpolation.h"

URBFInterpolationBPLibrary::URBFInterpolationBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

FColor URBFInterpolationBPLibrary::RBFInterpolationSampleFunction(
	const FRBFParams& Params,
	const TArray<FTransform>& Transforms,
	const TArray<FColor>& Inputs,
	const FTransform& SelfTransform)
{
	if (Transforms.IsEmpty() ||
		Inputs.IsEmpty() ||
		Transforms.Num() != Inputs.Num())
	{
		return FColor();
	}

	// 散布データを専用クラスに変換
	TArray<FRBFTarget> Targets;
	for (const auto& Transform : Transforms)
	{
		FRBFTarget Target;
		Target.AddFromVector(Transform.GetLocation());
		Targets.Add(Target);
	}

	// 1: RBFソルバーの初期化
	// 補間したいパラメータが複数ある場合に逆行列を使い回せるため
	// この工程では"y = Φω"でいうところのΦの逆行列の算出までしか行わない
	// そのためωベクトルそのものは算出されないので注意
	FRBFSolver RBFSolver;
	auto SolverData = RBFSolver.InitSolver(Params, Targets);
	
	// 2: 新しい入力に対するWeight値を計算
	// ※ Solve()の内部実装コードの都合の都合上、このブループリントノードは"Tick"で呼び出す必要がある
	//    "Construction Script"や"Begin Play"でノードを呼び出すとクラッシュする
	//    業務利用するならRBFSolver.cppの410行目のメモリアロケーション指定を改造する必要がありそう
	FRBFTarget Input;
	Input.AddFromVector(SelfTransform.GetLocation());
	TArray<FRBFOutputWeight> OutputWeights;
	RBFSolver.Solve(*SolverData.Get(), Params, Targets, Input, OutputWeights);

	// 3: Weight値をもとに補間値を最終計算
	FColor result(0, 0, 0, 1);
	for (int i = 0; i < OutputWeights.Num(); i++)
	{
		result.R += Inputs[OutputWeights[i].TargetIndex].R * OutputWeights[i].TargetWeight;
		result.G += Inputs[OutputWeights[i].TargetIndex].G * OutputWeights[i].TargetWeight;
		result.B += Inputs[OutputWeights[i].TargetIndex].B * OutputWeights[i].TargetWeight;
	}

	return result;
}

void URBFInterpolationBPLibrary::InterpolateHandPose(
	FHandPoseContainer& Result,
	const FRBFParams& Params,
	const TArray<FTransform>& PlotTransforms,
	const TArray<FHandPoseContainer>& PlotHandPoses,
	const FTransform& CurrentTransform)
{
	if (PlotTransforms.IsEmpty() ||
		PlotHandPoses.IsEmpty() ||
		PlotTransforms.Num() != PlotHandPoses.Num())
	{
		Result = FHandPoseContainer();
		return;
	}

	// 散布データを専用クラスに変換
	TArray<FRBFTarget> Targets;
	for (const auto& Transform : PlotTransforms)
	{
		FRBFTarget Target;
		Target.AddFromVector(Transform.GetLocation());
		Targets.Add(Target);
	}

	// RBFソルバー初期化
	FRBFSolver RBFSolver;
	auto SolverData = RBFSolver.InitSolver(Params, Targets);

	// 現在座標に応じたWeight値を計算
	FRBFTarget Input;
	Input.AddFromVector(CurrentTransform.GetLocation());
	TArray<FRBFOutputWeight> OutputWeights;
	RBFSolver.Solve(*SolverData.Get(), Params, Targets, Input, OutputWeights);

	// Weight値をもとに掴み姿勢を計算
	auto& FirstPoseFingerArray = PlotHandPoses[0].FingerPoseArray;
	Result = FHandPoseContainer();
	Result.FingerPoseArray.AddUninitialized(FirstPoseFingerArray.Num());	
	for (int32 i = 0; i < Result.FingerPoseArray.Num(); i++)
	{
		FVector TempPosition = FVector(0, 0, 0);
		FVector4 TempRotation = FVector4(0, 0, 0, 0);
		FVector TempScale = FVector(1, 1, 1);

		for (int j = 0; j < OutputWeights.Num(); j++)
		{
			auto& FingerPoseArray = PlotHandPoses[OutputWeights[j].TargetIndex].FingerPoseArray;
			TempPosition += FingerPoseArray[i].Transform.GetLocation() * OutputWeights[j].TargetWeight;
			auto Quat = FingerPoseArray[i].Transform.GetRotation();
			TempRotation += FVector4(Quat.X, Quat.Y, Quat.Z, Quat.W) * OutputWeights[j].TargetWeight;			
		}
		
		auto newRotator = FRotator(FQuat(
			TempRotation.X,
			TempRotation.Y,
			TempRotation.Z,
			TempRotation.W
		).GetNormalized());
		Result.FingerPoseArray[i].Transform = FTransform(newRotator, TempPosition, TempScale);
		Result.FingerPoseArray[i].BoneName = FirstPoseFingerArray[i].BoneName;
	}
}

