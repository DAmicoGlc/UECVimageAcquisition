#include "UnrealCVPrivate.h"
#include "GTCaptureComponent.h"
#include "ViewMode.h"
#include "cnpy.h"
#include "Serialization.h"
#include "IImageWrapperModule.h"
#include "Camera/CameraComponent.h"
#include "CineCameraComponent.h"

DECLARE_CYCLE_STAT(TEXT("SaveExr"), STAT_SaveExr, STATGROUP_UnrealCV);
DECLARE_CYCLE_STAT(TEXT("SavePng"), STAT_SavePng, STATGROUP_UnrealCV);
DECLARE_CYCLE_STAT(TEXT("SaveFile"), STAT_SaveFile, STATGROUP_UnrealCV);
DECLARE_CYCLE_STAT(TEXT("ReadPixels"), STAT_ReadPixels, STATGROUP_UnrealCV);
DECLARE_CYCLE_STAT(TEXT("ImageWrapper"), STAT_ImageWrapper, STATGROUP_UnrealCV);
DECLARE_CYCLE_STAT(TEXT("GetResource"), STAT_GetResource, STATGROUP_UnrealCV);

void InitCaptureComponent(USceneCaptureComponent2D* CaptureComponent)
{
	UWorld* World = FUE4CVServer::Get().GetGameWorld();
	// Can not use ESceneCaptureSource::SCS_SceneColorHDR, this option will disable post-processing
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	CaptureComponent->TextureTarget = NewObject<UTextureRenderTarget2D>();
	FServerConfig& Config = FUE4CVServer::Get().Config;
	CaptureComponent->TextureTarget->InitAutoFormat(Config.Width, Config.Height);
	CaptureComponent->FOVAngle = Config.FOV;

	/*
	UGameViewportClient* GameViewportClient = World->GetGameViewport();
	CaptureComponent->TextureTarget->InitAutoFormat(GameViewportClient->Viewport->GetSizeXY().X,  GameViewportClient->Viewport->GetSizeXY().Y); // TODO: Update this later
	*/

	CaptureComponent->RegisterComponentWithWorld(World); // What happened for this?
	// CaptureComponent->AddToRoot(); This is not necessary since it has been attached to the Pawn.
}

void SaveExr(UTextureRenderTarget2D* RenderTarget, FString Filename)
{
	SCOPE_CYCLE_COUNTER(STAT_SaveExr)
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FFloat16Color> FloatImage;
	FloatImage.AddZeroed(Width * Height);
	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	RenderTargetResource->ReadFloat16Pixels(FloatImage);

	TArray<uint8> ExrData = SerializationUtils::Image2Exr(FloatImage, Width, Height);
	FFileHelper::SaveArrayToFile(ExrData, *Filename);
}

void SavePng(UTextureRenderTarget2D* RenderTarget, FString Filename)
{
	SCOPE_CYCLE_COUNTER(STAT_SavePng);

	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FColor> Image;
	FTextureRenderTargetResource* RenderTargetResource;
	Image.AddZeroed(Width * Height);
	{
		SCOPE_CYCLE_COUNTER(STAT_GetResource);
		RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	}
	{
		SCOPE_CYCLE_COUNTER(STAT_ReadPixels);
		FReadSurfaceDataFlags ReadSurfaceDataFlags;
		ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
		// Instead of using this flag, we will set the gamma to the correct value directly
		RenderTargetResource->ReadPixels(Image, ReadSurfaceDataFlags);
	}
	TArray<uint8> ImgData = SerializationUtils::Image2Png(Image, Width, Height);
	FFileHelper::SaveArrayToFile(ImgData, *Filename);
}

UMaterial* UGTCaptureComponent::GetMaterial(FString InModeName = TEXT(""))
{
	// Load material for visualization
	static TMap<FString, FString>* MaterialPathMap = nullptr;
	if (MaterialPathMap == nullptr)
	{
		MaterialPathMap = new TMap<FString, FString>();
		MaterialPathMap->Add(TEXT("depth"), TEXT("Material'/UnrealCV/SceneDepthWorldUnits.SceneDepthWorldUnits'"));
    	MaterialPathMap->Add(TEXT("plane_depth"), TEXT("Material'/UnrealCV/ScenePlaneDepthWorldUnits.ScenePlaneDepthWorldUnits'"));
		MaterialPathMap->Add(TEXT("vis_depth"), TEXT("Material'/UnrealCV/SceneDepth.SceneDepth'"));
		MaterialPathMap->Add(TEXT("debug"), TEXT("Material'/UnrealCV/debug.debug'"));
		// MaterialPathMap->Add(TEXT("object_mask"), TEXT("Material'/UnrealCV/VertexColorMaterial.VertexColorMaterial'"));
		MaterialPathMap->Add(TEXT("normal"), TEXT("Material'/UnrealCV/WorldNormal.WorldNormal'"));

		FString OpaqueMaterialName = "Material'/UnrealCV/OpaqueMaterial.OpaqueMaterial'";
		MaterialPathMap->Add(TEXT("opaque"), OpaqueMaterialName);
	}

	static TMap<FString, UMaterial*>* StaticMaterialMap = nullptr;
	if (StaticMaterialMap == nullptr)
	{
		StaticMaterialMap = new TMap<FString, UMaterial*>();
		for (auto& Elem : *MaterialPathMap)
		{
			FString ModeName = Elem.Key;
			FString MaterialPath = Elem.Value;
			ConstructorHelpers::FObjectFinder<UMaterial> Material(*MaterialPath); // ConsturctorHelpers is only available for UObject.

			if (Material.Object != NULL)
			{
				StaticMaterialMap->Add(ModeName, (UMaterial*)Material.Object);
			}
		}
	}

	UMaterial* Material = StaticMaterialMap->FindRef(InModeName);
	if (Material == nullptr)
	{
		UE_LOG(LogUnrealCV, Warning, TEXT("Can not recognize visualization mode %s"), *InModeName);
	}
	return Material;
}

