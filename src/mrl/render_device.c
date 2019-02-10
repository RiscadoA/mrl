#include <mrl/render_device.h>
#include <mgl/error.h>

MRL_API mrl_error_t mrl_create_framebuffer(mrl_render_device_t * rd, mrl_framebuffer_t ** fb, const mrl_framebuffer_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && fb != NULL && desc != NULL);
	return rd->create_framebuffer(rd, fb, desc);
}

MRL_API void mrl_destroy_framebuffer(mrl_render_device_t * rd, mrl_framebuffer_t * fb)
{
	MGL_DEBUG_ASSERT(rd != NULL && fb != NULL);
	rd->destroy_framebuffer(rd, fb);
}

MRL_API void mrl_set_framebuffer(mrl_render_device_t * rd, mrl_framebuffer_t * fb)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	rd->set_framebuffer(rd, fb);
}

MRL_API mrl_error_t mrl_create_raster_state(mrl_render_device_t * rd, mrl_raster_state_t ** s, const mrl_raster_state_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && s != NULL && desc != NULL);
	return rd->create_raster_state(rd, s, desc);
}

MRL_API void mrl_destroy_raster_state(mrl_render_device_t * rd, mrl_raster_state_t * s)
{
	MGL_DEBUG_ASSERT(rd != NULL && s != NULL);
	rd->destroy_raster_state(rd, s);
}

MRL_API void mrl_set_raster_state(mrl_render_device_t * rd, mrl_raster_state_t * s)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	rd->set_raster_state(rd, s);
}

MRL_API mrl_error_t mrl_create_depth_stencil_state(mrl_render_device_t * rd, mrl_depth_stencil_state_t ** s, const mrl_depth_stencil_state_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && s != NULL && desc != NULL);
	return rd->create_depth_stencil_state(rd, s, desc);
}

MRL_API void mrl_destroy_depth_stencil_state(mrl_render_device_t * rd, mrl_depth_stencil_state_t * s)
{
	MGL_DEBUG_ASSERT(rd != NULL && s != NULL);
	rd->destroy_depth_stencil_state(rd, s);
}

MRL_API void mrl_set_depth_stencil_state(mrl_render_device_t * rd, mrl_depth_stencil_state_t * s)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	rd->set_depth_stencil_state(rd, s);
}

MRL_API mrl_error_t mrl_create_blend_state(mrl_render_device_t * rd, mrl_blend_state_t ** s, const mrl_blend_state_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && s != NULL && desc != NULL);
	return rd->create_blend_state(rd, s, desc);
}

MRL_API void mrl_destroy_blend_state(mrl_render_device_t * rd, mrl_blend_state_t * s)
{
	MGL_DEBUG_ASSERT(rd != NULL && s != NULL);
	rd->destroy_blend_state(rd, s);
}

MRL_API void mrl_set_blend_state(mrl_render_device_t * rd, mrl_blend_state_t * s)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	rd->set_blend_state(rd, s);
}

MRL_API mrl_error_t mrl_create_sampler(mrl_render_device_t * rd, mrl_sampler_t ** s, const mrl_sampler_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && s != NULL && desc != NULL);
	return rd->create_sampler(rd, s, desc);
}

MRL_API void mrl_destroy_sampler(mrl_render_device_t * rd, mrl_sampler_t * s)
{
	MGL_DEBUG_ASSERT(rd != NULL && s != NULL);
	rd->destroy_sampler(rd, s);
}

MRL_API void mrl_bind_sampler(mrl_render_device_t * rd, mrl_shader_binding_point_t * bp, mrl_sampler_t * s)
{
	MGL_DEBUG_ASSERT(rd != NULL && bp != NULL);
	rd->bind_sampler(rd, bp, s);
}

MRL_API mrl_error_t mrl_create_texture_1d(mrl_render_device_t * rd, mrl_texture_1d_t ** tex, const mrl_texture_1d_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL && desc != NULL);
	return rd->create_texture_1d(rd, tex, desc);
}

MRL_API void mrl_destroy_texture_1d(mrl_render_device_t * rd, mrl_texture_1d_t * tex)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL);
	rd->destroy_texture_1d(rd, tex);
}

MRL_API void mrl_generate_texture_1d_mipmaps(mrl_render_device_t * rd, mrl_texture_1d_t * tex)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL);
	rd->generate_texture_1d_mipmaps(rd, tex);
}

