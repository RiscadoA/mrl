#include <mrl/render_device.h>
#include <mgl/error.h>

MRL_API void mrl_clear_color(mrl_render_device_t * rd, mgl_f32_t r, mgl_f32_t g, mgl_f32_t b, mgl_f32_t a)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	rd->clear_color(rd, r, g, b, a);
}

MRL_API void mrl_clear_depth(mrl_render_device_t * rd, mgl_f32_t depth)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	rd->clear_depth(rd, depth);
}

MRL_API void mrl_clear_stencil(mrl_render_device_t * rd, mgl_i32_t stencil)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	rd->clear_stencil(rd, stencil);
}

MRL_API void mrl_swap_buffers(mrl_render_device_t * rd)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	rd->swap_buffers(rd);
}

MRL_API const mgl_chr8_t * mrl_get_type_name(mrl_render_device_t * rd)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	return rd->get_type_name(rd);
}
