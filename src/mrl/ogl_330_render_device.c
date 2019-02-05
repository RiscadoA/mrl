#include <mrl/ogl_330_render_device.h>

#include <mgl/memory/allocator.h>
#include <mgl/memory/manipulation.h>
#include <mgl/memory/pool_allocator.h>
#include <mgl/string/manipulation.h>
#include <mgl/input/window.h>

#ifdef MRL_BUILD_OGL_330
#	ifdef MGL_SYSTEM_WINDOWS
#		include <mgl/input/windows_window.h>
#		define GLEW_STATIC
#		include <GL/glew.h>
#		include <GL/wglew.h>
#	endif

typedef struct
{
	mrl_render_device_t base;

	void* allocator;
	void* window;

#	ifdef MGL_SYSTEM_WINDOWS
	struct
	{
		HDC hdc;
		HGLRC hrc;
	} win32;
#	endif

	struct
	{
		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} index_buffer;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} vertex_buffer;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} vertex_array;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} shader_stage;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} shader_pipeline;
	} memory;

	struct
	{
		GLenum index_buffer_format;
	} state;

	mrl_render_device_hint_error_callback_t error_callback;
	mrl_render_device_hint_error_callback_t warning_callback;
} mrl_ogl_330_render_device_t;

typedef struct
{
	GLuint id;
	GLenum format;
} mrl_ogl_330_index_buffer_t;

typedef struct
{
	GLuint id;
} mrl_ogl_330_vertex_buffer_t;

typedef struct
{
	GLuint id;
} mrl_ogl_330_vertex_array_t;

typedef struct
{
	GLuint id;
} mrl_ogl_330_shader_stage_t;

typedef struct
{
	GLuint id;
} mrl_ogl_330_shader_pipeline_t;

static mrl_error_t create_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t** ib, mrl_index_buffer_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Check for invalid input
	if (desc->usage == MRL_INDEX_BUFFER_USAGE_STATIC && desc->data == NULL)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create index buffer: when the usage mode is set to static, the pointer to the initial data must not be NULL");
		return MRL_ERROR_INVALID_PARAMS;
	}

	// Get format
	GLenum format;

	if (desc->format == MRL_INDEX_BUFFER_FORMAT_U16)
		format = GL_UNSIGNED_SHORT;
	else if (desc->format == MRL_INDEX_BUFFER_FORMAT_U32)
		format = GL_UNSIGNED_INT;
	else
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create index buffer: invalid index format");
		return MRL_ERROR_INVALID_PARAMS;
	}

	// Get usage
	GLenum usage;

	if (desc->usage == MRL_INDEX_BUFFER_USAGE_DEFAULT)
		usage = GL_STATIC_DRAW;
	else if (desc->usage == MRL_INDEX_BUFFER_USAGE_STATIC)
		usage = GL_STATIC_DRAW;
	else if (desc->usage == MRL_INDEX_BUFFER_USAGE_DYNAMIC)
		usage = GL_DYNAMIC_DRAW;
	else if (desc->usage == MRL_INDEX_BUFFER_USAGE_STREAM)
		usage = GL_STREAM_DRAW;
	else
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create index buffer: invalid usage mode");
		return MRL_ERROR_INVALID_PARAMS;
	}

	// Initialize index buffer
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	if (desc->data == NULL)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, desc->size, NULL, usage);
	else
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, desc->size, desc->data, usage);

	// Allocate object
	mrl_ogl_330_index_buffer_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.index_buffer.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteBuffers(1, &id);
		return mrl_make_mgl_error(err);
	}

	// Store index buffer info
	obj->id = id;
	obj->format = format;
	*ib = (mrl_index_buffer_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_index_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t* ib)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_index_buffer_t* obj = (mrl_ogl_330_index_buffer_t*)ib;

	// Delete index buffer
	glDeleteBuffers(1, &obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.index_buffer.pool,
		obj);
}

static void set_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t* ib)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_index_buffer_t* obj = (mrl_ogl_330_index_buffer_t*)ib;

	// Set index buffer
	rd->state.index_buffer_format = obj->format;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->id);
}

