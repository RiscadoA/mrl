#ifndef MRL_OGL330_LL_RENDERER_H
#define MRL_OGL330_LL_RENDERER_H

#include <mgl/type.h>
#include <mrl/api_utils.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	typedef struct mrl_ogl330_ll_renderer_t mrl_ogl330_ll_renderer_t;
	typedef struct
	{
		void* allocator; // Allocator used in the low level renderer.
		void* window; // Window where the renderer will render into.
		void(*error_callback)(const mgl_chr8_t* error); // Error callback (optional, can be NULL)
		void(*warning_callback)(const mgl_chr8_t* msg); // Warning callback (optional, can be NULL)
		void(*verbose_callback)(const mgl_chr8_t* msg); // Verbose callback (optional, can be NULL)
	} mrl_ogl330_ll_renderer_creation_info_t;

	/// <summary>
	///		Creates a new low level OpenGL 3.3 renderer.
	/// </summary>
	/// <param name="info">Renderer info</param>
	/// <returns>Renderer handle</returns>
	mrl_ogl330_ll_renderer_t* MRL_API mrl_create_ogl330_ll_renderer(const mrl_ogl330_ll_renderer_creation_info_t* info);
	
	/// <summary>
	///		Destroys a low level OpenGL 3.3 renderer.
	/// </summary>
	/// <param name="renderer">Renderer handle</param>
	void MRL_API mrl_destroy_ogl330_ll_renderer(mrl_ogl330_ll_renderer_t* renderer);

	/// <summary>
	///		Renders a frame package.
	/// </summary>
	/// <param name="renderer">Renderer handle</param>
	/// <param name="frame_package">Frame package</param>
	void MRL_API mrl_ogl330_ll_render(mrl_ogl330_ll_renderer_t* renderer, const mgl_u8_t* frame_package);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MRL_OGL330_LL_RENDERER_H
