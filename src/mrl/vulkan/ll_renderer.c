#ifdef MRL_BUILD_VULKAN

#include <mrl/vulkan/ll_renderer.h>
#include <mgl/error.h>
#include <mgl/memory/allocator.h>
#include <mgl/stream/buffer_stream.h>
#include <vulkan/vulkan.h>

#ifdef _WIN32
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif // _WIN32

struct mrl_vkll_renderer_t
{
	void* allocator;
	void* window;
	void(*error_callback)(const mgl_chr8_t* error);
	void(*warning_callback)(const mgl_chr8_t* msg);
	void(*verbose_callback)(const mgl_chr8_t* msg);

	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physical_device;
	VkDevice device;
	VkDebugUtilsMessengerEXT callback;
	VkAllocationCallbacks allocation_callbacks;
};

static void* VKAPI_PTR vulkan_allocation_function(
	void* pUserData,
	size_t size,
	size_t alignment,
	VkSystemAllocationScope allocationScope)
{
	VkSystemAllocationScope scope;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	mrl_vkll_renderer_t* renderer = (mrl_vkll_renderer_t*)pUserData;

	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		if (renderer->error_callback != NULL)
			renderer->error_callback(pCallbackData->pMessage);
	}
	else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		if (renderer->warning_callback != NULL)
			renderer->warning_callback(pCallbackData->pMessage);
	}
	else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		if (renderer->verbose_callback != NULL)
			renderer->verbose_callback(pCallbackData->pMessage);
	}
	return VK_FALSE;
}

static mgl_bool_t init_vulkan_instance(mrl_vkll_renderer_t* renderer)
{
	// Create Vulkan Instance
	VkInstanceCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pNext = NULL;
	create_info.flags = 0;
	create_info.pApplicationInfo = NULL;
	const mgl_chr8_t* layers[] = {
#ifdef _DEBUG
		"VK_LAYER_LUNARG_standard_validation",
#endif // _DEBUG
		""
	};
	create_info.ppEnabledLayerNames = layers;
	create_info.enabledLayerCount = sizeof(layers) / sizeof(*layers) - 1;
	const mgl_chr8_t* extensions[] = {
#ifdef _DEBUG
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif // _DEBUG
		VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif // _WIN32
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};
	create_info.ppEnabledExtensionNames = extensions;
	create_info.enabledExtensionCount = sizeof(extensions) / sizeof(*extensions);

	VkResult result = vkCreateInstance(&create_info, &renderer->allocation_callbacks, &renderer->instance);
	// Check error
	if (result == VK_ERROR_INCOMPATIBLE_DRIVER)
	{
		if (renderer->error_callback != NULL)
			renderer->error_callback(u8"Failed to create Vulkan instance: vkCreateInstance failed, incompatible driver");
		return MGL_FALSE;
	}
	else if (result != VK_SUCCESS)
	{
		if (renderer->error_callback != NULL)
			renderer->error_callback(u8"Failed to create Vulkan instance: vkCreateInstance failed");
		return MGL_FALSE;
	}

	// Add debug callback
#ifdef _DEBUG
	{
		VkDebugUtilsMessengerCreateInfoEXT create_info;
		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		create_info.pNext = NULL;
		create_info.flags = 0;
		create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = &vulkan_debug_callback;
		create_info.pUserData = renderer;

		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(renderer->instance, "vkCreateDebugUtilsMessengerEXT");
		if (vkCreateDebugUtilsMessengerEXT != NULL)
		{
			result = vkCreateDebugUtilsMessengerEXT(renderer->instance, &create_info, &renderer->allocation_callbacks, &renderer->callback);
			if (result != VK_SUCCESS)
			{
				if (renderer->error_callback != NULL)
					renderer->error_callback(u8"Failed to add Vulkan debug callback: messenger creation failed");
				vkDestroyInstance(renderer->instance, &renderer->allocation_callbacks);
				return MGL_FALSE;
			}
		}
		else
		{
			if (renderer->error_callback != NULL)
				renderer->error_callback(u8"Failed to add Vulkan debug callback: extension unsupported");
			vkDestroyInstance(renderer->instance, &renderer->allocation_callbacks);
			return MGL_FALSE;
		}
	}
#endif // _DEBUG

	return MGL_TRUE;
}

static void clean_vulkan_instance(mrl_vkll_renderer_t* renderer)
{
	MGL_DEBUG_ASSERT(renderer != NULL);
	PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(renderer->instance, "vkDestroyDebugUtilsMessengerEXT");
	if (vkDestroyDebugUtilsMessengerEXT != NULL)
		vkDestroyDebugUtilsMessengerEXT(renderer->instance, renderer->callback, &renderer->allocation_callbacks);
	vkDestroyInstance(renderer->instance, &renderer->allocation_callbacks);
}

static mgl_bool_t init_surface(mrl_vkll_renderer_t* renderer)
{
	return MGL_FALSE;
}

static void clean_surface(mrl_vkll_renderer_t* renderer)
{

}

static mgl_bool_t pick_physical_device(mrl_vkll_renderer_t* renderer)
{
	return MGL_FALSE;
}

static mgl_bool_t init_logical_device(mrl_vkll_renderer_t* renderer)
{
	return MGL_FALSE;
}

static void clean_logical_device(mrl_vkll_renderer_t* renderer)
{

}

static mgl_bool_t init_vulkan(mrl_vkll_renderer_t* renderer)
{
	if (!init_vulkan_instance(renderer))
		goto error1;
	if (!init_surface(renderer))
		goto error2;
	if (!pick_physical_device(renderer))
		goto error3;
	if (!init_logical_device(renderer))
		goto error3;
	return MGL_TRUE;

error3:
	clean_surface(renderer);
error2:
	clean_vulkan_instance(renderer);
error1:
	return MGL_FALSE;
}

mrl_vkll_renderer_t* MRL_API mrl_create_vkll_renderer(const mrl_vkll_renderer_creation_info_t * info)
{
	MGL_DEBUG_ASSERT(info != NULL && info->window != NULL);

	mrl_vkll_renderer_t* renderer;
	mgl_error_t e = mgl_allocate(info->allocator, sizeof(mrl_vkll_renderer_t), &renderer);
	if (e != MGL_ERROR_NONE)
	{
		if (info->error_callback != NULL)
		{
			mgl_chr8_t error_buf[4096] = { 0 };
			mgl_buffer_stream_t error_stream;
			mgl_init_buffer_stream(&error_stream, error_buf, sizeof(error_buf));
			mgl_print(&error_stream, u8"Failed to allocate mrl_vkll_renderer_t, mgl_allocate returned ");
			mgl_print(&error_stream, mgl_get_error_string(e));
			info->error_callback(error_buf);
		}
		return NULL;
	}

	renderer->allocator = info->allocator;
	renderer->window = info->window;
	renderer->error_callback = info->error_callback;
	renderer->warning_callback = info->warning_callback;
	renderer->verbose_callback = info->verbose_callback;

	if (!init_vulkan(renderer))
	{
		MGL_ASSERT(mgl_deallocate(renderer->allocator, renderer) == MGL_ERROR_NONE);
		return NULL;
	}

	return renderer;
}

void MRL_API mrl_destroy_vkll_renderer(mrl_vkll_renderer_t * renderer)
{
	MGL_DEBUG_ASSERT(renderer != NULL);

}

void MRL_API mrl_vkll_render(mrl_vkll_renderer_t * renderer, const mgl_u8_t * frame_package)
{
	MGL_DEBUG_ASSERT(renderer != NULL && frame_package != NULL);

}

#endif // MRL_BUILD_VULKAN