static void* map_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t* ib)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_index_buffer_t* obj = (mrl_ogl_330_index_buffer_t*)ib;

	// Map IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->id);
	return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
}

static void unmap_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t* ib)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_index_buffer_t* obj = (mrl_ogl_330_index_buffer_t*)ib;

	// Unmap IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->id);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

static void update_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t* ib, mgl_u64_t offset, mgl_u64_t size, const void* data)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_index_buffer_t* obj = (mrl_ogl_330_index_buffer_t*)ib;

	// Update IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->id);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

static mrl_error_t create_vertex_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t** vb, mrl_vertex_buffer_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Check for invalid input
	if (desc->usage == MRL_VERTEX_BUFFER_USAGE_STATIC && desc->data == NULL)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create vertex buffer: when the usage mode is set to static, the pointer to the initial data must not be NULL");
		return MRL_ERROR_INVALID_PARAMS;
	}

	// Get usage
	GLenum usage;

	if (desc->usage == MRL_VERTEX_BUFFER_USAGE_DEFAULT)
		usage = GL_STATIC_DRAW;
	else if (desc->usage == MRL_VERTEX_BUFFER_USAGE_STATIC)
		usage = GL_STATIC_DRAW;
	else if (desc->usage == MRL_VERTEX_BUFFER_USAGE_DYNAMIC)
		usage = GL_DYNAMIC_DRAW;
	else if (desc->usage == MRL_VERTEX_BUFFER_USAGE_STREAM)
		usage = GL_STREAM_DRAW;
	else
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create vertex buffer: invalid usage mode");
		return MRL_ERROR_INVALID_PARAMS;
	}

	// Initialize vertex buffer
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	if (desc->data == NULL)
		glBufferData(GL_ARRAY_BUFFER, desc->size, NULL, usage);
	else
		glBufferData(GL_ARRAY_BUFFER, desc->size, desc->data, usage);

	// Allocate object
	mrl_ogl_330_vertex_buffer_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.vertex_buffer.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteBuffers(1, &id);
		return mrl_make_mgl_error(err);
	}

	// Store vertex buffer info
	obj->id = id;
	*vb = (mrl_vertex_buffer_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_vertex_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t* vb)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_vertex_buffer_t* obj = (mrl_ogl_330_vertex_buffer_t*)vb;

	// Delete vertex buffer
	glDeleteBuffers(1, &obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.vertex_buffer.pool,
		obj);
}

static void* map_vertex_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t* vb)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_vertex_buffer_t* obj = (mrl_ogl_330_vertex_buffer_t*)vb;

	// Map VBO
	glBindBuffer(GL_ARRAY_BUFFER, obj->id);
	return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

static void unmap_vertex_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t* vb)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_vertex_buffer_t* obj = (mrl_ogl_330_vertex_buffer_t*)vb;

	// Unmap VBO
	glBindBuffer(GL_ARRAY_BUFFER, obj->id);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

static void update_vertex_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t* vb, mgl_u64_t offset, mgl_u64_t size, const void* data)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_vertex_buffer_t* obj = (mrl_ogl_330_vertex_buffer_t*)vb;

	// Update VBO
	glBindBuffer(GL_ARRAY_BUFFER, obj->id);
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

