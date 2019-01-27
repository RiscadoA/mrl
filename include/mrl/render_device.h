#ifndef MRL_RENDER_DEVICE_H
#define MRL_RENDER_DEVICE_H
#ifdef __cplusplus
extern "C" {
#endif 

#include <mrl/error.h>

	enum
	{
		MRL_VSYNC_OFF,
		MRL_VSYNC_ON,
		MRL_VSYNC_ADAPTIVE,
	};

	typedef struct mrl_render_device_desc_t mrl_render_device_desc_t;
	struct mrl_render_device_desc_t
	{
		void* allocator;
		void* window;
		mgl_enum_t vsync_mode; // MRL_VSYNC_OFF; MRL_VSYNC_ON; MRL_VSYNC_ADAPTIVE.
	};

#define MRL_DEFAULT_RENDER_DEVICE_DESC ((mrl_render_device_desc_t) {\
	NULL,\
	NULL,\
	MRL_VSYNC_ADAPTIVE,\
})

	typedef struct mrl_render_device_t mrl_render_device_t;
	struct mrl_render_device_t
	{
		// -------- Draw functions --------
		void(*clear_color)(mrl_render_device_t* rd, mgl_f32_t r, mgl_f32_t g, mgl_f32_t b, mgl_f32_t a);
		void(*clear_depth)(mrl_render_device_t* rd, mgl_f32_t depth);
		void(*clear_stencil)(mrl_render_device_t* rd, mgl_i32_t stencil);
		void(*swap_buffers)(mrl_render_device_t* rd);

		// ----------- Getters -----------
		const mgl_chr8_t*(*get_type_name)(mrl_render_device_t* rd);
	};

	/// <summary>
	///		Clears the currently bound color buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="r">New red value</param>
	/// <param name="g">New green value</param>
	/// <param name="b">New blue value</param>
	/// <param name="a">New alpha value</param>
	MRL_API void mrl_clear_color(mrl_render_device_t* rd, mgl_f32_t r, mgl_f32_t g, mgl_f32_t b, mgl_f32_t a);

	/// <summary>
	///		Clears the currently bound depth buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="depth">New depth value</param>
	MRL_API void mrl_clear_depth(mrl_render_device_t* rd, mgl_f32_t depth);

	/// <summary>
	///		Clears the currently bound stencil buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="stencil">New stencil value</param>
	MRL_API void mrl_clear_stencil(mrl_render_device_t* rd, mgl_i32_t stencil);

	/// <summary>
	///		Swaps the buffers of a render device, displaying the results to the screen.
	/// </summary>
	/// <param name="rd">Render device</param>
	MRL_API void mrl_swap_buffers(mrl_render_device_t* rd);

	/// <summary>
	///		Gets the type name of a render device.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <returns>Render device type name</returns>
	MRL_API const mgl_chr8_t* mrl_get_type_name(mrl_render_device_t* rd);

#ifdef __cplusplus
}
#endif
#endif