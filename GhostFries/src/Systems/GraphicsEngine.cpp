#define VK_USE_PLATFORM_WIN32_KHR
#include "Systems/GraphicsEngine.h"
#include "Systems/YamlDoc.h"
#include "Engine/Logger.h"
#include "Utility/StringUtils.h"

#ifdef _DEBUG
constexpr bool VulkanValidationLayers = true;
#else
constexpr bool VulkanValidationLayers = false;
#endif

#pragma comment(lib, "d3d11")

GraphicsEngine::GraphicsEngine() : mInstance{nullptr}, mDebugMessenger{nullptr}, mPhysicalDevice{nullptr},
									mDevice{nullptr}, mQueue{nullptr}, mSurface{nullptr}, mSwapchain{nullptr},
									mState { 640, 480, false, false }
{
}

GraphicsEngine::~GraphicsEngine()
{
}

bool GraphicsEngine::Initialize(HWND hWnd)
{
	if (CreateInstance() ||
		CreateSurface(hWnd) ||
		SelectPhysicalDevice() ||
		InitializeDevice() ||
		CreateSwapchain() ||
		CreateSwapchainViews())
	{
		Logger::Log("Initialized graphics engine");
		return true;
	}

	return false;
}

static VKAPI_ATTR vk::Bool32 VKAPI_CALL vulkanDebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	switch (severity)
	{
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
		{
			Logger::LogWarn("[Vulkan] ({}) {}", vk::to_string(type), pCallbackData->pMessage);
			break;
		}

		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
		{
			Logger::LogError("[Vulkan] ({}) {}", vk::to_string(type), pCallbackData->pMessage);
			break;
		}

		default:
			break;
	}

	return vk::False;
}

bool GraphicsEngine::CreateInstance()
{
	std::vector<const char*> layers;
	std::vector extensions
	{
		"VK_KHR_surface",
		"VK_KHR_win32_surface",
		"VK_KHR_get_physical_device_properties2",
		"VK_KHR_portability_enumeration",
	};
	if constexpr (VulkanValidationLayers)
	{
		layers.push_back("VK_LAYER_KHRONOS_validation");
		extensions.push_back("VK_EXT_debug_utils");
	}

	vk::InstanceCreateInfo instanceCreateInfo
	{
		.flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
		.enabledLayerCount = static_cast<uint32_t>(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data(),
	};
	auto instanceRV = mContext.createInstance(instanceCreateInfo);
	if (!instanceRV.has_value())
	{
		Logger::LogError("Graphics: Failed to create Vulkan instance: {}", vk::to_string(instanceRV.result));
		return false;
	}
	mInstance = std::move(instanceRV.value);

	if constexpr (VulkanValidationLayers)
	{
		vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
		vk::DebugUtilsMessageTypeFlagsEXT typeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
		vk::DebugUtilsMessengerCreateInfoEXT messengerCreateInfo
		{
			.messageSeverity = severityFlags,
			.messageType = typeFlags,
			.pfnUserCallback = &vulkanDebugCallback,
		};
		auto messengerRV = mInstance.createDebugUtilsMessengerEXT(messengerCreateInfo);
		if (!messengerRV.has_value())
		{
			Logger::LogError("Graphics: Failed to create Vulkan debug messenger: {}", vk::to_string(messengerRV.result));
			return false;
		}
		mDebugMessenger = std::move(messengerRV.value);
	}
	return true;
}

bool GraphicsEngine::CreateSurface(HWND hWnd)
{
	vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo
	{
		.hinstance = GetModuleHandle(NULL),
		.hwnd = hWnd,
	};
	auto surfaceRV = mInstance.createWin32SurfaceKHR(surfaceCreateInfo);
	if (!surfaceRV.has_value())
	{
		Logger::LogError("Graphics: Failed to create surface: {}", vk::to_string(surfaceRV.result));
		return false;
	}
	mSurface = std::move(surfaceRV.value);
	return true;
}

bool GraphicsEngine::SelectPhysicalDevice()
{
	auto physicalDevicesRV = mInstance.enumeratePhysicalDevices();
	if (!physicalDevicesRV.has_value())
	{
		Logger::LogError("Graphics: Failed to enumerate physical devices: {}", vk::to_string(physicalDevicesRV.result));
		return false;
	}
	auto& physicalDevices = physicalDevicesRV.value;
	if (physicalDevices.empty())
	{
		Logger::LogError("Graphics: No Vulkan devices available");
		return false;
	}

	std::vector requiredExtensions
	{
		vk::KHRSwapchainExtensionName,
	};
	auto deviceIt = std::ranges::find_if(physicalDevices, [&](auto& device)
	{
		auto properties = device.getProperties();
		bool supportsApi = properties.apiVersion >= vk::ApiVersion13;

		auto queueFamilies = device.getQueueFamilyProperties();
		bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& property)
		{
			return !!(property.queueFlags & vk::QueueFlagBits::eGraphics);
		});

		auto availableExtensionsRV = device.enumerateDeviceExtensionProperties();
		if (!availableExtensionsRV.has_value())
		{
			Logger::LogWarn("Graphics: Failed to enumerate device extension properties: {}", vk::to_string(availableExtensionsRV.result));
			return false;
		}
		bool supportsExtensions = std::ranges::all_of(requiredExtensions, [&](auto& requiredExtension)
		{
			return std::ranges::any_of(availableExtensionsRV.value, [&](auto& availableExtension)
			{
				return std::string_view(requiredExtension) == std::string_view(availableExtension.extensionName);
			});
		});

		auto features = device.template getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
		bool supportsFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters && features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering && features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

		return supportsApi && supportsGraphics && supportsExtensions && supportsFeatures;
	});
	if (deviceIt == physicalDevices.end())
	{
		Logger::LogError("Graphics: No compatible device available");
		return false;
	}
	mPhysicalDevice = std::move(*deviceIt);
	return true;
}

