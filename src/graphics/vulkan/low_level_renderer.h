#ifndef MGE_GRAPHICS_VULKAN_LOW_LEVEL_RENDERER_H
#define MGE_GRAPHICS_VULKAN_LOW_LEVEL_RENDERER_H

#include <mgl/error.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	typedef struct mgl_vulkan_low_level_renderer_t mgl_vulkan_low_level_renderer_t;

	typedef struct
	{
		void* allocator;
		void* window;
		mgl_bool_t shadows_on;
		void(*error_callback)(const mgl_chr8_t* error_string);
	} mgl_vulkan_low_level_renderer_info_t;

	/// <summary>
	///		Creates a new vulkan low level renderer.
	/// </summary>
	/// <param name="info">Creation info</param>
	/// <returns>Renderer handle</returns>
	mgl_vulkan_low_level_renderer_t* mgl_create_vulkan_low_level_renderer(const mgl_vulkan_low_level_renderer_info_t* info);

	/// <summary>
	///		Destroys a vulkan low level renderer.
	/// </summary>
	/// <param name="renderer">Renderer handle</param>
	void mgl_destroy_vulkan_low_level_renderer(mgl_vulkan_low_level_renderer_t* renderer);

	/// <summary>
	///		Renders a frame.
	/// </summary>
	/// <param name="renderer">Renderer handle</param>
	/// <param name="frame">Frame data</param>
	void mgl_vulkan_low_level_renderer_render(mgl_vulkan_low_level_renderer_t* renderer, const void* frame);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MGE_GRAPHICS_VULKAN_LOW_LEVEL_RENDERER_H
