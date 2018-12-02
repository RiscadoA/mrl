#include <mgl/stream/stream.h>
#include <mgl/input/windows_window.h>
#include <mgl/entry.h>
#include <Windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

static mgl_bool_t running = MGL_TRUE;
static mgl_windows_window_t window;
static VkInstance vk_instance = VK_NULL_HANDLE;
static VkPhysicalDevice vk_physical_device = VK_NULL_HANDLE;
static VkDevice vk_device = VK_NULL_HANDLE;
static VkQueue vk_graphics_queue = VK_NULL_HANDLE;
static VkQueue vk_present_queue = VK_NULL_HANDLE;
static VkSurfaceKHR vk_surface = VK_NULL_HANDLE;

void create_instance(void)
{
	VkApplicationInfo app_info;
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = u8"Hello Triangle";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = u8"Magma";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;
	app_info.pNext = NULL;

	VkInstanceCreateInfo create_info;
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pNext = NULL;
	create_info.flags = 0;
	create_info.pApplicationInfo = &app_info;
	const mgl_chr8_t* layers[] = { "VK_LAYER_LUNARG_standard_validation" };
	create_info.ppEnabledLayerNames = layers;
	create_info.enabledLayerCount = sizeof(layers) / sizeof(*layers);
	const mgl_chr8_t* extensions[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	create_info.ppEnabledExtensionNames = extensions;
	create_info.enabledExtensionCount = sizeof(extensions) / sizeof(*extensions);

	VkResult result = vkCreateInstance(&create_info, NULL, &vk_instance);
	if (result != VK_SUCCESS)
		mgl_abort();
}

typedef struct
{
	mgl_u32_t graphics_family;
	mgl_u32_t present_family;
	mgl_bool_t complete;
} queue_families_t;

void find_queue_families(VkPhysicalDevice device, queue_families_t* out)
{
	mgl_u32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);
	MGL_ASSERT(queue_family_count <= 16);
	VkQueueFamilyProperties queue_families[16];
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);
	
	out->complete = MGL_FALSE;
	out->graphics_family = 16;
	out->present_family = 16;

	for (mgl_u32_t i = 0; i < queue_family_count; ++i)
	{
		VkBool32 present_support = MGL_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, vk_surface, &present_support);
		if (queue_families[i].queueCount > 0 && present_support)
			out->present_family = i;
		if (queue_families[i].queueCount > 0 && (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
			out->graphics_family = i;

		if (out->graphics_family != 16 && out->present_family != 16)
		{
			out->complete = MGL_TRUE;
			break;
		}
	}
}

mgl_bool_t is_device_suitable(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(device, &features);

	queue_families_t queue_families;
	find_queue_families(device, &queue_families);

	return
		properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		features.geometryShader &&
		queue_families.complete;
}

void pick_physical_device(void)
{
	mgl_u32_t device_count = 0;
	VkResult result = vkEnumeratePhysicalDevices(vk_instance, &device_count, NULL);
	MGL_ASSERT(result == VK_SUCCESS);
	MGL_ASSERT(device_count >= 1 && device_count <= 16);

	VkPhysicalDevice physical_devices[16];
	result = vkEnumeratePhysicalDevices(vk_instance, &device_count, physical_devices);
	MGL_ASSERT(result == VK_SUCCESS);

	vk_physical_device = NULL;
	for (mgl_u32_t i = 0; i < device_count; ++i)
		if (is_device_suitable(physical_devices[i]))
		{
			vk_physical_device = physical_devices[i];
			break;
		}
	MGL_ASSERT(vk_physical_device != NULL);
}

