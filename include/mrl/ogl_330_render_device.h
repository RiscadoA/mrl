#ifndef MRL_OGL_330_RENDER_DEVICE_H
#define MRL_OGL_330_RENDER_DEVICE_H
#ifdef __cplusplus
extern "C" {
#endif 

#include <mrl/render_device.h>

	/// <summary>
	///		Initializes an OpenGL 3.3 render device.
	///		The typename of this render device is 'ogl_330'.
	/// </summary>
	/// <param name="desc">Render device description</param>
	/// <param name="out_rd">Out render device pointer</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_init_ogl_330_render_device(const mrl_render_device_desc_t* desc, mrl_render_device_t ** out_rd);

	/// <summary>
	///		Terminates an OpenGL 3.3 render device.
	/// </summary>
	/// <param name="rd">Render device</param>
	MRL_API void mrl_terminate_ogl_330_render_device(mrl_render_device_t* rd);

#ifdef __cplusplus
}
#endif
#endif
