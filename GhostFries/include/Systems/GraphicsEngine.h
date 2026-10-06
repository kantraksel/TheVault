#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan_raii.hpp>
#include "Utility/Function.h"

typedef struct HWND__* HWND;

class GraphicsEngine
{
private:
	vk::raii::Context mContext;
	vk::raii::Instance mInstance;
	vk::raii::DebugUtilsMessengerEXT mDebugMessenger;
	vk::raii::PhysicalDevice mPhysicalDevice;
	vk::raii::Device mDevice;
	vk::raii::Queue mQueue;
	vk::raii::SurfaceKHR mSurface;
	vk::raii::SwapchainKHR mSwapchain;
	std::vector<vk::Image> mSwapchainImages;
	std::vector<vk::raii::ImageView> mSwapchainImageViews;
	vk::SurfaceFormatKHR mSwapchainSurfaceFormat;
	vk::Extent2D mSwapchainExtent;
	Function<void()> mRenderEvent;

public:
	struct DisplayState
	{
		unsigned int mWidth;
		unsigned int mHeight;
		bool mFullScreen; //TODO: unused
		bool mVSync;
	};

	bool CreateInstance();
	bool CreateSurface(HWND hWnd);
	bool SelectPhysicalDevice();
	bool InitializeDevice();
	bool CreateSwapchain();
	bool CreateSwapchainViews();

private:
	DisplayState mState;

public:
	GraphicsEngine();
	~GraphicsEngine();

	bool Initialize(HWND hWnd);
	void Shutdown();
	void LoadConfig(struct YamlDoc& config);

	void Render();
	auto& GetInstance() { return mInstance; }
	auto& GetPhysicalDevice() { return mPhysicalDevice; }
	auto& GetLogicalDevice() { return mDevice; }
	auto& GetQueue() { return mQueue; }
	auto& GetRenderEvent() { return mRenderEvent; }

	bool SetDisplayDimensions(unsigned int width, unsigned int height);
	void SetVerticalSync(bool vsync);
};
