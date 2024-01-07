// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNode_ApplyHandPose.h"
#include "Animation/AnimInstanceProxy.h"

void FAnimNode_ApplyHandPose::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
	check(OutBoneTransforms.Num() == 0);

	const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();
	const FTransform ComponentTransform = Output.AnimInstanceProxy->GetComponentTransform();

	for (auto& FingerPose : HandPose.FingerPoseArray)
	{
		const int32 SkeletonBoneIndex = BoneContainer.GetPoseBoneIndexForBoneName(FingerPose.BoneName);
		const FCompactPoseBoneIndex BoneIndex = BoneContainer.GetCompactPoseIndexFromSkeletonIndex(SkeletonBoneIndex);
		if (BoneIndex == INDEX_NONE) continue;

		const FCompactPoseBoneIndex ParentIndex = BoneContainer.GetParentBoneIndex(BoneIndex);
		if (ParentIndex == INDEX_NONE) continue;

		FTransform ParentTransform;

		// 親のComponentSpace座標が計算済みなので再利用
		// TODO: FingerPoseArrayの要素がソート済みである前提になるので、それを担保する構造に改善
		FBoneTransform* FoundElement = OutBoneTransforms.FindByPredicate([&](const FBoneTransform& Elem) {
			return Elem.BoneIndex == ParentIndex;
			});
		if (FoundElement == nullptr)
		{
			ParentTransform = Output.Pose.GetComponentSpaceTransform(ParentIndex);
		}
		else
		{
			ParentTransform = FoundElement->Transform;
		}

		// ParentBoneSpace座標をComponentSpace座標に変換して適用
		FTransform NewBoneTransform = FingerPose.Transform * ParentTransform;

		// 変更前のTransformとアルファブレンド
		auto PrevTransform = Output.Pose.GetComponentSpaceTransform(BoneIndex);
		NewBoneTransform.Blend(PrevTransform, NewBoneTransform, GetAlpha(FingerPose));

		OutBoneTransforms.Add(FBoneTransform(BoneIndex, NewBoneTransform));
	}
}

bool FAnimNode_ApplyHandPose::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
	// 入力のHandPoseに不正なボーン名が登録されていれば実行しない
	for (auto& FingerPose : HandPose.FingerPoseArray)
	{
		if (RequiredBones.GetPoseBoneIndexForBoneName(FingerPose.BoneName) == INDEX_NONE)
			return false;
	}

	return true;
}

float FAnimNode_ApplyHandPose::GetAlpha(FHandJointPose HandJointPose) const
{
	switch (HandJointPose.FingerGroup)
	{
	case FingerGroup::Thumb:
		return ThumbAlpha;
	case FingerGroup::Index:
		return IndexAlpha;
	case FingerGroup::Middle:
		return MiddleAlpha;
	case FingerGroup::Ring:
		return RingAlpha;
	case FingerGroup::Pinky:
		return PinkyAlpha;
	default:
		return 0.f;
	}
}