MRL_API void mrl_bind_texture_1d(mrl_render_device_t * rd, mrl_shader_binding_point_t * bp, mrl_texture_1d_t * tex)
{
	MGL_DEBUG_ASSERT(rd != NULL && bp != NULL);
	rd->bind_texture_1d(rd, bp, tex);
}

MRL_API mrl_error_t mrl_update_texture_1d(mrl_render_device_t * rd, mrl_texture_1d_t * tex, const mrl_texture_1d_update_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL && desc != NULL);
	return rd->update_texture_1d(rd, tex, desc);
}

MRL_API mrl_error_t mrl_create_texture_2d(mrl_render_device_t * rd, mrl_texture_2d_t ** tex, const mrl_texture_2d_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL && desc != NULL);
	return rd->create_texture_2d(rd, tex, desc);
}

MRL_API void mrl_destroy_texture_2d(mrl_render_device_t * rd, mrl_texture_2d_t * tex)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL);
	rd->destroy_texture_2d(rd, tex);
}

MRL_API void mrl_generate_texture_2d_mipmaps(mrl_render_device_t * rd, mrl_texture_2d_t * tex)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL);
	rd->generate_texture_2d_mipmaps(rd, tex);
}

MRL_API void mrl_bind_texture_2d(mrl_render_device_t * rd, mrl_shader_binding_point_t * bp, mrl_texture_2d_t * tex)
{
	MGL_DEBUG_ASSERT(rd != NULL && bp != NULL);
	rd->bind_texture_2d(rd, bp, tex);
}

MRL_API mrl_error_t mrl_update_texture_2d(mrl_render_device_t * rd, mrl_texture_2d_t * tex, const mrl_texture_2d_update_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL && desc != NULL);
	return rd->update_texture_2d(rd, tex, desc);
}

MRL_API mrl_error_t mrl_create_texture_3d(mrl_render_device_t * rd, mrl_texture_3d_t ** tex, const mrl_texture_3d_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL && desc != NULL);
	return rd->create_texture_3d(rd, tex, desc);
}

MRL_API void mrl_destroy_texture_3d(mrl_render_device_t * rd, mrl_texture_3d_t * tex)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL);
	rd->destroy_texture_3d(rd, tex);
}

MRL_API void mrl_generate_texture_3d_mipmaps(mrl_render_device_t * rd, mrl_texture_3d_t * tex)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL);
	rd->generate_texture_3d_mipmaps(rd, tex);
}

MRL_API void mrl_bind_texture_3d(mrl_render_device_t * rd, mrl_shader_binding_point_t * bp, mrl_texture_3d_t * tex)
{
	MGL_DEBUG_ASSERT(rd != NULL && bp != NULL);
	rd->bind_texture_3d(rd, bp, tex);
}

MRL_API mrl_error_t mrl_update_texture_3d(mrl_render_device_t * rd, mrl_texture_3d_t * tex, const mrl_texture_3d_update_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && tex != NULL && desc != NULL);
	return rd->update_texture_3d(rd, tex, desc);
}

MRL_API mrl_error_t mrl_create_cube_map(mrl_render_device_t * rd, mrl_cube_map_t ** cb, const mrl_cube_map_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && cb != NULL && desc != NULL);
	return rd->create_cube_map(rd, cb, desc);
}

MRL_API void mrl_destroy_cube_map(mrl_render_device_t * rd, mrl_cube_map_t * cb)
{
	MGL_DEBUG_ASSERT(rd != NULL && cb != NULL);
	rd->destroy_cube_map(rd, cb);
}

MRL_API void mrl_generate_cube_map_mipmaps(mrl_render_device_t * rd, mrl_cube_map_t * cb)
{
	MGL_DEBUG_ASSERT(rd != NULL && cb != NULL);
	rd->generate_cube_map_mipmaps(rd, cb);
}

MRL_API void mrl_bind_cube_map(mrl_render_device_t * rd, mrl_shader_binding_point_t * bp, mrl_cube_map_t * cb)
{
	MGL_DEBUG_ASSERT(rd != NULL && bp != NULL);
	rd->bind_cube_map(rd, bp, cb);
}