void create_logical_device(void)
{	
	queue_families_t queue_families;
	find_queue_families(vk_physical_device, &queue_families);

	mgl_u64_t device_queue_create_infos_count = 0;
	VkDeviceQueueCreateInfo device_queue_create_infos[2];

	{
		mgl_f32_t priorities[] = { 1.0f };
		
		if (queue_families.graphics_family == queue_families.present_family)
		{
			device_queue_create_infos_count = 1;

			device_queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			device_queue_create_infos[0].pNext = NULL;
			device_queue_create_infos[0].flags = 0;
			device_queue_create_infos[0].queueFamilyIndex = queue_families.graphics_family;
			device_queue_create_infos[0].queueCount = 1;
			device_queue_create_infos[0].pQueuePriorities = priorities;
		}
		else
		{
			device_queue_create_infos_count = 2;

			device_queue_create_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			device_queue_create_infos[0].pNext = NULL;
			device_queue_create_infos[0].flags = 0;
			device_queue_create_infos[0].queueFamilyIndex = queue_families.graphics_family;
			device_queue_create_infos[0].queueCount = 1;
			device_queue_create_infos[0].pQueuePriorities = priorities;

			device_queue_create_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			device_queue_create_infos[1].pNext = NULL;
			device_queue_create_infos[1].flags = 0;
			device_queue_create_infos[1].queueFamilyIndex = queue_families.present_family;
			device_queue_create_infos[1].queueCount = 1;
			device_queue_create_infos[1].pQueuePriorities = priorities;
		}
	}

	VkDeviceCreateInfo device_create_info;
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.pNext = NULL;
	device_create_info.flags = 0;
	device_create_info.queueCreateInfoCount = device_queue_create_infos_count;
	device_create_info.pQueueCreateInfos = device_queue_create_infos;
	const mgl_chr8_t* layers[] = { "VK_LAYER_LUNARG_standard_validation" };
	device_create_info.ppEnabledLayerNames = layers;
	device_create_info.enabledLayerCount = sizeof(layers) / sizeof(*layers);
	const mgl_chr8_t* extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	device_create_info.ppEnabledExtensionNames = extensions;
	device_create_info.enabledExtensionCount = sizeof(extensions) / sizeof(*extensions);
	device_create_info.pEnabledFeatures = NULL;

	VkResult result = vkCreateDevice(vk_physical_device, &device_create_info, NULL, &vk_device);
	MGL_ASSERT(result == VK_SUCCESS);

	vkGetDeviceQueue(vk_device, queue_families.graphics_family, 0, &vk_graphics_queue);
	vkGetDeviceQueue(vk_device, queue_families.present_family, 0, &vk_present_queue);
}

void create_surface(void)
{
	VkWin32SurfaceCreateInfoKHR create_info;
	create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	create_info.pNext = NULL;
	create_info.flags = 0;
	create_info.hwnd = (HWND)window.hwnd;
	create_info.hinstance = GetModuleHandle(NULL);

	PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(vk_instance, "vkCreateWin32SurfaceKHR");
	MGL_ASSERT(vkCreateWin32SurfaceKHR != NULL);
	VkResult result = vkCreateWin32SurfaceKHR(vk_instance, &create_info, NULL, &vk_surface);
	MGL_ASSERT(result == VK_SUCCESS);
} 

void init_vulkan(void)
{
	create_instance();
	create_surface();
	pick_physical_device();
	create_logical_device();
}

void main_loop(void)
{

}

void cleanup_vulkan(void)
{
	vkDestroyDevice(vk_device, NULL);
	vkDestroySurfaceKHR(vk_instance, vk_surface, NULL);
	vkDestroyInstance(vk_instance, NULL);
}

void on_close(void* window)
{
	running = MGL_FALSE;
}

int main(int argc, char** argv)
{
	// Init MGL
	if (mgl_init() != MGL_ERROR_NONE)
		return 1;

	// Open window and set callbacks
	{
		mgl_windows_window_settings_t settings;
		settings.title = u8"Hello Triangle";
		settings.width = 800;
		settings.height = 646;
		settings.mode = MGL_WINDOW_MODE_WINDOWED;
		mgl_open_windows_window(&window, &settings);
		mgl_set_window_on_close_callback(&window, &on_close);
	}

	// Init vulkan
	init_vulkan();

	// Main loop
	while (running)
	{
		mgl_poll_window_events(&window);
		main_loop();
	}

	// Terminate vulkan
	cleanup_vulkan();

	// Close window
	mgl_close_windows_window(&window);

	// Terminate MGL
	mgl_terminate();
	return 0;
}