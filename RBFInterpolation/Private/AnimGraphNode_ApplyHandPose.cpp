// Fill out your copyright notice in the Description page of Project Settings.
#define LOCTEXT_NAMESPACE "AnimGraphNode_ApplyHandPose"

#include "AnimGraphNode_ApplyHandPose.h"

UAnimGraphNode_ApplyHandPose::UAnimGraphNode_ApplyHandPose(
	const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

FText UAnimGraphNode_ApplyHandPose::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("ApplyHandPose", "Apply HandPose");
}
