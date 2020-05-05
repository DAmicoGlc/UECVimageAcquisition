#pragma once
#include "CommandHandler.h"
#include "GTCaptureComponent.h"

class FCameraCommandHandler : public FCommandHandler
{
public:
	FCameraCommandHandler(FCommandDispatcher* InCommandDispatcher) : FCommandHandler(InCommandDispatcher)
	{}
	void RegisterCommands();

	/** vget /camera/horizontal_fieldofview */
	FExecStatus GetCameraHorizontalFieldOfView(const TArray<FString>& Args);
	/** vset /camera/horizontal_fieldofview */
	FExecStatus SetCameraHorizontalFieldOfView(const TArray<FString>& Args);
	/** vget /camera/location */
	FExecStatus GetCameraLocation(const TArray<FString>& Args);
	/** vset /camera/location */
	FExecStatus	SetCameraLocation(const TArray<FString>& Args);
	/** vset /camera/moveto */
	FExecStatus	MoveTo(const TArray<FString>& Args);
	/** vget /camera/[id]/rotation */
	FExecStatus GetCameraRotation(const TArray<FString>& Args);
	/** vset /camera/[id]/rotation */
	FExecStatus	SetCameraRotation(const TArray<FString>& Args);
	/** vget /camera/[id]/pose */
	FExecStatus GetCameraPose(const TArray<FString>& Args);
	/** vset /camera/[id]/pose */
	FExecStatus	SetCameraPose(const TArray<FString>& Args);

	/** vget /camera/view */
	FExecStatus GetScreenshot(const TArray<FString>& Args);

	/** Get camera image with a given mode, Get ViewMode data using SceneCaptureComponent, support multi-camera */
	FExecStatus GetCameraViewMode(const TArray<FString>& Args);

	/** Explicitly set ViewMode, then GetCameraViewMode */
	FExecStatus GetLitViewMode(const TArray<FString>& Args);

	/** Get camera project matrix */
	FExecStatus GetCameraProjMatrix(const TArray<FString>& Args);

	/** Get HDR buffer visualization */
	FExecStatus GetBuffer(const TArray<FString>& Args);

	/** Get ViewMode data by switching to this viewmode then switch back, can not support multi-camera */
	FExecStatus GetObjectInstanceMask(const TArray<FString>& Args);

	/** Get raw binary image data instead of filename */
	FExecStatus GetPngBinary(const TArray<FString>& Args, const FString& ViewMode);

	/** Get raw binary data as an uncompressed numpy array */
	TArray<uint8> GetNpyBinaryUint8Data(const TArray<FString>& Args, const FString& ViewMode, int32 Channels);

	/** Get response with raw binary data as an uncompressed numpy array */
	FExecStatus GetNpyBinaryUint8(const TArray<FString>& Args, const FString& ViewMode, int32 Channels);

	/** Get raw binary data as an uncompressed numpy array */
	TArray<uint8> GetNpyBinaryFloat16Data(const TArray<FString>& Args, const FString& ViewMode, int32 Channels);

	/** Get response with raw binary data as an uncompressed numpy array */
	FExecStatus GetNpyBinaryFloat16(const TArray<FString>& Args, const FString& ViewMode, int32 Channels);




	//************************************************* MY FUNCTION *************************************************//

	/** Get camera component image with a given mode, Get ViewMode data using SceneCaptureComponent, support multi-camera */
	FExecStatus GetCameraComponentViewMode(const TArray<FString>& Args);

	/** Explicitly set ViewMode, then GetCameraViewMode from camera components */
	FExecStatus GetCameraComponentLitViewMode(const TArray<FString>& Args);

	/** Get cine camera component image with a given mode, Get ViewMode data using SceneCaptureComponent, support multi-camera */
	FExecStatus GetCineCameraComponentViewMode(const TArray<FString>& Args);

	/** Explicitly set ViewMode, then GetCameraViewMode from cine camera components */
	FExecStatus GetCineCameraComponentLitViewMode(const TArray<FString>& Args);
	
	/** Get raw binary image data from camera component */
	FExecStatus GetCameraComponentPngBinary(const TArray<FString>& Args, const FString& ViewMode, const FString& ComponentType);

	/** Get raw binary data as an uncompressed numpy array from camera component */
	TArray<uint8> GetCameraComponentNpyBinaryUint8Data(const TArray<FString>& Args, const FString& ViewMode, int32 Channels, const FString& ComponentType);

	/** Get response with raw binary data as an uncompressed numpy array from camera component */
	FExecStatus GetCameraComponentNpyBinaryUint8(const TArray<FString>& Args, const FString& ViewMode, int32 Channels, const FString& ComponentType);

	/** Get response with raw binary data as an uncompressed numpy array from all camera component */
	FExecStatus GetAllCameraComponentNpyBinaryUint8(const TArray<FString>& Args, const FString& ViewMode, int32 Channels);
	TArray<uint8> GetAllCameraComponentNpyBinaryUint8Data(const TArray<FString>& Args, const FString& ViewMode, int32 Channels);

	/** Get raw binary data as an uncompressed numpy array from camera component */
	TArray<uint8> GetCameraComponentNpyBinaryFloat16Data(const TArray<FString>& Args, const FString& ViewMode, int32 Channels, const FString& ComponentType);

	/** Get response with raw binary data as an uncompressed numpy array from camera component */
	FExecStatus GetCameraComponentNpyBinaryFloat16(const TArray<FString>& Args, const FString& ViewMode, int32 Channels, const FString& ComponentType);

};