MRL_API mrl_error_t mrl_update_cube_map(mrl_render_device_t * rd, mrl_cube_map_t * cb, const mrl_cube_map_update_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && cb != NULL && desc != NULL);
	return rd->update_cube_map(rd, cb, desc);
}

MRL_API mrl_error_t mrl_create_constant_buffer(mrl_render_device_t * rd, mrl_constant_buffer_t ** cb, const mrl_constant_buffer_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && cb != NULL && desc != NULL);
	return rd->create_constant_buffer(rd, cb, desc);
}

MRL_API void mrl_destroy_constant_buffer(mrl_render_device_t * rd, mrl_constant_buffer_t * cb)
{
	MGL_DEBUG_ASSERT(rd != NULL && cb != NULL);
	rd->destroy_constant_buffer(rd, cb);
}

MRL_API void mrl_bind_constant_buffer(mrl_render_device_t * rd, mrl_shader_binding_point_t * bp, mrl_constant_buffer_t * cb)
{
	MGL_DEBUG_ASSERT(rd != NULL && bp != NULL);
	rd->bind_constant_buffer(rd, bp, cb);
}

MRL_API void * mrl_map_constant_buffer(mrl_render_device_t * rd, mrl_constant_buffer_t * cb)
{
	MGL_DEBUG_ASSERT(rd != NULL && cb != NULL);
	return rd->map_constant_buffer(rd, cb);
}

MRL_API void mrl_unmap_constant_buffer(mrl_render_device_t * rd, mrl_constant_buffer_t * cb)
{
	MGL_DEBUG_ASSERT(rd != NULL && cb != NULL);
	rd->unmap_constant_buffer(rd, cb);
}

MRL_API void mrl_update_constant_buffer(mrl_render_device_t * rd, mrl_constant_buffer_t * cb, mgl_u64_t offset, mgl_u64_t size, const void * data)
{
	MGL_DEBUG_ASSERT(rd != NULL && cb != NULL && data != NULL);
	rd->update_constant_buffer(rd, cb, offset, size, data);
}

MRL_API void mrl_query_constant_buffer_structure(mrl_render_device_t * rd, mrl_shader_binding_point_t * bp, mrl_constant_buffer_structure_t * cbs)
{
	MGL_DEBUG_ASSERT(rd != NULL && bp != NULL && cbs != NULL);
	rd->query_constant_buffer_structure(rd, bp, cbs);
}

MRL_API mrl_error_t mrl_create_index_buffer(mrl_render_device_t * rd, mrl_index_buffer_t ** ib, const mrl_index_buffer_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && ib != NULL && desc != NULL);
	return rd->create_index_buffer(rd, ib, desc);
}

MRL_API void mrl_destroy_index_buffer(mrl_render_device_t * rd, mrl_index_buffer_t * ib)
{
	MGL_DEBUG_ASSERT(rd != NULL && ib != NULL);
	rd->destroy_index_buffer(rd, ib);
}

MRL_API void mrl_set_index_buffer(mrl_render_device_t * rd, mrl_index_buffer_t * ib)
{
	MGL_DEBUG_ASSERT(rd != NULL && ib != NULL);
	rd->set_index_buffer(rd, ib);
}

MRL_API void * mrl_map_index_buffer(mrl_render_device_t * rd, mrl_index_buffer_t * ib)
{
	MGL_DEBUG_ASSERT(rd != NULL && ib != NULL);
	return rd->map_index_buffer(rd, ib);
}

MRL_API void mrl_unmap_index_buffer(mrl_render_device_t * rd, mrl_index_buffer_t * ib)
{
	MGL_DEBUG_ASSERT(rd != NULL && ib != NULL);
	rd->unmap_index_buffer(rd, ib);
}

MRL_API void mrl_update_index_buffer(mrl_render_device_t * rd, mrl_index_buffer_t * ib, mgl_u64_t offset, mgl_u64_t size, const void * data)
{
	MGL_DEBUG_ASSERT(rd != NULL && ib != NULL && data != NULL);
	rd->update_index_buffer(rd, ib, offset, size, data);
}

MRL_API mrl_error_t mrl_create_vertex_buffer(mrl_render_device_t * rd, mrl_vertex_buffer_t ** vb, const mrl_vertex_buffer_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && vb != NULL && desc != NULL);
	return rd->create_vertex_buffer(rd, vb, desc);
}

