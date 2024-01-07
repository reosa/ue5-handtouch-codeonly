// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNode_ApplyHandPose.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimGraphNode_ApplyHandPose.generated.h"

/// <summary>
/// HandPoseを姿勢に反映するAnimationBlueprintノードのエディタ上挙動
/// </summary>
UCLASS()
class UAnimGraphNode_ApplyHandPose : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_ApplyHandPose Node;

public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// End of UEdGraphNode interface

protected:
	// UAnimGraphNode_SkeletalControlBase interface
	virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
	// End of UAnimGraphNode_SkeletalControlBase interface
};