static mrl_error_t create_vertex_array(mrl_render_device_t* brd, mrl_vertex_array_t** va, mrl_vertex_array_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	MGL_DEBUG_ASSERT(desc->shader_pipeline != NULL);
	mrl_ogl_330_shader_pipeline_t* pp = (mrl_ogl_330_shader_pipeline_t*)desc->shader_pipeline;

	// Initialize vertex array
	GLuint id;
	glGenVertexArrays(1, &id);
	glBindVertexArray(id);
	
	// Link elements
	MGL_DEBUG_ASSERT(desc->element_count <= MRL_MAX_VERTEX_ARRAY_ELEMENT_COUNT);
	MGL_DEBUG_ASSERT(desc->buffer_count <= MRL_MAX_VERTEX_ARRAY_BUFFER_COUNT);
	for (mgl_u32_t i = 0; i < desc->element_count; ++i)
	{
		// Get buffer
		MGL_DEBUG_ASSERT(desc->elements[i].buffer.index < desc->buffer_count);
		mrl_ogl_330_vertex_buffer_t* vbo = (mrl_ogl_330_vertex_buffer_t*)desc->buffers[desc->elements[i].buffer.index];
		MGL_DEBUG_ASSERT(vbo != NULL);
		glBindBuffer(GL_ARRAY_BUFFER, vbo->id);

		// Get attribute location
		GLuint loc = glGetAttribLocation(pp->id, desc->elements[i].name);
		
		// Get type
		GLenum type;
		GLboolean normalized;
		GLboolean integer;

		switch (desc->elements[i].type)
		{
			case MRL_VERTEX_ELEMENT_TYPE_I8: type = GL_BYTE; normalized = GL_FALSE; integer = MGL_TRUE; break;
			case MRL_VERTEX_ELEMENT_TYPE_I16: type = GL_SHORT; normalized = GL_FALSE; integer = MGL_TRUE; break;
			case MRL_VERTEX_ELEMENT_TYPE_I32: type = GL_INT; normalized = GL_FALSE; integer = MGL_TRUE; break;
			case MRL_VERTEX_ELEMENT_TYPE_U8: type = GL_UNSIGNED_BYTE; normalized = GL_FALSE; integer = MGL_TRUE; break;
			case MRL_VERTEX_ELEMENT_TYPE_U16: type = GL_UNSIGNED_SHORT; normalized = GL_FALSE; integer = MGL_TRUE; break;
			case MRL_VERTEX_ELEMENT_TYPE_U32: type = GL_UNSIGNED_INT; normalized = GL_FALSE; integer = MGL_TRUE; break;
			case MRL_VERTEX_ELEMENT_TYPE_N8: type = GL_BYTE; normalized = GL_TRUE; integer = MGL_FALSE; break;
			case MRL_VERTEX_ELEMENT_TYPE_N16: type = GL_SHORT; normalized = GL_TRUE; integer = MGL_FALSE; break;
			case MRL_VERTEX_ELEMENT_TYPE_NU8: type = GL_UNSIGNED_BYTE; normalized = GL_TRUE; integer = MGL_FALSE; break;
			case MRL_VERTEX_ELEMENT_TYPE_NU16: type = GL_UNSIGNED_SHORT; normalized = GL_TRUE; integer = MGL_FALSE; break;
			case MRL_VERTEX_ELEMENT_TYPE_F32: type = GL_FLOAT; normalized = GL_FALSE; integer = MGL_FALSE; break;
			default:
				if (rd->error_callback != NULL)
					rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create vertex array, invalid vertex element type");
				glDeleteVertexArrays(1, &id);
				return MRL_ERROR_INVALID_PARAMS;
		}

		MGL_DEBUG_ASSERT(desc->elements[i].size >= 1 && desc->elements[i].size <= 4);
		
		glEnableVertexAttribArray(loc);
		if (!integer)
			glVertexAttribPointer(loc, (GLint)desc->elements[i].size, type, normalized, (GLsizei)desc->elements[i].buffer.stride, (const void*)desc->elements[i].buffer.offset);
		else
			glVertexAttribIPointer(loc, (GLint)desc->elements[i].size, type, (GLsizei)desc->elements[i].buffer.stride, (const void*)desc->elements[i].buffer.offset);
	}

	// Allocate object
	mrl_ogl_330_vertex_array_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.vertex_array.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteVertexArrays(1, &id);
		return mrl_make_mgl_error(err);
	}

	// Store vertex array info
	obj->id = id;
	*va = (mrl_vertex_array_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_vertex_array(mrl_render_device_t* brd, mrl_shader_pipeline_t* va)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_vertex_array_t* obj = (mrl_ogl_330_vertex_array_t*)va;

	// Delete vertex array
	glDeleteVertexArrays(1, &obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.vertex_array.pool,
		obj);
}

static void set_vertex_array(mrl_render_device_t* brd, mrl_shader_pipeline_t* va)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_vertex_array_t* obj = (mrl_ogl_330_vertex_array_t*)va;

	// Set vertex array
	glBindVertexArray(obj->id);
}