bool GraphicsEngine::InitializeDevice()
{
	auto queueFamilyProperties = mPhysicalDevice.getQueueFamilyProperties();

	uint32_t graphicsIndex = ~0;
	for (uint32_t index = 0; index < queueFamilyProperties.size(); ++index)
	{
		auto& property = queueFamilyProperties[index];
		if (property.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			auto supportKV = mPhysicalDevice.getSurfaceSupportKHR(index, mSurface);
			if (supportKV.has_value() && supportKV.value)
			{
				graphicsIndex = index;
				break;
			}
		}
	}
	if (graphicsIndex == ~0)
	{
		Logger::LogError("Graphics: No device with compatible queue family available");
		return false;
	}

	float queuePriority = 1.0f;
	vk::DeviceQueueCreateInfo queueCreateInfo
	{
		.queueFamilyIndex = graphicsIndex,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority,
	};

	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain
	{
		{},
		{ .shaderDrawParameters = true, },
		{ .dynamicRendering = true, },
		{ .extendedDynamicState = true, },
	};

	std::vector requiredExtensions
	{
		vk::KHRSwapchainExtensionName,
	};
	vk::DeviceCreateInfo deviceCreateInfo
	{
		.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queueCreateInfo,
		.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
		.ppEnabledExtensionNames = requiredExtensions.data(),
	};
	auto deviceRV = mPhysicalDevice.createDevice(deviceCreateInfo);
	if (!deviceRV.has_value())
	{
		Logger::LogError("Graphics: Failed to create logical device: {}", vk::to_string(deviceRV.result));
		return false;
	}
	mDevice = std::move(deviceRV.value);
	mQueue = mDevice.getQueue(graphicsIndex, 0);
	return true;
}