/**
Attach a GTCaptureComponent to a pawn
 */
UGTCaptureComponent* UGTCaptureComponent::Create(AActor* InActor, TArray<FString> Modes)
{
	UWorld* World = FUE4CVServer::Get().GetGameWorld();
	UGTCaptureComponent* GTCapturer = NewObject<UGTCaptureComponent>();

	GTCapturer->SetActive(true);
	// check(GTCapturer->IsComponentTickEnabled() == true);
	GTCapturer->Actor = InActor; // This GTCapturer should depend on the Pawn and be released together with the Pawn.

	// This snippet is from Engine/Source/Runtime/Engine/Private/Components/SceneComponent.cpp, AttachTo
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, false);
	ConvertAttachLocation(EAttachLocation::KeepRelativeOffset, AttachmentRules.LocationRule, AttachmentRules.RotationRule, AttachmentRules.ScaleRule);
	GTCapturer->AttachToComponent(InActor->GetRootComponent(), AttachmentRules);
	// GTCapturer->AddToRoot();
	GTCapturer->RegisterComponentWithWorld(World);
	GTCapturer->SetTickableWhenPaused(true);

	for (FString Mode : Modes)
	{
		// DEPRECATED_FORGAME(4.6, "CaptureComponent2D should not be accessed directly, please use GetCaptureComponent2D() function instead. CaptureComponent2D will soon be private and your code will not compile.")
		USceneCaptureComponent2D* CaptureComponent = NewObject<USceneCaptureComponent2D>();
		CaptureComponent->SetActive(false); // Disable it by default for performance consideration
		GTCapturer->CaptureComponents.Add(Mode, CaptureComponent);

		// CaptureComponent needs to be attached to somewhere immediately, otherwise it will be gc-ed

		CaptureComponent->AttachToComponent(GTCapturer, AttachmentRules);
		InitCaptureComponent(CaptureComponent);

		UMaterial* Material = GetMaterial(Mode);
		if (Mode == "lit") // For rendered images
		{
			// FViewMode::Lit(CaptureComponent->ShowFlags);
			CaptureComponent->TextureTarget->TargetGamma = GEngine->GetDisplayGamma();
			// float DisplayGamma = SceneViewport->GetDisplayGamma();
		}
		else if (Mode == "default")
		{
			continue;
		}
		else // for ground truth
		{
			CaptureComponent->TextureTarget->TargetGamma = 1;
			if (Mode == "object_mask") // For object mask
			{
				// FViewMode::Lit(CaptureComponent->ShowFlags);
				FViewMode::VertexColor(CaptureComponent->ShowFlags);
			}
			else if (Mode == "wireframe") // For object mask
			{
				FViewMode::Wireframe(CaptureComponent->ShowFlags);
			}
			else
			{
				check(Material);
				// GEngine->GetDisplayGamma(), the default gamma is 2.2
				// CaptureComponent->TextureTarget->TargetGamma = 2.2;
				FViewMode::PostProcess(CaptureComponent->ShowFlags);

				CaptureComponent->PostProcessSettings.AddBlendable(Material, 1);
				// Instead of switching post-process materials, we create several SceneCaptureComponent, so that we can capture different GT within the same frame.
			}
		}
	}

	//************************************************* MY EXTENSION *************************************************//

	// Extract all components of the pawn
    auto components = InActor->GetComponents();

    // Need to dynamic casting
    UCameraComponent* CameraComponent = nullptr;
    UCineCameraComponent* CineCameraComponent = nullptr;

	TArray<UCameraComponent*> CameraComponentList;
	TArray<UCineCameraComponent*> CineCameraComponentList;

    // Search for cineCameraComponent and cameraComponent
	for (auto component : components)
	{
        // Try to cast it to CineCameraComponent
        CineCameraComponent = dynamic_cast<UCineCameraComponent*>(component);
        if (CineCameraComponent != nullptr) 
        {
            CineCameraComponentList.Push(CineCameraComponent);
        }
        else 
        {
            // Try to cast it to CameraComponent
            CameraComponent = dynamic_cast<UCameraComponent*>(component);
            if (CameraComponent != nullptr) 
            {
                CameraComponentList.Push(CameraComponent);
            }
        }
	}

    // Create a USceneCaptureComponent2D for each cameraComponent and cineCameraComponent
    // and for each Mode
    for (int32 Index = 0; Index != CineCameraComponentList.Num(); ++Index) {
        CineCameraComponent = CineCameraComponentList[Index];
		ComponentMap NewComponentMap = ComponentMap();
		GTCapturer->CaptureCineCameraComponents.Add(NewComponentMap);

        for (FString Mode : Modes)
	    {
            // Create the USceneCaptureComponent2D
            USceneCaptureComponent2D* CaptureComponent = NewObject<USceneCaptureComponent2D>();
		    CaptureComponent->SetActive(false);

            // Define the attachmentRules and Attach the USceneCaptureComponent2D to the cameraComponent
            CaptureComponent->AttachToComponent(CineCameraComponent, AttachmentRules);
            InitCaptureComponent(CaptureComponent);

            // Add USceneCaptureComponent2D to the capture component list
            GTCapturer->CaptureCineCameraComponents[Index].CaptureComponents.Add(Mode, CaptureComponent);

            UMaterial* Material = GetMaterial(Mode);
            if (Mode == "lit")
            {
                CaptureComponent->TextureTarget->TargetGamma = GEngine->GetDisplayGamma();
            }
            else if (Mode == "default")
            {
                continue;
            }
            else 
            {
                CaptureComponent->TextureTarget->TargetGamma = 1;
                if (Mode == "object_mask")
                {
                    FViewMode::VertexColor(CaptureComponent->ShowFlags);
                }
                else if (Mode == "wireframe")
                {
                    FViewMode::Wireframe(CaptureComponent->ShowFlags);
                }
                else
                {
                    check(Material);
                    FViewMode::PostProcess(CaptureComponent->ShowFlags);
                    CaptureComponent->PostProcessSettings.AddBlendable(Material, 1);
                }
            }

        }

    }

    for (int32 Index = 0; Index != CameraComponentList.Num(); ++Index) {
        CameraComponent = CameraComponentList[Index];
		ComponentMap NewComponentMap = ComponentMap();
		GTCapturer->CaptureCameraComponents.Add(NewComponentMap);

        for (FString Mode : Modes)
	    {
            // Create the USceneCaptureComponent2D
            USceneCaptureComponent2D* CaptureComponent = NewObject<USceneCaptureComponent2D>();
		    CaptureComponent->SetActive(false);

            // Define the attachmentRules and Attach the USceneCaptureComponent2D to the cameraComponent
            CaptureComponent->AttachToComponent(CameraComponent, AttachmentRules);
            InitCaptureComponent(CaptureComponent);

            // Add USceneCaptureComponent2D to the capture component list
            GTCapturer->CaptureCameraComponents[Index].CaptureComponents.Add(Mode, CaptureComponent);

            UMaterial* Material = GetMaterial(Mode);
            if (Mode == "lit")
            {
                CaptureComponent->TextureTarget->TargetGamma = GEngine->GetDisplayGamma();
            }
            else if (Mode == "default")
            {
                continue;
            }
            else 
            {
                CaptureComponent->TextureTarget->TargetGamma = 1;
                if (Mode == "object_mask")
                {
                    FViewMode::VertexColor(CaptureComponent->ShowFlags);
                }
                else if (Mode == "wireframe")
                {
                    FViewMode::Wireframe(CaptureComponent->ShowFlags);
                }
                else
                {
                    check(Material);
                    FViewMode::PostProcess(CaptureComponent->ShowFlags);
                    CaptureComponent->PostProcessSettings.AddBlendable(Material, 1);
                }
            }

        }

    }

	return GTCapturer;
}