// -------- Shaders ----------

static mrl_error_t create_shader_stage(mrl_render_device_t* brd, mrl_shader_stage_t** stage, mrl_shader_stage_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	
	// Check if the source is supported (only GLSL is supported for now)
	if (desc->src_type != MRL_SHADER_SOURCE_GLSL)
		return MRL_ERROR_UNSUPPORTED_SHADER_SOURCE;

	// Get shader stage type
	GLenum shader_type;
	switch (desc->stage)
	{
		case MRL_SHADER_STAGE_VERTEX: shader_type = GL_VERTEX_SHADER; break;
		case MRL_SHADER_STAGE_PIXEL: shader_type = GL_FRAGMENT_SHADER; break;
		default: return MRL_ERROR_UNSUPPORTED_SHADER_STAGE;
	}

	// Initialize shader
	GLuint id = glCreateShader(shader_type);
	glShaderSource(id, 1, (const GLchar* const*)&desc->src, NULL);
	glCompileShader(id);

	// Check for errors
	GLint success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar info_log[512];
		glGetShaderInfoLog(id, sizeof(info_log), NULL, info_log);
		glDeleteShader(id);

		// Call error callback
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_FAILED_TO_COMPILE_SHADER_STAGE, info_log);
		return MRL_ERROR_FAILED_TO_COMPILE_SHADER_STAGE;
	}

	// Allocate object
	mrl_ogl_330_shader_stage_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.shader_stage.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteShader(id);
		return mrl_make_mgl_error(err);
	}

	// Store stage info
	obj->id = id;
	*stage = (mrl_shader_stage_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_shader_stage(mrl_render_device_t* brd, mrl_shader_stage_t* stage)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_shader_stage_t* obj = (mrl_ogl_330_shader_stage_t*)stage;

	// Delete shader
	glDeleteShader(obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.shader_stage.pool,
		obj);
}

static mrl_error_t create_shader_pipeline(mrl_render_device_t* brd, mrl_shader_pipeline_t** pipeline, mrl_shader_pipeline_desc_t* desc)
{
	MGL_DEBUG_ASSERT(desc->vertex != NULL && desc->pixel != NULL);

	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Initialize program
	GLuint id = glCreateProgram();
	glAttachShader(id, ((mrl_ogl_330_shader_stage_t*)desc->vertex)->id);
	glAttachShader(id, ((mrl_ogl_330_shader_stage_t*)desc->pixel)->id);
	glLinkProgram(id);

	// Check for errors
	GLint success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar info_log[512];
		glGetProgramInfoLog(id, sizeof(info_log), NULL, info_log);
		glDeleteProgram(id);

		// Call error callback
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_FAILED_TO_LINK_SHADER_PIPELINE, info_log);
		return MRL_ERROR_FAILED_TO_LINK_SHADER_PIPELINE;
	}

	// Allocate object
	mrl_ogl_330_shader_pipeline_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.shader_pipeline.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteProgram(id);
		return mrl_make_mgl_error(err);
	}

	// Store pipeline info
	obj->id = id;
	*pipeline = (mrl_shader_pipeline_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_shader_pipeline(mrl_render_device_t* brd, mrl_shader_pipeline_t* pipeline)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_shader_pipeline_t* obj = (mrl_ogl_330_shader_pipeline_t*)pipeline;

	// Delete program
	glDeleteProgram(obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.shader_pipeline.pool,
		obj);
}

static void set_shader_pipeline(mrl_render_device_t* brd, mrl_shader_pipeline_t* pipeline)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_shader_pipeline_t* obj = (mrl_ogl_330_shader_pipeline_t*)pipeline;

	// Set program
	glUseProgram(obj->id);
}

// --------- Draw functions ----------

static void clear_color(mrl_render_device_t* brd, mgl_f32_t r, mgl_f32_t g, mgl_f32_t b, mgl_f32_t a)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

static void clear_depth(mrl_render_device_t* brd, mgl_f32_t depth)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	glClearDepth(depth);
	glClear(GL_DEPTH_BUFFER_BIT);
}

