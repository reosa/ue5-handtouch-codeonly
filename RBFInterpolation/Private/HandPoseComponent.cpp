// Fill out your copyright notice in the Description page of Project Settings.


#include "HandPoseComponent.h"
#include "BoneContainer.h"
#include "Animation/AnimSingleNodeInstance.h"

void UHandPoseComponent::GetHandPose(
	FHandPoseContainer& HandPoseContainer,
	const TArray<FName> FingerBoneNames,
	const float Time) const
{
	const UAnimSingleNodeInstance* SingleNodeInstance = GetSingleNodeInstance();
	if (SingleNodeInstance == nullptr)
	{
		return;
	}

	const UAnimSequence* AnimSequence = Cast<UAnimSequence>(SingleNodeInstance->GetAnimationAsset());
	if (AnimSequence == nullptr || AnimSequence->GetSkeleton() == nullptr)
	{
		return;
	}

	// スケルトン情報を取得
	const USkeleton* Skeleton = AnimSequence->GetSkeleton();
	const FReferenceSkeleton* ReferenceSkeleton = &Skeleton->GetReferenceSkeleton();
	const auto NumBones = ReferenceSkeleton->GetNum();
	TArray<uint16> BoneIndices;
	BoneIndices.SetNumUninitialized(NumBones);
	for (auto Index = 0; Index < NumBones; ++Index)
	{
		BoneIndices[Index] = Index;
	}
	FBoneContainer BoneContainer;
	BoneContainer.InitializeTo(BoneIndices, UE::Anim::FCurveFilterSettings(), *(UObject*)Skeleton);

	// 姿勢情報を取得
	FCompactPose OutPose;
	OutPose.SetBoneContainer(&BoneContainer);
	FBlendedCurve OutCurve;
	OutCurve.InitFrom(BoneContainer);
	UE::Anim::FStackAttributeContainer TempAttributes;
	FAnimationPoseData AnimationPoseData(OutPose, OutCurve, TempAttributes);
	const FAnimExtractContext ExtractionContext(Time);
	AnimSequence->GetAnimationPose(AnimationPoseData, ExtractionContext);

	// 返却値に掴み姿勢を格納
	TArray<FHandJointPose> FingerPoseArray;
	FingerPoseArray.Reserve(FingerBoneNames.Num());
	for (int32 Index = 0; Index < NumBones; ++Index)
	{
		auto BoneName = ReferenceSkeleton->GetBoneName(Index);
		if (FingerBoneNames.Contains(BoneName) == false) continue;

		FHandJointPose JointPose;
		const auto CompactIndex = BoneContainer.MakeCompactPoseIndex(FMeshPoseBoneIndex(Index));
		auto LocalBoneTransform = AnimationPoseData.GetPose()[CompactIndex];

		JointPose.Transform = LocalBoneTransform;
		JointPose.BoneName = BoneName;
		JointPose.FingerGroup = ClassifyFingerGroup(BoneName);
		FingerPoseArray.Add(JointPose);
	}
	HandPoseContainer.FingerPoseArray = FingerPoseArray;
}

FingerGroup UHandPoseComponent::ClassifyFingerGroup(FName BoneName)
{
	auto NameStr = BoneName.ToString();
	if (NameStr.Contains(C_THUMB))
	{
		return FingerGroup::Thumb;
	}
	else if (NameStr.Contains(C_INDEX))
	{
		return FingerGroup::Index;
	}
	else if (NameStr.Contains(C_MIDDLE))
	{
		return FingerGroup::Middle;
	}
	else if (NameStr.Contains(C_RING))
	{
		return FingerGroup::Ring;
	}
	else if (NameStr.Contains(C_PINKY))
	{
		return FingerGroup::Pinky;
	}

	return FingerGroup::None;
}
