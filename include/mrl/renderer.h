#ifndef MRL_RENDERER_H
#define MRL_RENDERER_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <mrl/render_device.h>
#include <mrl/mesh.h>
#include <mrl/effect.h>
#include <mrl/frame.h>

	typedef struct mrl_renderer_t mrl_renderer_t;
	struct mrl_renderer_t
	{
		mrl_error_t(*create_mesh)(mrl_renderer_t* r, const mrl_mesh_desc_t* desc, mgl_u32_t* out_id);
		void(*destroy_mesh)(mrl_renderer_t* r, mgl_u32_t mesh);

		mrl_error_t(*create_effect)(mrl_renderer_t* r, const mrl_effect_desc_t* desc, mgl_u32_t* out_id);
		void(*destroy_effect)(mrl_renderer_t* r, mgl_u32_t effect);

		void(*render_frame)(mrl_renderer_t* r, const mrl_frame_t* frame);

		const mgl_chr8_t*(*get_type)(mrl_renderer_t* r);
	};

	/// <summary>
	///		Creates a new mesh.
	/// </summary>
	/// <param name="r">Renderer handle</param>
	/// <param name="desc">Mesh description</param>
	/// <param name="out_id">Out mesh ID</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_mesh(mrl_renderer_t* r, const mrl_mesh_desc_t* desc, mgl_u32_t* out_id);

	/// <summary>
	///		Destroys an effect.
	/// </summary>
	/// <param name="r">Renderer handle</param>
	/// <param name="effect">Effect ID</param>
	MRL_API void mrl_destroy_mesh(mrl_renderer_t* r, mgl_u32_t effect);

	/// <summary>
	///		Creates a new effect.
	/// </summary>
	/// <param name="r">Renderer handle</param>
	/// <param name="desc">Effect description</param>
	/// <param name="out_id">Out effect ID</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_effect(mrl_renderer_t* r, const mrl_effect_desc_t* desc, mgl_u32_t* out_id);

	/// <summary>
	///		Destroys an effect.
	/// </summary>
	/// <param name="r">Renderer handle</param>
	/// <param name="effect">Effect ID</param>
	MRL_API void mrl_destroy_effect(mrl_renderer_t* r, mgl_u32_t effect);

	/// <summary>
	///		Renders a frame.
	/// </summary>
	/// <param name="r">Renderer handle</param>
	/// <param name="frame">Pointer to frame data</param>
	MRL_API void mrl_render_frame(mrl_renderer_t* r, const mrl_frame_t* frame);

	/// <summary>
	///		Gets a renderer's type name.
	/// </summary>
	/// <param name="r">Renderer handle</param>
	/// <returns>Type name</returns>
	MRL_API const mgl_chr8_t* mrl_get_renderer_type(mrl_renderer_t* r);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif
