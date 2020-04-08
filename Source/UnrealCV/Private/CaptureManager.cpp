#include "UnrealCVPrivate.h"
#include "CineCameraActor.h"
#include "UE4CVServer.h"
#include "CaptureManager.h"

/**
  * Where to put cameras
  * For each camera in the scene, attach SceneCaptureComponent2D to it
  * So that ground truth can be generated, invoked when a new actor is created
  */
void FCaptureManager::AttachGTCaptureComponentToCamera(APawn* Pawn, TArray<AActor*> ActorList)
{
	FServerConfig& Config = FUE4CVServer::Get().Config;
	// TODO: Only support one camera at the beginning
	// TODO: Make this automatic from material loader.

	CaptureComponentList.Empty();

	UGTCaptureComponent* Capturer = UGTCaptureComponent::Create(Pawn, Config.SupportedModes);
	CaptureComponentList.Add(Capturer);

	// For each aCineCameraActor in the World add a Capturer in the CaptureComponentList
	ACineCameraActor* CineCameraActor = nullptr;
	for (AActor* Actor : ActorList)
	{
		if (Actor && Actor->IsA(ACineCameraActor::StaticClass()))
		{
			Capturer = UGTCaptureComponent::Create(Actor, Config.SupportedModes);
			CaptureComponentList.Add(Capturer);
		}
	}

	if (Config.EnableRightEye)
	{
		UGTCaptureComponent *RightEye = UGTCaptureComponent::Create(Pawn, Config.SupportedModes);
		RightEye->SetRelativeLocation(FVector(0, 40, 0));
		// RightEye->AddLocalOffset(FVector(0, 40, 0)); // TODO: make this configurable
		CaptureComponentList.Add(RightEye);
	}
}

UGTCaptureComponent* FCaptureManager::GetCamera(int32 CameraId)
{
	if (CameraId < CaptureComponentList.Num() && CameraId >= 0)
	{
		return CaptureComponentList[CameraId];
	}
	else
	{
		return nullptr;
	}
}