static void clear_stencil(mrl_render_device_t* brd, mgl_i32_t stencil)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	glClearStencil(stencil);
	glClear(GL_STENCIL_BUFFER_BIT);
}

static void swap_buffers(mrl_render_device_t* brd)
{
#	ifdef MGL_SYSTEM_WINDOWS
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	SwapBuffers(rd->win32.hdc);
#	endif
}

static void draw_triangles(mrl_render_device_t* brd, mgl_u64_t offset, mgl_u64_t count)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	glDrawArrays(GL_TRIANGLES, (GLint)offset, (GLsizei)count);
}

static void draw_triangles_indexed(mrl_render_device_t* brd, mgl_u64_t offset, mgl_u64_t count)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	glDrawElements(GL_TRIANGLES, (GLsizei)count, rd->state.index_buffer_format, (const void*)offset);
}

static const mgl_chr8_t* get_type_name(mrl_render_device_t* rd)
{
	return u8"ogl_330";
}

static mrl_error_t create_rd_allocators(mrl_ogl_330_render_device_t* rd, const mrl_render_device_desc_t* desc)
{
	mgl_error_t err;

	// Create shader stage pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_shader_stage_count, sizeof(mrl_ogl_330_shader_stage_t)),
		(void**)&rd->memory.shader_stage.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_1;
	mgl_init_pool_allocator(
		&rd->memory.shader_stage.pool,
		desc->max_shader_stage_count,
		sizeof(mrl_ogl_330_shader_stage_t),
		rd->memory.shader_stage.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_shader_stage_count, sizeof(mrl_ogl_330_shader_stage_t)));

	// Create shader stage pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_shader_pipeline_count, sizeof(mrl_ogl_330_shader_pipeline_t)),
		(void**)&rd->memory.shader_pipeline.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_2;
	mgl_init_pool_allocator(
		&rd->memory.shader_pipeline.pool,
		desc->max_shader_pipeline_count,
		sizeof(mrl_ogl_330_shader_pipeline_t),
		rd->memory.shader_pipeline.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_shader_pipeline_count, sizeof(mrl_ogl_330_shader_pipeline_t)));

	// Create vertex buffer pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_vertex_buffer_count, sizeof(mrl_ogl_330_vertex_buffer_t)),
		(void**)&rd->memory.vertex_buffer.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_3;
	mgl_init_pool_allocator(
		&rd->memory.vertex_buffer.pool,
		desc->max_vertex_buffer_count,
		sizeof(mrl_ogl_330_vertex_buffer_t),
		rd->memory.vertex_buffer.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_vertex_buffer_count, sizeof(mrl_ogl_330_vertex_buffer_t)));

	// Create vertex array pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_vertex_array_count, sizeof(mrl_ogl_330_vertex_array_t)),
		(void**)&rd->memory.vertex_array.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_4;
	mgl_init_pool_allocator(
		&rd->memory.vertex_array.pool,
		desc->max_vertex_array_count,
		sizeof(mrl_ogl_330_vertex_array_t),
		rd->memory.vertex_array.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_vertex_array_count, sizeof(mrl_ogl_330_vertex_array_t)));

	// Create index buffer pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_index_buffer_count, sizeof(mrl_ogl_330_index_buffer_t)),
		(void**)&rd->memory.index_buffer.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_5;
	mgl_init_pool_allocator(
		&rd->memory.index_buffer.pool,
		desc->max_index_buffer_count,
		sizeof(mrl_ogl_330_index_buffer_t),
		rd->memory.index_buffer.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_index_buffer_count, sizeof(mrl_ogl_330_index_buffer_t)));

	return MRL_ERROR_NONE;

mgl_error_5:
	mgl_deallocate(rd->allocator, rd->memory.vertex_array.data);
mgl_error_4:
	mgl_deallocate(rd->allocator, rd->memory.vertex_buffer.data);
mgl_error_3:
	mgl_deallocate(rd->allocator, rd->memory.shader_pipeline.data);
mgl_error_2:
	mgl_deallocate(rd->allocator, rd->memory.shader_stage.data);
mgl_error_1:
	return mrl_make_mgl_error(err);
}

