#include <mrl/renderer.h>

MRL_API mrl_error_t mrl_create_mesh(mrl_renderer_t * r, const mrl_mesh_desc_t * desc, mgl_u32_t * out_id)
{
	MGL_DEBUG_ASSERT(r != NULL && desc != NULL && out_id != NULL);
	return r->create_mesh(r, desc, out_id);
}

MRL_API void mrl_destroy_mesh(mrl_renderer_t * r, mgl_u32_t effect)
{
	MGL_DEBUG_ASSERT(r != NULL);
	r->destroy_mesh(r, effect);
}

MRL_API mrl_error_t mrl_create_effect(mrl_renderer_t * r, const mrl_effect_desc_t * desc, mgl_u32_t * out_id)
{
	MGL_DEBUG_ASSERT(r != NULL && desc != NULL && out_id != NULL);
	return r->create_effect(r, desc, out_id);
}

MRL_API void mrl_destroy_effect(mrl_renderer_t * r, mgl_u32_t effect)
{
	MGL_DEBUG_ASSERT(r != NULL);
	r->destroy_effect(r, effect);
}

MRL_API void mrl_render_frame(mrl_renderer_t * r, const mrl_frame_t * frame)
{
	MGL_DEBUG_ASSERT(r != NULL && frame != NULL);
	r->render_frame(r, frame);
}

MRL_API const mgl_chr8_t * mrl_get_renderer_type(mrl_renderer_t * r)
{
	MGL_DEBUG_ASSERT(r != NULL);
	return r->get_type(r);
}