MRL_API void mrl_destroy_vertex_buffer(mrl_render_device_t * rd, mrl_vertex_buffer_t * vb)
{
	MGL_DEBUG_ASSERT(rd != NULL && vb != NULL);
	rd->destroy_vertex_buffer(rd, vb);
}

MRL_API void * mrl_map_vertex_buffer(mrl_render_device_t * rd, mrl_vertex_buffer_t * vb)
{
	MGL_DEBUG_ASSERT(rd != NULL && vb != NULL);
	return rd->map_vertex_buffer(rd, vb);
}

MRL_API void mrl_unmap_vertex_buffer(mrl_render_device_t * rd, mrl_vertex_buffer_t * vb)
{
	MGL_DEBUG_ASSERT(rd != NULL && vb != NULL);
	rd->unmap_vertex_buffer(rd, vb);
}

MRL_API void mrl_update_vertex_buffer(mrl_render_device_t * rd, mrl_vertex_buffer_t * vb, mgl_u64_t offset, mgl_u64_t size, const void * data)
{
	MGL_DEBUG_ASSERT(rd != NULL && vb != NULL && data != NULL);
	rd->update_vertex_buffer(rd, vb, offset, size, data);
}

MRL_API mrl_error_t mrl_create_vertex_array(mrl_render_device_t * rd, mrl_vertex_array_t ** va, const mrl_vertex_array_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && va != NULL && desc != NULL);
	return rd->create_vertex_array(rd, va, desc);
}

MRL_API void mrl_destroy_vertex_array(mrl_render_device_t * rd, mrl_vertex_array_t * va)
{
	MGL_DEBUG_ASSERT(rd != NULL && va != NULL);
	rd->destroy_vertex_array(rd, va);
}

MRL_API void mrl_set_vertex_array(mrl_render_device_t * rd, mrl_vertex_array_t * va)
{
	MGL_DEBUG_ASSERT(rd != NULL && va != NULL);
	rd->set_vertex_array(rd, va);
}

MRL_API mrl_error_t mrl_create_shader_stage(mrl_render_device_t * rd, mrl_shader_stage_t ** stage, const mrl_shader_stage_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && stage != NULL && desc != NULL);
	return rd->create_shader_stage(rd, stage, desc);
}

MRL_API void mrl_destroy_shader_stage(mrl_render_device_t * rd, mrl_shader_stage_t * stage)
{
	MGL_DEBUG_ASSERT(rd != NULL && stage != NULL);
	rd->destroy_shader_stage(rd, stage);
}

MRL_API mrl_error_t mrl_create_shader_pipeline(mrl_render_device_t * rd, mrl_shader_pipeline_t ** pipeline, const mrl_shader_pipeline_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && pipeline != NULL && desc != NULL);
	return rd->create_shader_pipeline(rd, pipeline, desc);
}

MRL_API void mrl_destroy_shader_pipeline(mrl_render_device_t * rd, mrl_shader_pipeline_t * pipeline)
{
	MGL_DEBUG_ASSERT(rd != NULL && pipeline != NULL);
	rd->destroy_shader_pipeline(rd, pipeline);
}

MRL_API void mrl_set_shader_pipeline(mrl_render_device_t * rd, mrl_shader_pipeline_t * pipeline)
{
	MGL_DEBUG_ASSERT(rd != NULL && pipeline != NULL);
	rd->set_shader_pipeline(rd, pipeline);
}

MRL_API mrl_shader_binding_point_t * mrl_get_shader_binding_point(mrl_render_device_t * rd, mrl_shader_pipeline_t * pipeline, const mgl_chr8_t * name)
{
	MGL_DEBUG_ASSERT(rd != NULL && pipeline != NULL && name != NULL);
	return rd->get_shader_binding_point(rd, pipeline, name);
}

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

MRL_API void mrl_draw_triangles(mrl_render_device_t * rd, mgl_u64_t offset, mgl_u64_t count)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	rd->draw_triangles(rd, offset, count);
}

MRL_API void mrl_draw_triangles_indexed(mrl_render_device_t * rd, mgl_u64_t offset, mgl_u64_t count)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	rd->draw_triangles_indexed(rd, offset, count);
}

MRL_API const mgl_chr8_t * mrl_get_type_name(mrl_render_device_t * rd)
{
	MGL_DEBUG_ASSERT(rd != NULL);
	return rd->get_type_name(rd);
}
