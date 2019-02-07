#include <mrl/render_device.h>
#include <mgl/error.h>

MRL_API mrl_error_t mrl_create_constant_buffer(mrl_render_device_t * rd, mrl_constant_buffer_t ** cb, mrl_constant_buffer_desc_t * desc)
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

MRL_API mrl_error_t mrl_create_index_buffer(mrl_render_device_t * rd, mrl_index_buffer_t ** ib, mrl_index_buffer_desc_t * desc)
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

MRL_API mrl_error_t mrl_create_vertex_buffer(mrl_render_device_t * rd, mrl_vertex_buffer_t ** vb, mrl_vertex_buffer_desc_t * desc)
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

MRL_API mrl_error_t mrl_create_vertex_array(mrl_render_device_t * rd, mrl_vertex_array_t ** va, mrl_vertex_array_desc_t * desc)
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

MRL_API mrl_error_t mrl_create_shader_stage(mrl_render_device_t * rd, mrl_shader_stage_t ** stage, mrl_shader_stage_desc_t * desc)
{
	MGL_DEBUG_ASSERT(rd != NULL && stage != NULL && desc != NULL);
	return rd->create_shader_stage(rd, stage, desc);
}

MRL_API void mrl_destroy_shader_stage(mrl_render_device_t * rd, mrl_shader_stage_t * stage)
{
	MGL_DEBUG_ASSERT(rd != NULL && stage != NULL);
	rd->destroy_shader_stage(rd, stage);
}

MRL_API mrl_error_t mrl_create_shader_pipeline(mrl_render_device_t * rd, mrl_shader_pipeline_t ** pipeline, mrl_shader_pipeline_desc_t * desc)
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
