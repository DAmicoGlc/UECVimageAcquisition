#pragma once
#include "UE4CVServer.h"
#include "Engine.h"
#include "GTCaptureComponent.generated.h"

struct FGTCaptureTask
{
	FString Mode;
	FString Filename;
	uint64 CurrentFrame;
	FAsyncRecord* AsyncRecord;
	FGTCaptureTask() {}
	FGTCaptureTask(FString InMode, FString InFilename, uint64 InCurrentFrame, FAsyncRecord* InAsyncRecord) :
		Mode(InMode), Filename(InFilename), CurrentFrame(InCurrentFrame), AsyncRecord(InAsyncRecord) {}
};

struct ComponentMap {
	TMap<FString, USceneCaptureComponent2D*> CaptureComponents;
	ComponentMap() {}
};

struct ImageStruct {
	TArray<FColor> ImageData;
	ImageStruct() {}
	ImageStruct(TArray<FColor> InImage) :
		ImageData(InImage) {}
};

/**
 * Use USceneCaptureComponent2D to export information from the scene.
 * This class needs to be tickable to update the rotation of the USceneCaptureComponent2D
 */
UCLASS()
class UNREALCV_API UGTCaptureComponent : public USceneComponent // , public FTickableGameObject
{
	GENERATED_BODY()
private:
	UGTCaptureComponent();

	TArray<uint8> NpySerialization(TArray<FColor> ImageData, int32 Width, int32 Height, int32 Channel);
	TArray<uint8> NpySerialization(TArray<FFloat16Color> ImageData, int32 Width, int32 Height, int32 Channel);
	TArray<uint8> NpySerialization(TArray<ImageStruct> ImageDataList, int32 Width, int32 Height, int32 Channel);

public:
	AActor* Actor;
	
	static UGTCaptureComponent* Create(AActor* Actor, TArray<FString> Modes);
	
	static UMaterial* GetMaterial(FString ModeName);
	
	// virtual void Tick(float DeltaTime) override; // TODO
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override; // TODO
	
	void SetFOVAngle(float FOV);
	
	/** Save image to a file */
	FAsyncRecord* Capture(FString Mode, FString Filename);
	FAsyncRecord* CameraComponentCapture(FString Mode, FString InFilename, int32 Index);
	FAsyncRecord* CineCameraComponentCapture(FString Mode, FString InFilename, int32 Index);
	
	/** Read binary data in png format */
	TArray<uint8> CapturePng(FString Mode);
	TArray<uint8> CameraComponentCapturePng(FString Mode, int32 Index);
	TArray<uint8> CineCameraComponentCapturePng(FString Mode, int32 Index);
	
	/** Read binary data in uncompressed numpy array */
	TArray<uint8> CaptureNpyUint8(FString Mode, int32 Channels);
	TArray<uint8> CameraComponentCaptureNpyUint8(FString Mode, int32 Channels, int32 Index);
	TArray<uint8> CineCameraComponentCaptureNpyUint8(FString Mode, int32 Channels, int32 Index);
	TArray<uint8> AllCameraComponentCaptureNpyUint8(FString Mode, int32 Channels);
	
	/** Read binary data in uncompressed numpy array */
	TArray<uint8> CaptureNpyFloat16(FString Mode, int32 Channels);
	TArray<uint8> CameraComponentCaptureNpyFloat16(FString Mode, int32 Channels, int32 Index);
	TArray<uint8> CineCameraComponentCaptureNpyFloat16(FString Mode, int32 Channels, int32 Index);
	
	USceneCaptureComponent2D* GetCaptureComponent(FString Mode);
	USceneCaptureComponent2D* GetCameraComponentCaptureComponent(FString Mode, int32 Index);
	USceneCaptureComponent2D* GetCineCameraComponentCaptureComponent(FString Mode, int32 Index);

	/** Return the rotation of the component **/
	FRotator GetCaptureComponentRotation();
	/** Return the location of the component **/
	FVector GetCaptureComponentLocation();

	/** Set the rotation of the component **/
	void SetCaptureComponentRotation(FRotator newRotation);
	/** Set the location of the component **/
	void SetCaptureComponentLocation(FVector newLocation);
private:
	const bool bIsTicking = true;

	TQueue<FGTCaptureTask, EQueueMode::Spsc> PendingTasks;
	TMap<FString, USceneCaptureComponent2D*> CaptureComponents;

	TArray<ComponentMap> CaptureCameraComponents;
	TArray<ComponentMap> CaptureCineCameraComponents;
};