static void destroy_rd_allocators(mrl_ogl_330_render_device_t* rd)
{
	mgl_deallocate(rd->allocator, rd->memory.index_buffer.data);
	mgl_deallocate(rd->allocator, rd->memory.vertex_buffer.data);
	mgl_deallocate(rd->allocator, rd->memory.vertex_array.data);
	mgl_deallocate(rd->allocator, rd->memory.shader_stage.data);
	mgl_deallocate(rd->allocator, rd->memory.shader_pipeline.data);
}

static void set_rd_functions(mrl_ogl_330_render_device_t* rd)
{
	// Index buffer functions
	rd->base.create_index_buffer = &create_index_buffer;
	rd->base.destroy_index_buffer = &destroy_index_buffer;
	rd->base.set_index_buffer = &set_index_buffer;
	rd->base.map_index_buffer = &map_index_buffer;
	rd->base.unmap_index_buffer = &unmap_index_buffer;
	rd->base.update_index_buffer = &update_index_buffer;

	// Vertex buffer functions
	rd->base.create_vertex_buffer = &create_vertex_buffer;
	rd->base.destroy_vertex_buffer = &destroy_vertex_buffer;
	rd->base.map_vertex_buffer = &map_vertex_buffer;
	rd->base.unmap_vertex_buffer = &unmap_vertex_buffer;
	rd->base.update_vertex_buffer = &update_vertex_buffer;

	// Vertex array functions
	rd->base.create_vertex_array = &create_vertex_array;
	rd->base.destroy_vertex_array = &destroy_vertex_array;
	rd->base.set_vertex_array = &set_vertex_array;

	// Shader functions
	rd->base.create_shader_stage = &create_shader_stage;
	rd->base.destroy_shader_stage = &destroy_shader_stage;
	rd->base.create_shader_pipeline = &create_shader_pipeline;
	rd->base.destroy_shader_pipeline = &destroy_shader_pipeline;
	rd->base.set_shader_pipeline = &set_shader_pipeline;

	// Draw functions
	rd->base.clear_color = &clear_color;
	rd->base.clear_depth = &clear_depth;
	rd->base.clear_stencil = &clear_stencil;
	rd->base.draw_triangles = &draw_triangles;
	rd->base.draw_triangles_indexed = &draw_triangles_indexed;

	// Swap buffers
	if (mgl_str_equal(u8"win32", mgl_get_window_type(rd->window)))
	{
#	ifndef MGL_SYSTEM_WINDOWS
		mgl_abort();
#	else
		rd->base.swap_buffers = &swap_buffers;
#	endif
	}
	else mgl_abort();

	rd->base.get_type_name = &get_type_name;
}

static void extract_hints(mrl_ogl_330_render_device_t* rd, const mrl_render_device_desc_t* desc)
{
	for (const mrl_hint_t* hint = desc->hints; hint != NULL; hint = hint->next)
	{
		// Check if the hint should be skipped
		if (hint->device_type != NULL && !mgl_str_equal(hint->device_type, u8"ogl_330"))
			continue;

		// Extract hint info
		switch (hint->type)
		{
			case MRL_HINT_WARNING_CALLBACK:
				MGL_DEBUG_ASSERT(hint->data != NULL);
				rd->warning_callback = *(const mrl_render_device_hint_warning_callback_t*)hint->data;
				break;

			case MRL_HINT_ERROR_CALLBACK:
				MGL_DEBUG_ASSERT(hint->data != NULL);
				rd->error_callback = *(const mrl_render_device_hint_error_callback_t*)hint->data;
				break;

			default:
				// Unsupported hint type, ignore it
				continue;
		}
	}
}

