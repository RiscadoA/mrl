#ifndef MRL_VULKAN_LL_RENDERER_H
#define MRL_VULKAN_LL_RENDERER_H

#include <mgl/type.h>
#include <mrl/api_utils.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	typedef struct mrl_vkll_renderer_t mrl_vkll_renderer_t;
	typedef struct
	{
		void* allocator; // Allocator used in the low level renderer.
		void* window; // Window where the renderer will render into.
		void(*error_callback)(const mgl_chr8_t* error); // Error callback (optional, can be NULL)
		void(*warning_callback)(const mgl_chr8_t* msg); // Warning callback (optional, can be NULL)
		void(*verbose_callback)(const mgl_chr8_t* msg); // Verbose callback (optional, can be NULL)
	} mrl_vkll_renderer_creation_info_t;

	/// <summary>
	///		Creates a new low level vulkan renderer.
	/// </summary>
	/// <param name="info">Renderer info</param>
	/// <returns>Renderer handle</returns>
	mrl_vkll_renderer_t* MRL_API mrl_create_vkll_renderer(const mrl_vkll_renderer_creation_info_t* info);
	
	/// <summary>
	///		Destroys a low level vulkan renderer.
	/// </summary>
	/// <param name="renderer">Renderer handle</param>
	void MRL_API mrl_destroy_vkll_renderer(mrl_vkll_renderer_t* renderer);

	/// <summary>
	///		Renders a frame package.
	/// </summary>
	/// <param name="renderer">Renderer handle</param>
	/// <param name="frame_package">Frame package</param>
	void MRL_API mrl_vkll_render(mrl_vkll_renderer_t* renderer, const mgl_u8_t* frame_package);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MRL_VULKAN_LL_RENDERER_H