UGTCaptureComponent::UGTCaptureComponent()
{
	GetMaterial(); // Initialize the TMap
	PrimaryComponentTick.bCanEverTick = true;
	// bIsTicking = false;

	// Create USceneCaptureComponent2D
	// Design Choice: do I need one capture component with changing post-process materials or multiple components?
	// USceneCaptureComponent2D* CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("GTCaptureComponent"));
}

// Each GTCapturer can serve as one camera of the scene
void UGTCaptureComponent::SetFOVAngle(float FOV)
{
    for (auto Iterator = CaptureComponents.CreateIterator(); Iterator; ++Iterator)
    {
        Iterator.Value()->FOVAngle = FOV;
    }

	for (auto CameraComponentMap : CaptureCameraComponents) 
	{
		for (auto Iterator = CameraComponentMap.CaptureComponents.CreateIterator(); Iterator; ++Iterator)
		{
			Iterator.Value()->FOVAngle = FOV;
		}
	}

	for (auto CineCameraComponentMap : CaptureCameraComponents) 
	{
		for (auto Iterator = CineCameraComponentMap.CaptureComponents.CreateIterator(); Iterator; ++Iterator)
		{
			Iterator.Value()->FOVAngle = FOV;
		}
	}
}

FAsyncRecord* UGTCaptureComponent::Capture(FString Mode, FString InFilename)
{
	// Flush location and rotation
	check(CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureComponents.FindRef(Mode);
	if (CaptureComponent == nullptr)
		return nullptr;
	
	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	FAsyncRecord* AsyncRecord = FAsyncRecord::Create();
	FGTCaptureTask GTCaptureTask = FGTCaptureTask(Mode, InFilename, GFrameCounter, AsyncRecord);
	this->PendingTasks.Enqueue(GTCaptureTask);

	return AsyncRecord;
}

TArray<uint8> UGTCaptureComponent::CapturePng(FString Mode)
{
	// Flush location and rotation
	check(CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureComponents.FindRef(Mode);

	TArray<uint8> ImgData;
	if (CaptureComponent == nullptr)
		return ImgData;

	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FColor> Image;
	FTextureRenderTargetResource* RenderTargetResource;
	Image.AddZeroed(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
												  // Instead of using this flag, we will set the gamma to the correct value directly
	RenderTargetResource->ReadPixels(Image, ReadSurfaceDataFlags);
	ImageWrapper->SetRaw(Image.GetData(), Image.GetAllocatedSize(), Width, Height, ERGBFormat::BGRA, 8);
	ImgData = ImageWrapper->GetCompressed();

	return ImgData;
}

TArray<uint8> UGTCaptureComponent::CaptureNpyUint8(FString Mode, int32 Channels)
{
	// Flush location and rotation
	check(CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureComponents.FindRef(Mode);

	TArray<uint8> NpyData;
	if (CaptureComponent == nullptr) {
		UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return NpyData;
	}

	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FColor> ImageData;
	FTextureRenderTargetResource* RenderTargetResource;
	ImageData.AddUninitialized(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
	// Instead of using this flag, we will set the gamma to the correct value directly
	RenderTargetResource->ReadPixels(ImageData, ReadSurfaceDataFlags);

	// Check the byte order of data
	// Compress image data to npy array
	// Generate a header for the numpy array
	NpyData = NpySerialization(ImageData, Width, Height, Channels);

	return NpyData;
}

TArray<uint8> UGTCaptureComponent::CaptureNpyFloat16(FString Mode, int32 Channels)
{
	// Flush location and rotation
	check(CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureComponents.FindRef(Mode);

	TArray<uint8> NpyData;
	if (CaptureComponent == nullptr) {
	  	UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return NpyData;
  	}

	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FFloat16Color> ImageData;
	FTextureRenderTargetResource* RenderTargetResource;
	ImageData.AddUninitialized(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	RenderTargetResource->ReadFloat16Pixels(ImageData);

	// Check the byte order of data
	// Compress image data to npy array
	// Generate a header for the numpy array
	NpyData = NpySerialization(ImageData, Width, Height, Channels);

	return NpyData;
}

TArray<uint8> UGTCaptureComponent::NpySerialization(TArray<FColor> ImageData, int32 Width, int32 Height, int32 Channel)
{
	uint8 *TypePointer = nullptr; // Only used for determing the type

	std::vector<int> Shape;
	Shape.push_back(Height);
	Shape.push_back(Width);
	if (Channel != 1) Shape.push_back(Channel);

	std::vector<char> NpyHeader = cnpy::create_npy_header(TypePointer, Shape);

	// Append the actual data
	// FIXME: A slow implementation to convert TArray<FFloat16Color> to binary.
	/* A small size test
	std::vector<char> NpyData;
	for (int i = 0; i < 3 * 3 * 3; i++)
	{
		NpyData.push_back(i);
	}
	*/
	// std::vector<char> NpyData;
	std::vector<uint8> Uint8Data;
	Uint8Data.reserve(ImageData.Num() * Channel);

	for (int i = 0; i < ImageData.Num(); i++)
	{
		if (Channel == 1)
		{
			uint8 v = ImageData[i].R;
			Uint8Data.push_back(ImageData[i].R);
		}
		else if (Channel == 3)
		{
			Uint8Data.push_back(ImageData[i].R);
			Uint8Data.push_back(ImageData[i].G);
			Uint8Data.push_back(ImageData[i].B);
		}
		else if (Channel == 4)
		{
			Uint8Data.push_back(ImageData[i].R);
			Uint8Data.push_back(ImageData[i].G);
			Uint8Data.push_back(ImageData[i].B);
			Uint8Data.push_back(ImageData[i].A);
		}
	}
	check(Uint8Data.size() == Width * Height * Channel);
	// Convert to binary array
	const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&Uint8Data[0]);

	// https://stackoverflow.com/questions/22629728/what-is-the-difference-between-char-and-unsigned-char
	// https://stackoverflow.com/questions/11022099/convert-float-vector-to-byte-vector-and-back
	std::vector<unsigned char> NpyData(bytes, bytes + sizeof(uint8) * Uint8Data.size());

	NpyHeader.insert(NpyHeader.end(), NpyData.begin(), NpyData.end());

	// FIXME: Find a more efficient implementation
	TArray<uint8> BinaryData;
	for (char Element : NpyHeader)
	{
		BinaryData.Add(Element);
	}
	return BinaryData;
}

TArray<uint8> UGTCaptureComponent::NpySerialization(TArray<FFloat16Color> ImageData, int32 Width, int32 Height, int32 Channel)
{
	float *TypePointer = nullptr; // Only used for determing the type

	std::vector<int> Shape;
	Shape.push_back(Height);
	Shape.push_back(Width);
	if (Channel != 1) Shape.push_back(Channel);

	std::vector<char> NpyHeader = cnpy::create_npy_header(TypePointer, Shape);

	// Append the actual data
	// FIXME: A slow implementation to convert TArray<FFloat16Color> to binary.
	/* A small size test
	std::vector<char> NpyData;
	for (int i = 0; i < 3 * 3 * 3; i++)
	{
		NpyData.push_back(i);
	}
	*/
	// std::vector<char> NpyData;
	std::vector<float> FloatData;
	FloatData.reserve(ImageData.Num() * Channel);
	for (int i = 0; i < ImageData.Num(); i++)
	{
		if (Channel == 1)
		{
			const float Value = ImageData[i].R;
			FloatData.push_back(Value);
		}
		if (Channel == 3)
		{
			FloatData.push_back(ImageData[i].R);
			FloatData.push_back(ImageData[i].G);
			FloatData.push_back(ImageData[i].B); // TODO: Is this a correct order in numpy?
		}
	}
	check(FloatData.size() == Width * Height * Channel);
	// Convert to binary array
	const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&FloatData[0]);

	// https://stackoverflow.com/questions/22629728/what-is-the-difference-between-char-and-unsigned-char
	// https://stackoverflow.com/questions/11022099/convert-float-vector-to-byte-vector-and-back
	std::vector<unsigned char> NpyData(bytes, bytes + sizeof(float) * FloatData.size());

	NpyHeader.insert(NpyHeader.end(), NpyData.begin(), NpyData.end());

	// FIXME: Find a more efficient implementation
	TArray<uint8> BinaryData;
	for (char Element : NpyHeader)
	{
		BinaryData.Add(Element);
	}
	return BinaryData;
}

void UGTCaptureComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
// void UGTCaptureComponent::Tick(float DeltaTime) // This tick function should be called by the scene instead of been
{
	// Render pixels out in the next tick. To allow time to render images out.

	// Update rotation of each frame
	// from ab237f46dc0eee40263acbacbe938312eb0dffbb:CameraComponent.cpp:232
	// if (this->isPawn) {
	// 	check(this->Pawn); // this GTCapturer should be released, if the Pawn is deleted.
	// 	const APawn* OwningPawn = this->Pawn;
	// 	const AController* OwningController = OwningPawn ? OwningPawn->GetController() : nullptr;
	// 	if (OwningController && OwningController->IsLocalPlayerController())
	// 	{
	// 		const FRotator PawnViewRotation = OwningPawn->GetViewRotation();
	// 		for (auto Elem : CaptureComponents)
	// 		{
	// 			USceneCaptureComponent2D* CaptureComponent = Elem.Value;
	// 			CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	// 		}
	// 	}
	// }
	
	// while (!PendingTasks.IsEmpty())
	// {
	// 	FGTCaptureTask Task;
	// 	PendingTasks.Peek(Task);
	// 	uint64 CurrentFrame = GFrameCounter;

	// 	int32 SkipFrame = 1;
	// 	if (!(CurrentFrame > Task.CurrentFrame + SkipFrame)) // TODO: This is not an elegant solution, fix it later.
	// 	{ // Wait for the rendering thread to catch up game thread.
	// 		break;
	// 	}

	// 	PendingTasks.Dequeue(Task);
	// 	USceneCaptureComponent2D* CaptureComponent = this->CaptureComponents.FindRef(Task.Mode);
	// 	if (CaptureComponent == nullptr)
	// 	{
	// 		UE_LOG(LogUnrealCV, Warning, TEXT("Unrecognized capture mode %s"), *Task.Mode);
	// 	}
	// 	else
	// 	{
	// 		FString LowerCaseFilename = Task.Filename.ToLower();
	// 		if (LowerCaseFilename.EndsWith("png"))
	// 		{
	// 			SavePng(CaptureComponent->TextureTarget, Task.Filename);
	// 		}
	// 		else if (LowerCaseFilename.EndsWith("exr"))
	// 		{
	// 			SaveExr(CaptureComponent->TextureTarget, Task.Filename);
	// 		}
	// 		else
	// 		{
	// 			UE_LOG(LogUnrealCV, Warning, TEXT("Unrecognized image file extension %s"), *LowerCaseFilename);
	// 		}
	// 	}
	// 	Task.AsyncRecord->bIsCompleted = true;
	// }
}

USceneCaptureComponent2D* UGTCaptureComponent::GetCaptureComponent(FString Mode)
{
	check(CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureComponents.FindRef(Mode);
    return CaptureComponent;
}

/** Return the rotation of the component **/
FRotator UGTCaptureComponent::GetCaptureComponentRotation()
{
	AActor* actualActor = this->Actor;
	FRotator CameraRotation = actualActor->GetActorRotation();

	return CameraRotation;
}

/** Return the location of the component **/
FVector UGTCaptureComponent::GetCaptureComponentLocation()
{
	AActor* actualActor = this->Actor;
	FVector CameraLocation = actualActor->GetActorLocation();
	
	return CameraLocation;
}

/** Set the rotation of the component **/
void UGTCaptureComponent::SetCaptureComponentRotation(FRotator newRotation) 
{
	AActor* actualActor = this->Actor;
	actualActor->SetActorRotation(newRotation);
}
/** Set the location of the component **/
void UGTCaptureComponent::SetCaptureComponentLocation(FVector newLocation) {
	bool Success;
	bool Sweep = false;
	AActor* actualActor = this->Actor;
	Success = actualActor->SetActorLocation(newLocation, Sweep, NULL, ETeleportType::TeleportPhysics);
}

//************************************************* MY FUNCTION *************************************************//

FAsyncRecord* UGTCaptureComponent::CameraComponentCapture(FString Mode, FString InFilename, int32 Index)
{
	// Flush location and rotation
	if (CaptureCameraComponents.Num() <= Index) 
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component with index %d not found. Returning null pointer."), Index);
		return nullptr;
	}
	if (CaptureCameraComponents[Index].CaptureComponents.Num() == 0) 
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning null pointer."), *Mode);
		return nullptr;
	}
	USceneCaptureComponent2D* CaptureComponent = CaptureCameraComponents[Index].CaptureComponents.FindRef(Mode);
	if (CaptureComponent == nullptr)
		return nullptr;
	
	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	FAsyncRecord* AsyncRecord = FAsyncRecord::Create();
	FGTCaptureTask GTCaptureTask = FGTCaptureTask(Mode, InFilename, GFrameCounter, AsyncRecord);
	this->PendingTasks.Enqueue(GTCaptureTask);

	return AsyncRecord;
}

FAsyncRecord* UGTCaptureComponent::CineCameraComponentCapture(FString Mode, FString InFilename, int32 Index)
{
	// Flush location and rotation
	if (CaptureCineCameraComponents.Num() <= Index) 
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component with index %d not found. Returning null pointer."), Index);
		return nullptr;
	}
	if (CaptureCineCameraComponents[Index].CaptureComponents.Num() == 0) 
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning null pointer."), *Mode);
		return nullptr;
	}
	USceneCaptureComponent2D* CaptureComponent = CaptureCineCameraComponents[Index].CaptureComponents.FindRef(Mode);
	if (CaptureComponent == nullptr)
		return nullptr;
	
	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	FAsyncRecord* AsyncRecord = FAsyncRecord::Create();
	FGTCaptureTask GTCaptureTask = FGTCaptureTask(Mode, InFilename, GFrameCounter, AsyncRecord);
	this->PendingTasks.Enqueue(GTCaptureTask);

	return AsyncRecord;
}

TArray<uint8> UGTCaptureComponent::CameraComponentCapturePng(FString Mode, int32 Index)
{
	if (CaptureCameraComponents.Num() <= 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component with index %d not found. Returning empty array."), Index);
		return TArray<uint8>();
	}
	if (CaptureCameraComponents[Index].CaptureComponents.Num() == 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return TArray<uint8>();
	}
	USceneCaptureComponent2D* CaptureComponent = CaptureCameraComponents[Index].CaptureComponents.FindRef(Mode);

	TArray<uint8> ImgData;
	if (CaptureComponent == nullptr)
		return ImgData;

	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FColor> Image;
	FTextureRenderTargetResource* RenderTargetResource;
	Image.AddZeroed(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
												  // Instead of using this flag, we will set the gamma to the correct value directly
	RenderTargetResource->ReadPixels(Image, ReadSurfaceDataFlags);
	ImageWrapper->SetRaw(Image.GetData(), Image.GetAllocatedSize(), Width, Height, ERGBFormat::BGRA, 8);
	ImgData = ImageWrapper->GetCompressed();

	return ImgData;
}

TArray<uint8> UGTCaptureComponent::CineCameraComponentCapturePng(FString Mode, int32 Index)
{
	if (CaptureCineCameraComponents.Num() <= 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component with index %d not found. Returning empty array."), Index);
		return TArray<uint8>();
	}
	if (CaptureCineCameraComponents[Index].CaptureComponents.Num() == 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return TArray<uint8>();
	}
	USceneCaptureComponent2D* CaptureComponent = CaptureCineCameraComponents[Index].CaptureComponents.FindRef(Mode);

	TArray<uint8> ImgData;
	if (CaptureComponent == nullptr)
		return ImgData;

	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	static IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	static TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FColor> Image;
	FTextureRenderTargetResource* RenderTargetResource;
	Image.AddZeroed(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
												  // Instead of using this flag, we will set the gamma to the correct value directly
	RenderTargetResource->ReadPixels(Image, ReadSurfaceDataFlags);
	ImageWrapper->SetRaw(Image.GetData(), Image.GetAllocatedSize(), Width, Height, ERGBFormat::BGRA, 8);
	ImgData = ImageWrapper->GetCompressed();

	return ImgData;
}

TArray<uint8> UGTCaptureComponent::CameraComponentCaptureNpyUint8(FString Mode, int32 Channels, int32 Index)
{
	if (CaptureCameraComponents.Num() <= 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component with index %d not found. Returning empty array."), Index);
		return TArray<uint8>();
	}
	if (CaptureCameraComponents[Index].CaptureComponents.Num() == 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return TArray<uint8>();
	}
	USceneCaptureComponent2D* CaptureComponent = CaptureCameraComponents[Index].CaptureComponents.FindRef(Mode);

	TArray<uint8> NpyData;
	if (CaptureComponent == nullptr) {
		UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return NpyData;
	}

	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FColor> ImageData;
	FTextureRenderTargetResource* RenderTargetResource;
	ImageData.AddUninitialized(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
	// Instead of using this flag, we will set the gamma to the correct value directly
	RenderTargetResource->ReadPixels(ImageData, ReadSurfaceDataFlags);

	// Check the byte order of data
	// Compress image data to npy array
	// Generate a header for the numpy array
	NpyData = NpySerialization(ImageData, Width, Height, Channels);

	return NpyData;
}

TArray<uint8> UGTCaptureComponent::CineCameraComponentCaptureNpyUint8(FString Mode, int32 Channels, int32 Index)
{
	if (CaptureCineCameraComponents.Num() <= 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component with index %d not found. Returning empty array."), Index);
		return TArray<uint8>();
	}
	if (CaptureCineCameraComponents[Index].CaptureComponents.Num() == 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return TArray<uint8>();
	}
	USceneCaptureComponent2D* CaptureComponent = CaptureCineCameraComponents[Index].CaptureComponents.FindRef(Mode);

	TArray<uint8> NpyData;
	if (CaptureComponent == nullptr) {
		UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return NpyData;
	}

	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FColor> ImageData;
	FTextureRenderTargetResource* RenderTargetResource;
	ImageData.AddUninitialized(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	FReadSurfaceDataFlags ReadSurfaceDataFlags;
	ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
	// Instead of using this flag, we will set the gamma to the correct value directly
	RenderTargetResource->ReadPixels(ImageData, ReadSurfaceDataFlags);

	// Check the byte order of data
	// Compress image data to npy array
	// Generate a header for the numpy array
	NpyData = NpySerialization(ImageData, Width, Height, Channels);

	return NpyData;
}

TArray<uint8> UGTCaptureComponent::AllCameraComponentCaptureNpyUint8(FString Mode, int32 Channels)
{
	if (CaptureCameraComponents.Num() <= 0 && CaptureCineCameraComponents.Num() <= 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Components not found. Returning empty array."));
		return TArray<uint8>();
	}

	USceneCaptureComponent2D* CaptureComponent;
	TArray<uint8> NpyData = TArray<uint8>();
	TArray<ImageStruct> ImageList;
	ImageStruct ImageDataStruct;
	int32 Width = 0, Height = 0;
	
	for (int index = 0; index < CaptureCameraComponents.Num(); index++)
	{
		if (CaptureCameraComponents[index].CaptureComponents.Num() == 0)
		{
			UE_LOG(LogUnrealCV, Error, TEXT("Component for index %d not found. Returning empty array."), index);
			return TArray<uint8>();
		}

		CaptureComponent = CaptureCameraComponents[index].CaptureComponents.FindRef(Mode);
		if (CaptureComponent == nullptr) {
			UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
			return NpyData;
		}

		CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

		UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
		Width = RenderTarget->SizeX;
		Height = RenderTarget->SizeY;
		TArray<FColor> ImageData;
		FTextureRenderTargetResource* RenderTargetResource;
		ImageData.AddUninitialized(Width * Height);
		RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
		FReadSurfaceDataFlags ReadSurfaceDataFlags;
		ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
		// Instead of using this flag, we will set the gamma to the correct value directly
		RenderTargetResource->ReadPixels(ImageData, ReadSurfaceDataFlags);

		ImageDataStruct = ImageStruct(ImageData);

		// Check the byte order of data
		// Compress image data to npy array
		// Generate a header for the numpy array
		ImageList.Add(ImageDataStruct);
		
	}

	for (int index = 0; index < CaptureCineCameraComponents.Num(); index++)
	{
		if (CaptureCineCameraComponents[index].CaptureComponents.Num() == 0)
		{
			UE_LOG(LogUnrealCV, Error, TEXT("Component for index %d not found. Returning empty array."), index);
			return TArray<uint8>();
		}

		CaptureComponent = CaptureCineCameraComponents[index].CaptureComponents.FindRef(Mode);
		if (CaptureComponent == nullptr) {
			UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
			return NpyData;
		}

		CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

		UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
		Width = RenderTarget->SizeX;
		Height = RenderTarget->SizeY;
		TArray<FColor> ImageData;
		FTextureRenderTargetResource* RenderTargetResource;
		ImageData.AddUninitialized(Width * Height);
		RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
		FReadSurfaceDataFlags ReadSurfaceDataFlags;
		ReadSurfaceDataFlags.SetLinearToGamma(false); // This is super important to disable this!
		// Instead of using this flag, we will set the gamma to the correct value directly
		RenderTargetResource->ReadPixels(ImageData, ReadSurfaceDataFlags);

		ImageDataStruct = ImageStruct(ImageData);

		// Check the byte order of data
		// Compress image data to npy array
		// Generate a header for the numpy array
		ImageList.Add(ImageStruct(ImageDataStruct));
		
	}

	if (Width == 0 || Height == 0) 
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Height or Width error for components for mode %s. Returning empty array."), *Mode);
		return NpyData;
	} 
	NpyData = NpySerialization(ImageList, Width, Height, 3);

	return NpyData;
}

TArray<uint8> UGTCaptureComponent::NpySerialization(TArray<ImageStruct> ImageDataList, int32 Width, int32 Height, int32 Channel)
{
	uint8 *TypePointer = nullptr; // Only used for determing the type

	std::vector<int> Shape;
	Shape.push_back(ImageDataList.Num());
	Shape.push_back(Height);
	Shape.push_back(Width);
	if (Channel != 1) Shape.push_back(Channel);

	std::vector<char> NpyHeader = cnpy::create_npy_header(TypePointer, Shape);

	std::vector<uint8> Uint8Data;
	Uint8Data.reserve(Height * Width * Channel * ImageDataList.Num());

	for (int j = 0; j < ImageDataList.Num(); j++)
	{
		for (int i = 0; i < Height * Width; i++)
		{
			if (Channel == 1)
			{
				Uint8Data.push_back(ImageDataList[j].ImageData[i].R);
			}
			else if (Channel == 3)
			{
				Uint8Data.push_back(ImageDataList[j].ImageData[i].R);
				Uint8Data.push_back(ImageDataList[j].ImageData[i].G);
				Uint8Data.push_back(ImageDataList[j].ImageData[i].B);
			}
			else if (Channel == 4)
			{
				Uint8Data.push_back(ImageDataList[j].ImageData[i].R);
				Uint8Data.push_back(ImageDataList[j].ImageData[i].G);
				Uint8Data.push_back(ImageDataList[j].ImageData[i].B);
				Uint8Data.push_back(ImageDataList[j].ImageData[i].A);
			}
		}
	}
	check(Uint8Data.size() == Height * Width * Channel * ImageDataList.Num());
	// Convert to binary array
	const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&Uint8Data[0]);

	// https://stackoverflow.com/questions/22629728/what-is-the-difference-between-char-and-unsigned-char
	// https://stackoverflow.com/questions/11022099/convert-float-vector-to-byte-vector-and-back
	std::vector<unsigned char> NpyData(bytes, bytes + sizeof(uint8) * Uint8Data.size());

	NpyHeader.insert(NpyHeader.end(), NpyData.begin(), NpyData.end());

	// FIXME: Find a more efficient implementation
	TArray<uint8> BinaryData;
	for (char Element : NpyHeader)
	{
		BinaryData.Add(Element);
	}
	return BinaryData;
}


TArray<uint8> UGTCaptureComponent::CameraComponentCaptureNpyFloat16(FString Mode, int32 Channels, int32 Index)
{
	if (CaptureCameraComponents.Num() <= 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component with index %d not found. Returning empty array."), Index);
		return TArray<uint8>();
	}
	if (CaptureCameraComponents[Index].CaptureComponents.Num() == 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return TArray<uint8>();
	}
	USceneCaptureComponent2D* CaptureComponent = CaptureCameraComponents[Index].CaptureComponents.FindRef(Mode);

	TArray<uint8> NpyData;
	if (CaptureComponent == nullptr) {
	  	UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return NpyData;
  	}

	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FFloat16Color> ImageData;
	FTextureRenderTargetResource* RenderTargetResource;
	ImageData.AddUninitialized(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	RenderTargetResource->ReadFloat16Pixels(ImageData);

	// Check the byte order of data
	// Compress image data to npy array
	// Generate a header for the numpy array
	NpyData = NpySerialization(ImageData, Width, Height, Channels);

	return NpyData;
}

TArray<uint8> UGTCaptureComponent::CineCameraComponentCaptureNpyFloat16(FString Mode, int32 Channels, int32 Index)
{
	if (CaptureCineCameraComponents.Num() <= 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component with index %d not found. Returning empty array."), Index);
		return TArray<uint8>();
	}
	if (CaptureCineCameraComponents[Index].CaptureComponents.Num() == 0)
	{
		UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return TArray<uint8>();
	}
	USceneCaptureComponent2D* CaptureComponent = CaptureCineCameraComponents[Index].CaptureComponents.FindRef(Mode);

	TArray<uint8> NpyData;
	if (CaptureComponent == nullptr) {
	  	UE_LOG(LogUnrealCV, Error, TEXT("Component for mode %s not found. Returning empty array."), *Mode);
		return NpyData;
  	}

	CaptureComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	UTextureRenderTarget2D* RenderTarget = CaptureComponent->TextureTarget;
	int32 Width = RenderTarget->SizeX, Height = RenderTarget->SizeY;
	TArray<FFloat16Color> ImageData;
	FTextureRenderTargetResource* RenderTargetResource;
	ImageData.AddUninitialized(Width * Height);
	RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	RenderTargetResource->ReadFloat16Pixels(ImageData);

	// Check the byte order of data
	// Compress image data to npy array
	// Generate a header for the numpy array
	NpyData = NpySerialization(ImageData, Width, Height, Channels);

	return NpyData;
}

USceneCaptureComponent2D* UGTCaptureComponent::GetCameraComponentCaptureComponent(FString Mode, int32 Index)
{
	check(CaptureCameraComponents.Num() > Index);
	check(CaptureCameraComponents[Index].CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureCameraComponents[Index].CaptureComponents.FindRef(Mode);
    return CaptureComponent;
}

USceneCaptureComponent2D* UGTCaptureComponent::GetCineCameraComponentCaptureComponent(FString Mode, int32 Index)
{
	check(CaptureCineCameraComponents.Num() > Index);
	check(CaptureCineCameraComponents[Index].CaptureComponents.Num() != 0);
	USceneCaptureComponent2D* CaptureComponent = CaptureCineCameraComponents[Index].CaptureComponents.FindRef(Mode);
    return CaptureComponent;
}