bool GraphicsEngine::CreateSwapchain()
{
	auto formatsRV = mPhysicalDevice.getSurfaceFormatsKHR(mSurface);
	if (!formatsRV.has_value())
	{
		Logger::LogError("Graphics: Failed to get surface formats: {}", vk::to_string(formatsRV.result));
		return false;
	}
	auto formatIt = std::ranges::find_if(formatsRV.value, [](auto& format)
	{
		return format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
	});
	if (formatIt == formatsRV.value.end())
	{
		Logger::LogError("Graphics: No compatible surface format available");
		return false;
	}

	auto presentModesRV = mPhysicalDevice.getSurfacePresentModesKHR(mSurface);
	if (!presentModesRV.has_value())
	{
		Logger::LogError("Graphics: Failed to get present modes: {}", vk::to_string(presentModesRV.result));
		return false;
	}
	auto presentMode = std::ranges::any_of(presentModesRV.value, [](auto& mode)
	{
		return mode == vk::PresentModeKHR::eImmediate;
	}) ? vk::PresentModeKHR::eImmediate : vk::PresentModeKHR::eFifo;

	auto capabilitiesRV = mPhysicalDevice.getSurfaceCapabilitiesKHR(mSurface);
	if (!capabilitiesRV.has_value())
	{
		Logger::LogError("Graphics: Failed to get surface capabilities: {}", vk::to_string(capabilitiesRV.result));
		return false;
	}
	auto& capabilities = capabilitiesRV.value;

	auto extent = capabilities.currentExtent;
	if (extent.width == ~0)
	{
		extent = {
			.width = std::clamp<uint32_t>(mState.mWidth, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			.height = std::clamp<uint32_t>(mState.mHeight, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
		};
	}
	auto imageCount = std::max(2u, capabilities.minImageCount);
	if (capabilities.maxImageCount > 0)
		imageCount = std::min(imageCount, capabilities.maxImageCount);
	vk::SwapchainCreateInfoKHR swapchainCreateInfo
	{
		.surface = mSurface,
		.minImageCount = imageCount,
		.imageFormat = formatIt->format,
		.imageColorSpace = formatIt->colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.preTransform = capabilities.currentTransform,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = presentMode,
		.clipped = true,
	};
	auto swapchainRV = mDevice.createSwapchainKHR(swapchainCreateInfo);
	if (!swapchainRV.has_value())
	{
		Logger::LogError("Graphics: Failed to create swapchain: {}", vk::to_string(swapchainRV.result));
		return false;
	}
	mSwapchain = std::move(swapchainRV.value);
	mSwapchainExtent = extent;
	mSwapchainSurfaceFormat = *formatIt;
	return true;
}

bool GraphicsEngine::CreateSwapchainViews()
{
	auto imagesRV = mSwapchain.getImages();
	if (!imagesRV.has_value())
	{
		Logger::LogError("Graphics: Failed to get swapchain images: {}", vk::to_string(imagesRV.result));
		return false;
	}
	mSwapchainImages = std::move(imagesRV.value);

	vk::ImageViewCreateInfo imageViewCreateInfo
	{
		.viewType = vk::ImageViewType::e2D,
		.format = mSwapchainSurfaceFormat.format,
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};

	for (auto& image : mSwapchainImages)
	{
		imageViewCreateInfo.image = image;
		auto imageViewRV = mDevice.createImageView(imageViewCreateInfo);
		if (!imageViewRV.has_value())
		{
			Logger::LogError("Graphics: Failed to get swapchain image views: {}", vk::to_string(imageViewRV.result));
			return false;
		}
		mSwapchainImageViews.emplace_back(std::move(imageViewRV.value));
	}
	return true;
}

void GraphicsEngine::Shutdown()
{
	//TODO: queue is destroyed with device
	if (mpTargetView)
	{
		mpTargetView->Release();
		mpTargetView = nullptr;
	}

	if (mpSwapChain)
	{
		mpSwapChain->Release();
		mpSwapChain = nullptr;
	}

	if (mpContext)
	{
		mpContext->Release();
		mpContext = nullptr;
	}

	if (mpDevice)
	{
		mpDevice->Release();
		mpDevice = nullptr;
	}

	Logger::Log("Graphics engine shut down");
}

void GraphicsEngine::Render()
{
	static float background[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mpContext->ClearRenderTargetView(mpTargetView, background);
	mpContext->OMSetRenderTargets(1, &mpTargetView, nullptr);

	if (mRenderEvent)
		mRenderEvent();

	auto sync = mState.mVSync ? 1 : 0;
	mpSwapChain->Present(sync, 0);
}

void GraphicsEngine::LoadConfig(YamlDoc& config)
{
	if (mpContext)
	{
		Logger::LogError("Graphics: Cannot load config - context already exists");
		return;
	}

	auto display = config["display"];
	display.SetMap();

	auto width = display["width"].GetUInt();
	auto height = display["height"].GetUInt();
	bool fullscreen = display["fullscreen"].GetBool();
	bool vsync = display["vertical_sync"].GetBool(true);

	//width and height must be valid display mode
	if (width < 640 || height < 480)
	{
		width = 640;
		height = 480;
	}

	mState.mWidth = width;
	mState.mHeight = height;
	mState.mFullScreen = fullscreen;
	mState.mVSync = vsync;
}

static bool ResizeWindow(GraphicsEngine::DisplayState& displayState, IDXGISwapChain* pSwapChain)
{
	DXGI_MODE_DESC desc
	{
		.Width = displayState.mWidth,
		.Height = displayState.mHeight,
		.RefreshRate = { 60, 1 },
		.Format = DXGI_FORMAT_UNKNOWN,
	};
	auto hResult = pSwapChain->ResizeTarget(&desc);
	if (FAILED(hResult) && hResult != DXGI_STATUS_MODE_CHANGE_IN_PROGRESS)
	{
		Logger::LogError("Graphics: Failed to resize target window: {}", StringUtils::Format(hResult));
		return false;
	}

	return true;
}

bool GraphicsEngine::SetDisplayDimensions(unsigned int width, unsigned int height)
{
	Logger::Log("Graphics: Changing display dims to {}x{}", width, height);

	ShutdownTargetView();

	auto hResult = mpSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hResult))
	{
		Logger::LogError("Graphics: Failed to resize swap chain buffers: {}", StringUtils::Format(hResult));
		Logger::Log("Graphics: Recreating context for display dims {}x{}", mState.mWidth, mState.mHeight);
	}
	else
	{
		mState.mWidth = width;
		mState.mHeight = height;
	}

	if (!InitializeTargetView())
	{
		Logger::LogError("Graphics: Failed to recreate render context for new display state");
		return false;
	}

	if (!mState.mFullScreen && !ResizeWindow(mState, mpSwapChain))
		return false;

	return true;
}

void GraphicsEngine::SetVerticalSync(bool vsync)
{
	mState.mVSync = vsync;
}