static mrl_error_t create_gl_context(mrl_ogl_330_render_device_t* rd, const mrl_render_device_desc_t* desc)
{
	if (mgl_str_equal(u8"win32", mgl_get_window_type(rd->window)))
	{
#	ifndef MGL_SYSTEM_WINDOWS
		return MRL_ERROR_UNSUPPORTED_WINDOW;
#	else
		HWND hwnd = (HWND)((mgl_windows_window_t*)rd->window)->hwnd;
		rd->win32.hdc = GetDC(hwnd);

		// Init context
		PIXELFORMATDESCRIPTOR pfd;
		mgl_mem_set(&pfd, sizeof(pfd), 0);
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int n_pixel_format = ChoosePixelFormat(rd->win32.hdc, &pfd);
		if (n_pixel_format == 0)
			return MRL_ERROR_EXTERNAL;
		if (!SetPixelFormat(rd->win32.hdc, n_pixel_format, &pfd))
			return MRL_ERROR_EXTERNAL;

		HGLRC temp_context = wglCreateContext(rd->win32.hdc);
		wglMakeCurrent(rd->win32.hdc, temp_context);

		if (glewInit() != GLEW_OK)
			return MGL_ERROR_EXTERNAL;

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0,
		};

		if (!WGL_ARB_create_context)
			return MRL_ERROR_UNSUPPORTED_DEVICE;

		rd->win32.hrc = wglCreateContextAttribsARB(rd->win32.hdc, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(temp_context);
		wglMakeCurrent(rd->win32.hdc, rd->win32.hrc);
		
		if (WGL_EXT_swap_control_tear && desc->vsync_mode == MRL_VSYNC_ADAPTIVE)
			wglSwapIntervalEXT(-1);
		else if (WGL_EXT_swap_control)
		{
			if (desc->vsync_mode == MRL_VSYNC_ADAPTIVE || desc->vsync_mode == MRL_VSYNC_ON)
				wglSwapIntervalEXT(1);
			else
				wglSwapIntervalEXT(0);
		}

		if (!rd->win32.hrc)
			return MRL_ERROR_EXTERNAL;
		return MRL_ERROR_NONE;
#	endif
	}
	else
		return MRL_ERROR_UNSUPPORTED_WINDOW;
}

static void destroy_gl_context(mrl_ogl_330_render_device_t* rd)
{
	if (mgl_str_equal(u8"win32", mgl_get_window_type(rd->window)))
	{
#	ifndef MGL_SYSTEM_WINDOWS
		mgl_abort();
#	else
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(rd->win32.hrc);
#	endif
	}
	else
		mgl_abort();
}
#endif

MRL_API mrl_error_t mrl_init_ogl_330_render_device(const mrl_render_device_desc_t* desc, mrl_render_device_t ** out_rd)
{
	MGL_DEBUG_ASSERT(desc != NULL && out_rd != NULL);
	MGL_DEBUG_ASSERT(desc->allocator != NULL && desc->window != NULL);

#ifndef MRL_BUILD_OGL_330
	return MRL_ERROR_UNSUPPORTED_DEVICE;
#else

	// Allocate render device
	mrl_ogl_330_render_device_t* rd;
	mgl_error_t mglerr = mgl_allocate(desc->allocator, sizeof(mrl_ogl_330_render_device_t), (void**)&rd);
	if (mglerr != MGL_ERROR_NONE)
		return mrl_make_mgl_error(mglerr);

	rd->allocator = desc->allocator;
	rd->window = desc->window;

	// Extract hints
	extract_hints(rd, desc);

	// Create allocators
	mrl_error_t err = create_rd_allocators(rd, desc);
	if (err != MRL_ERROR_NONE)
	{
		mgl_deallocate(rd->allocator, rd);
		return err;
	}

	// Init context
	err = create_gl_context(rd, desc);
	if (err != MRL_ERROR_NONE)
	{
		destroy_rd_allocators(rd);
		mgl_deallocate(rd->allocator, rd);
		return err;
	}

	// Set render device funcs
	set_rd_functions(rd);

	*out_rd = (mrl_render_device_t*)rd;

	return MRL_ERROR_NONE;
#endif
}

MRL_API void mrl_terminate_ogl_330_render_device(mrl_render_device_t * brd)
{
	MGL_DEBUG_ASSERT(brd != NULL);

#ifndef MRL_BUILD_OGL_330
	mgl_abort();
#else
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Terminate context
	destroy_gl_context(rd);

	// Destroy allocators
	destroy_rd_allocators(rd);

	// Deallocate
	MGL_DEBUG_ASSERT(mgl_deallocate(rd->allocator, rd) == MRL_ERROR_NONE);
#endif
}
