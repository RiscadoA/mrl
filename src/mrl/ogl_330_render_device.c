#include <mrl/ogl_330_render_device.h>

#include <mgl/memory/allocator.h>
#include <mgl/memory/manipulation.h>
#include <mgl/memory/pool_allocator.h>
#include <mgl/string/manipulation.h>
#include <mgl/input/window.h>
#include <mgl/math/scalar.h>

#ifdef MRL_BUILD_OGL_330
#	ifdef MGL_SYSTEM_WINDOWS
#		include <mgl/input/windows_window.h>
#		define GLEW_STATIC
#		include <GL/glew.h>
#		include <GL/wglew.h>
#	endif

static const mgl_chr8_t* opengl_error_code_to_str(GLenum err)
{
	switch (err)
	{
		case 0: return u8"GLEW_OK";
		case GL_INVALID_ENUM: return u8"GL_INVALID_ENUM";
		case GL_INVALID_VALUE: return u8"GL_INVALID_VALUE";
		case GL_INVALID_OPERATION: return u8"GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW: return u8"GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW: return u8"GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY: return u8"GL_OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return u8"GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_CONTEXT_LOST: return u8"GL_CONTEXT_LOST";
		default: return u8"GL_?";
	}
}

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
		} framebuffer;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} raster_state;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} depth_stencil_state;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} blend_state;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} sampler;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} texture_1d;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} texture_2d;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} texture_3d;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} cube_map;

		struct
		{
			mgl_pool_allocator_t pool;
			mgl_u8_t* data;
		} constant_buffer;

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
} mrl_ogl_330_framebuffer_t;

typedef struct
{
	GLuint id;
} mrl_ogl_330_raster_state_t;

typedef struct
{
	GLuint id;
} mrl_ogl_330_depth_stencil_state_t;

typedef struct
{
	GLuint id;
} mrl_ogl_330_blend_state_t;

typedef struct
{
	GLuint id;
} mrl_ogl_330_sampler_t;

typedef struct
{
	GLenum internal_format, format, type;
	mgl_u64_t width;
	GLuint id;
} mrl_ogl_330_texture_1d_t;

typedef struct
{
	GLenum internal_format, format, type;
	mgl_u64_t width, height;
	GLuint id;
} mrl_ogl_330_texture_2d_t;

typedef struct
{
	GLenum internal_format, format, type;
	mgl_u64_t width, height, depth;
	GLuint id;
} mrl_ogl_330_texture_3d_t;

typedef struct
{
	GLenum internal_format, format, type;
	mgl_u64_t width, height;
	GLuint id;
} mrl_ogl_330_cube_map_t;

typedef struct
{
	GLuint id;
} mrl_ogl_330_constant_buffer_t;

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

#define MRL_OGL_330_SHADER_BINDING_POINT_MAX_NAME_SIZE 32
#define MRL_OGL_330_SHADER_MAX_BINDING_POINT_COUNT 32

typedef struct mrl_ogl_330_shader_pipeline_t mrl_ogl_330_shader_pipeline_t;

typedef struct
{
	mgl_chr8_t name[MRL_OGL_330_SHADER_BINDING_POINT_MAX_NAME_SIZE];
	GLint loc;
	mrl_ogl_330_shader_pipeline_t* pp;
} mrl_ogl_330_shader_binding_point_t;

struct mrl_ogl_330_shader_pipeline_t
{
	GLuint id;
	mrl_ogl_330_shader_binding_point_t bps[MRL_OGL_330_SHADER_MAX_BINDING_POINT_COUNT];
};

// ---------- Framebuffers ----------

static mrl_error_t create_framebuffer(mrl_render_device_t* brd, mrl_framebuffer_t** fb, const mrl_framebuffer_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Check for input errors
	if (desc->target_count == 0)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create framebuffer: target count must be at least 1");
		return MRL_ERROR_INVALID_PARAMS;
	}
	else if (desc->target_count > MRL_MAX_FRAMEBUFFER_RENDER_TARGET_COUNT)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create framebuffer: maximum target count surpassed");
		return MRL_ERROR_INVALID_PARAMS;
	}

	for (mgl_u32_t i = 0; i < desc->target_count; ++i)
	{
		if ((desc->targets[i].type == MRL_RENDER_TARGET_TYPE_TEXTURE_2D && desc->targets[i].tex_2d.handle == NULL) ||
			(desc->targets[i].type == MRL_RENDER_TARGET_TYPE_CUBE_MAP && desc->targets[i].cube_map.handle == NULL))
		{
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create framebuffer: defined target cannot be NULL");
			return MRL_ERROR_INVALID_PARAMS;
		}

		if (desc->targets[i].type == MRL_RENDER_TARGET_TYPE_CUBE_MAP &&
			(desc->targets[i].cube_map.face < MRL_CUBE_MAP_FACE_POSITIVE_X || desc->targets[i].cube_map.face > MRL_CUBE_MAP_FACE_NEGATIVE_Z))
		{
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create framebuffer: invalid cube map target face");
			return MRL_ERROR_INVALID_PARAMS;
		}

		if (desc->targets[i].type != MRL_RENDER_TARGET_TYPE_TEXTURE_2D &&
			desc->targets[i].type != MRL_RENDER_TARGET_TYPE_CUBE_MAP)
		{
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create framebuffer: invalid target type");
			return MRL_ERROR_INVALID_PARAMS;
		}
	}

	// Initialize framebuffer
	GLuint id;
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	for (mgl_u32_t i = 0; i < desc->target_count; ++i)
	{
		if (desc->targets[i].type == MRL_RENDER_TARGET_TYPE_TEXTURE_2D)
		{
			mrl_ogl_330_texture_2d_t* tex = (mrl_ogl_330_texture_2d_t*)desc->targets[i].tex_2d.handle;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tex->id, 0);
		}
		else if (desc->targets[i].type == MRL_RENDER_TARGET_TYPE_CUBE_MAP)
		{
			mrl_ogl_330_cube_map_t* cb = (mrl_ogl_330_cube_map_t*)desc->targets[i].cube_map.handle;
			GLenum face;

			switch (desc->targets[i].cube_map.face)
			{
				case MRL_CUBE_MAP_FACE_POSITIVE_X: face = GL_TEXTURE_CUBE_MAP_POSITIVE_X; break;
				case MRL_CUBE_MAP_FACE_NEGATIVE_X: face = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; break;
				case MRL_CUBE_MAP_FACE_POSITIVE_Y: face = GL_TEXTURE_CUBE_MAP_POSITIVE_Y; break;
				case MRL_CUBE_MAP_FACE_NEGATIVE_Y: face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; break;
				case MRL_CUBE_MAP_FACE_POSITIVE_Z: face = GL_TEXTURE_CUBE_MAP_POSITIVE_Z; break;
				case MRL_CUBE_MAP_FACE_NEGATIVE_Z: face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; break;
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, face, cb->id, 0);
		}
	}

	if (desc->depth_stencil != NULL)
	{
		mrl_ogl_330_texture_2d_t* tex = (mrl_ogl_330_texture_2d_t*)desc->depth_stencil;
		
		if (tex->format == GL_DEPTH_COMPONENT)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex->id, 0);
		else if (tex->format == GL_DEPTH_STENCIL)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, tex->id, 0);
		else
		{
			glDeleteFramebuffers(1, &id);
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create framebuffer: invalid depth/stencil texture format");
			return MRL_ERROR_INVALID_PARAMS;
		}
	}

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteFramebuffers(1, &id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glDeleteFramebuffers(1, &id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, u8"Failed to create framebuffer: glCheckFramebufferStatus didn't return GL_FRAMEBUFFER_COMPLETE");
		return MRL_ERROR_EXTERNAL;
	}

	// Allocate object
	mrl_ogl_330_framebuffer_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.framebuffer.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteFramebuffers(1, &id);
		return mrl_make_mgl_error(err);
	}

	// Store framebuffer info
	obj->id = id;
	*fb = (mrl_framebuffer_t*)obj;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return MRL_ERROR_NONE;
}

static void destroy_framebuffer(mrl_render_device_t* brd, mrl_framebuffer_t* fb)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_framebuffer_t* obj = (mrl_ogl_330_framebuffer_t*)fb;

	// Delete framebuffer
	glDeleteFramebuffers(1, &obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.framebuffer.pool,
		obj);
}

static void set_framebuffer(mrl_render_device_t* brd, mrl_framebuffer_t* fb)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_framebuffer_t* obj = (mrl_ogl_330_framebuffer_t*)fb;

	// Set framebuffer
	if (obj == NULL)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	else
		glBindFramebuffer(GL_FRAMEBUFFER, obj->id);
}

// ---------- Samplers ----------

static mrl_error_t create_sampler(mrl_render_device_t* brd, mrl_sampler_t** s, const mrl_sampler_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Get address modes
	GLenum address_u, address_v, address_w;

	switch (desc->address_u)
	{
		case MRL_SAMPLER_ADDRESS_REPEAT: address_u = GL_REPEAT; break;
		case MRL_SAMPLER_ADDRESS_MIRROR: address_u = GL_MIRRORED_REPEAT; break;
		case MRL_SAMPLER_ADDRESS_CLAMP: address_u = GL_CLAMP; break;
		case MRL_SAMPLER_ADDRESS_BORDER: address_u = GL_CLAMP_TO_BORDER; break;
		default:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create sampler: invalid U address mode");
			return MRL_ERROR_INVALID_PARAMS;
	}

	switch (desc->address_v)
	{
		case MRL_SAMPLER_ADDRESS_REPEAT: address_v = GL_REPEAT; break;
		case MRL_SAMPLER_ADDRESS_MIRROR: address_v = GL_MIRRORED_REPEAT; break;
		case MRL_SAMPLER_ADDRESS_CLAMP: address_v = GL_CLAMP; break;
		case MRL_SAMPLER_ADDRESS_BORDER: address_v = GL_CLAMP_TO_BORDER; break;
		default:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create sampler: invalid V address mode");
			return MRL_ERROR_INVALID_PARAMS;
	}

	switch (desc->address_w)
	{
		case MRL_SAMPLER_ADDRESS_REPEAT: address_w = GL_REPEAT; break;
		case MRL_SAMPLER_ADDRESS_MIRROR: address_w = GL_MIRRORED_REPEAT; break;
		case MRL_SAMPLER_ADDRESS_CLAMP: address_w = GL_CLAMP; break;
		case MRL_SAMPLER_ADDRESS_BORDER: address_w = GL_CLAMP_TO_BORDER; break;
		default:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create sampler: invalid W address mode");
			return MRL_ERROR_INVALID_PARAMS;
	}

	// Get filter

	GLenum min_filter, mag_filter;

	switch (desc->min_filter)
	{
		case MRL_SAMPLER_FILTER_NEAREST:
			if (desc->mip_filter == GL_NONE)
				min_filter = GL_NEAREST;
			else if (desc->mip_filter == GL_NEAREST)
				min_filter = GL_NEAREST_MIPMAP_NEAREST;
			else if (desc->mip_filter == GL_LINEAR)
				min_filter = GL_NEAREST_MIPMAP_LINEAR;
			else
			{
				if (rd->error_callback != NULL)
					rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create sampler: invalid mipmap filter");
				return MRL_ERROR_INVALID_PARAMS;
			}
			break;

		case MRL_SAMPLER_FILTER_LINEAR:
			if (desc->mip_filter == GL_NONE)
				min_filter = GL_LINEAR;
			else if (desc->mip_filter == GL_NEAREST)
				min_filter = GL_LINEAR_MIPMAP_NEAREST;
			else if (desc->mip_filter == GL_LINEAR)
				min_filter = GL_LINEAR_MIPMAP_LINEAR;
			else
			{
				if (rd->error_callback != NULL)
					rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create sampler: invalid mipmap filter");
				return MRL_ERROR_INVALID_PARAMS;
			}
			break;

		default:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create sampler: invalid miniying filter");
			return MRL_ERROR_INVALID_PARAMS;
	}

	switch (desc->mag_filter)
	{
		case MRL_SAMPLER_FILTER_NEAREST: mag_filter = GL_NEAREST; break;
		case MRL_SAMPLER_FILTER_LINEAR: mag_filter = GL_LINEAR; break;
		default:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create sampler: invalid magnifying filter");
			return MRL_ERROR_INVALID_PARAMS;
	}

	// Initialize sampler
	GLuint id;
	glGenSamplers(1, &id);
	glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, min_filter);
	glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, mag_filter);
	if (GL_ARB_texture_filter_anisotropic)
		glSamplerParameteri(id, GL_TEXTURE_MAX_ANISOTROPY, desc->max_anisotropy);
	glSamplerParameteri(id, GL_TEXTURE_WRAP_S, address_u);
	glSamplerParameteri(id, GL_TEXTURE_WRAP_T, address_v);
	glSamplerParameteri(id, GL_TEXTURE_WRAP_R, address_w);
	glSamplerParameterfv(id, GL_TEXTURE_BORDER_COLOR, desc->border_color);

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteBuffers(1, &id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

	// Allocate object
	mrl_ogl_330_sampler_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.sampler.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteBuffers(1, &id);
		return mrl_make_mgl_error(err);
	}

	// Store sampler info
	obj->id = id;
	*s = (mrl_sampler_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_sampler(mrl_render_device_t* brd, mrl_sampler_t* s)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_sampler_t* obj = (mrl_ogl_330_sampler_t*)s;

	// Delete sampler
	glDeleteBuffers(1, &obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.sampler.pool,
		obj);
}

static void bind_sampler(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_sampler_t* s)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_sampler_t* obj = (mrl_ogl_330_sampler_t*)s;
	mrl_ogl_330_shader_binding_point_t* rbp = (mrl_ogl_330_shader_binding_point_t*)bp;

	// Bind sampler
	if (s == NULL)
		glBindSampler(rbp->loc, 0);
	else
		glBindSampler(rbp->loc, obj->id);
}

// ---------- Texture 1D ----------

static mrl_error_t create_texture_1d(mrl_render_device_t* brd, mrl_texture_1d_t** tex, const mrl_texture_1d_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Get internal format, format and type
	GLenum internal_format, format, type;

	switch (desc->format)
	{
		case MRL_TEXTURE_FORMAT_R8_UN: internal_format = GL_R8; format = GL_R; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_SN: internal_format = GL_R8_SNORM; format = GL_R; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_UI: internal_format = GL_R8UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_SI: internal_format = GL_R8I; format = GL_RED_INTEGER; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_UN: internal_format = GL_RG8; format = GL_RG; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_SN: internal_format = GL_RG8_SNORM; format = GL_RG; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_UI: internal_format = GL_RG8UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_SI: internal_format = GL_RG8I; format = GL_RG_INTEGER; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_UN: internal_format = GL_RGBA8; format = GL_RGBA; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_SN: internal_format = GL_RGBA8_SNORM; format = GL_RGBA; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_UI: internal_format = GL_RGBA8UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_SI: internal_format = GL_RGBA8I; format = GL_RGBA_INTEGER; type = GL_BYTE; break;

		case MRL_TEXTURE_FORMAT_R16_UN: internal_format = GL_R16; format = GL_R; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_SN: internal_format = GL_R16_SNORM; format = GL_R; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_UI: internal_format = GL_R16UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_SI: internal_format = GL_R16I; format = GL_RED_INTEGER; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_UN: internal_format = GL_RG16; format = GL_RG; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_SN: internal_format = GL_RG16_SNORM; format = GL_RG; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_UI: internal_format = GL_RG16UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_SI: internal_format = GL_RG16I; format = GL_RG_INTEGER; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_UN: internal_format = GL_RGBA16; format = GL_RGBA; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_SN: internal_format = GL_RGBA16_SNORM; format = GL_RGBA; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_UI: internal_format = GL_RGBA16UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_SI: internal_format = GL_RGBA16I; format = GL_RGBA_INTEGER; type = GL_SHORT; break;

		case MRL_TEXTURE_FORMAT_R32_UI: internal_format = GL_R32UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_R32_SI: internal_format = GL_R32I; format = GL_RED_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_R32_F: internal_format = GL_R32F; format = GL_R; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_RG32_UI: internal_format = GL_RG32UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_RG32_SI: internal_format = GL_RG32I; format = GL_RG_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_RG32_F: internal_format = GL_RG32F; format = GL_RG; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_UI: internal_format = GL_RGBA32UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_SI: internal_format = GL_RGBA32I; format = GL_RGBA_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_F: internal_format = GL_RGBA32F; format = GL_RGBA; type = GL_FLOAT; break;

		case MRL_TEXTURE_FORMAT_D16:
		case MRL_TEXTURE_FORMAT_D32:
		case MRL_TEXTURE_FORMAT_D24S8:
		case MRL_TEXTURE_FORMAT_D32S8:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create 1D texture: depth/stencil format is not supported on 1D textures");
			return MRL_ERROR_INVALID_PARAMS;

		default:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create 1D texture: invalid format");
			return MRL_ERROR_INVALID_PARAMS;
	}

	// Initialize texture
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_1D, id);
	for (mgl_u32_t i = 0, div = 1; i < desc->mip_level_count; ++i, div *= 2)
		glTexImage1D(GL_TEXTURE_1D, i, internal_format, (GLsizei)(desc->width / div), 0, format, type, desc->data[i]);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteTextures(1, &id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

	// Allocate object
	mrl_ogl_330_texture_1d_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.texture_1d.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteTextures(1, &id);
		return mrl_make_mgl_error(err);
	}

	// Store texture info
	obj->id = id;
	obj->width = desc->width;
	obj->internal_format = internal_format;
	obj->format = format;
	obj->type = type;
	*tex = (mrl_texture_1d_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_texture_1d(mrl_render_device_t* brd, mrl_texture_1d_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_1d_t* obj = (mrl_ogl_330_texture_1d_t*)tex;

	// Delete texture
	glDeleteTextures(1, &obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.texture_1d.pool,
		obj);
}

static void generate_texture_1d_mipmaps(mrl_render_device_t* brd, mrl_texture_2d_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_1d_t* obj = (mrl_ogl_330_texture_1d_t*)tex;

	glBindTexture(GL_TEXTURE_1D, obj->id);
	glGenerateMipmap(GL_TEXTURE_1D);
}

static void bind_texture_1d(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_texture_1d_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_1d_t* obj = (mrl_ogl_330_texture_1d_t*)tex;
	mrl_ogl_330_shader_binding_point_t* rbp = (mrl_ogl_330_shader_binding_point_t*)bp;

	// Bind texture
	glActiveTexture(GL_TEXTURE0 + rbp->loc);
	if (tex == NULL)
		glBindTexture(GL_TEXTURE_1D, 0);
	else
		glBindTexture(GL_TEXTURE_1D, obj->id);
	glUniform1i(rbp->loc, rbp->loc);
}

static mrl_error_t update_texture_1d(mrl_render_device_t* brd, mrl_texture_1d_t* tex, const mrl_texture_1d_update_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_1d_t* obj = (mrl_ogl_330_texture_1d_t*)tex;

	// Update texture
	glBindTexture(GL_TEXTURE_1D, obj->id);
	glTexSubImage1D(GL_TEXTURE_1D, desc->mip_level, (GLint)desc->dst_x, (GLsizei)desc->width, obj->format, obj->type, desc->data);

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

	return MRL_ERROR_NONE;
}

// ---------- Texture 2D ----------

static mrl_error_t create_texture_2d(mrl_render_device_t* brd, mrl_texture_2d_t** tex, const mrl_texture_2d_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Get internal format, format and type
	GLenum internal_format, format, type;

	switch (desc->format)
	{
		case MRL_TEXTURE_FORMAT_R8_UN: internal_format = GL_R8; format = GL_R; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_SN: internal_format = GL_R8_SNORM; format = GL_R; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_UI: internal_format = GL_R8UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_SI: internal_format = GL_R8I; format = GL_RED_INTEGER; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_UN: internal_format = GL_RG8; format = GL_RG; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_SN: internal_format = GL_RG8_SNORM; format = GL_RG; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_UI: internal_format = GL_RG8UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_SI: internal_format = GL_RG8I; format = GL_RG_INTEGER; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_UN: internal_format = GL_RGBA8; format = GL_RGBA; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_SN: internal_format = GL_RGBA8_SNORM; format = GL_RGBA; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_UI: internal_format = GL_RGBA8UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_SI: internal_format = GL_RGBA8I; format = GL_RGBA_INTEGER; type = GL_BYTE; break;

		case MRL_TEXTURE_FORMAT_R16_UN: internal_format = GL_R16; format = GL_R; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_SN: internal_format = GL_R16_SNORM; format = GL_R; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_UI: internal_format = GL_R16UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_SI: internal_format = GL_R16I; format = GL_RED_INTEGER; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_UN: internal_format = GL_RG16; format = GL_RG; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_SN: internal_format = GL_RG16_SNORM; format = GL_RG; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_UI: internal_format = GL_RG16UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_SI: internal_format = GL_RG16I; format = GL_RG_INTEGER; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_UN: internal_format = GL_RGBA16; format = GL_RGBA; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_SN: internal_format = GL_RGBA16_SNORM; format = GL_RGBA; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_UI: internal_format = GL_RGBA16UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_SI: internal_format = GL_RGBA16I; format = GL_RGBA_INTEGER; type = GL_SHORT; break;

		case MRL_TEXTURE_FORMAT_R32_UI: internal_format = GL_R32UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_R32_SI: internal_format = GL_R32I; format = GL_RED_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_R32_F: internal_format = GL_R32F; format = GL_R; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_RG32_UI: internal_format = GL_RG32UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_RG32_SI: internal_format = GL_RG32I; format = GL_RG_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_RG32_F: internal_format = GL_RG32F; format = GL_RG; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_UI: internal_format = GL_RGBA32UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_SI: internal_format = GL_RGBA32I; format = GL_RGBA_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_F: internal_format = GL_RGBA32F; format = GL_RGBA; type = GL_FLOAT; break;

		case MRL_TEXTURE_FORMAT_D16: internal_format = GL_DEPTH; format = GL_DEPTH_COMPONENT; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_D32: internal_format = GL_DEPTH; format = GL_DEPTH_COMPONENT; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_D24S8: internal_format = GL_DEPTH24_STENCIL8; format = GL_DEPTH_STENCIL; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_D32S8: internal_format = GL_DEPTH32F_STENCIL8; format = GL_DEPTH_STENCIL; type = GL_FLOAT; break;

		default:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create 2D texture: invalid format");
			return MRL_ERROR_INVALID_PARAMS;
	}

	// Initialize texture
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	for (mgl_u32_t i = 0, div = 1; i < desc->mip_level_count; ++i, div *= 2)
		glTexImage2D(GL_TEXTURE_2D, i, internal_format, (GLsizei)(desc->width / div), (GLsizei)(desc->height / div), 0, format, type, desc->data[i]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteTextures(1, &id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

	// Allocate object
	mrl_ogl_330_texture_2d_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.texture_2d.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteTextures(1, &id);
		return mrl_make_mgl_error(err);
	}

	// Store texture info
	obj->id = id;
	obj->width = desc->width;
	obj->height = desc->height;
	obj->internal_format = internal_format;
	obj->format = format;
	obj->type = type;
	*tex = (mrl_texture_2d_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_texture_2d(mrl_render_device_t* brd, mrl_texture_2d_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_2d_t* obj = (mrl_ogl_330_texture_2d_t*)tex;

	// Delete texture
	glDeleteTextures(1, &obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.texture_2d.pool,
		obj);
}

static void generate_texture_2d_mipmaps(mrl_render_device_t* brd, mrl_texture_2d_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_2d_t* obj = (mrl_ogl_330_texture_2d_t*)tex;

	glBindTexture(GL_TEXTURE_2D, obj->id);
	glGenerateMipmap(GL_TEXTURE_2D);
}

static void bind_texture_2d(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_texture_2d_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_2d_t* obj = (mrl_ogl_330_texture_2d_t*)tex;
	mrl_ogl_330_shader_binding_point_t* rbp = (mrl_ogl_330_shader_binding_point_t*)bp;

	// Bind texture
	glActiveTexture(GL_TEXTURE0 + rbp->loc);
	if (tex == NULL)
		glBindTexture(GL_TEXTURE_2D, 0);
	else
		glBindTexture(GL_TEXTURE_2D, obj->id);
	glUniform1i(rbp->loc, rbp->loc);
}

static mrl_error_t update_texture_2d(mrl_render_device_t* brd, mrl_texture_2d_t* tex, const mrl_texture_2d_update_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_2d_t* obj = (mrl_ogl_330_texture_2d_t*)tex;

	// Update texture
	glBindTexture(GL_TEXTURE_2D, obj->id);
	glTexSubImage2D(GL_TEXTURE_2D, desc->mip_level, (GLint)desc->dst_x, (GLint)desc->dst_y, (GLsizei)desc->width, (GLsizei)desc->height, obj->format, obj->type, desc->data);

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

	return MRL_ERROR_NONE;
}

// ---------- Texture 3D ----------

static mrl_error_t create_texture_3d(mrl_render_device_t* brd, mrl_texture_3d_t** tex, const mrl_texture_3d_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Get internal format, format and type
	GLenum internal_format, format, type;

	switch (desc->format)
	{
		case MRL_TEXTURE_FORMAT_R8_UN: internal_format = GL_R8; format = GL_R; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_SN: internal_format = GL_R8_SNORM; format = GL_R; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_UI: internal_format = GL_R8UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_SI: internal_format = GL_R8I; format = GL_RED_INTEGER; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_UN: internal_format = GL_RG8; format = GL_RG; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_SN: internal_format = GL_RG8_SNORM; format = GL_RG; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_UI: internal_format = GL_RG8UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_SI: internal_format = GL_RG8I; format = GL_RG_INTEGER; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_UN: internal_format = GL_RGBA8; format = GL_RGBA; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_SN: internal_format = GL_RGBA8_SNORM; format = GL_RGBA; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_UI: internal_format = GL_RGBA8UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_SI: internal_format = GL_RGBA8I; format = GL_RGBA_INTEGER; type = GL_BYTE; break;
	
		case MRL_TEXTURE_FORMAT_R16_UN: internal_format = GL_R16; format = GL_R; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_SN: internal_format = GL_R16_SNORM; format = GL_R; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_UI: internal_format = GL_R16UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_SI: internal_format = GL_R16I; format = GL_RED_INTEGER; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_UN: internal_format = GL_RG16; format = GL_RG; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_SN: internal_format = GL_RG16_SNORM; format = GL_RG; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_UI: internal_format = GL_RG16UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_SI: internal_format = GL_RG16I; format = GL_RG_INTEGER; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_UN: internal_format = GL_RGBA16; format = GL_RGBA; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_SN: internal_format = GL_RGBA16_SNORM; format = GL_RGBA; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_UI: internal_format = GL_RGBA16UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_SI: internal_format = GL_RGBA16I; format = GL_RGBA_INTEGER; type = GL_SHORT; break;

		case MRL_TEXTURE_FORMAT_R32_UI: internal_format = GL_R32UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_R32_SI: internal_format = GL_R32I; format = GL_RED_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_R32_F: internal_format = GL_R32F; format = GL_R; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_RG32_UI: internal_format = GL_RG32UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_RG32_SI: internal_format = GL_RG32I; format = GL_RG_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_RG32_F: internal_format = GL_RG32F; format = GL_RG; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_UI: internal_format = GL_RGBA32UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_SI: internal_format = GL_RGBA32I; format = GL_RGBA_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_F: internal_format = GL_RGBA32F; format = GL_RGBA; type = GL_FLOAT; break;
	
		case MRL_TEXTURE_FORMAT_D16:
		case MRL_TEXTURE_FORMAT_D32:
		case MRL_TEXTURE_FORMAT_D24S8:
		case MRL_TEXTURE_FORMAT_D32S8:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create 3D texture: depth/stencil format is not supported on 3D textures");
			return MRL_ERROR_INVALID_PARAMS;

		default:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create 3D texture: invalid format");
			return MRL_ERROR_INVALID_PARAMS;
	}

	// Initialize texture
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_3D, id);
	for (mgl_u32_t i = 0, div = 1; i < desc->mip_level_count; ++i, div *= 2)
		glTexImage3D(GL_TEXTURE_3D, i, internal_format, (GLsizei)(desc->width / div), (GLsizei)(desc->height / div), (GLsizei)(desc->depth / div), 0, format, type, desc->data[i]);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteTextures(1, &id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

	// Allocate object
	mrl_ogl_330_texture_3d_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.texture_3d.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteTextures(1, &id);
		return mrl_make_mgl_error(err);
	}

	// Store texture info
	obj->id = id;
	obj->width = desc->width;
	obj->height = desc->height;
	obj->depth = desc->depth;
	obj->internal_format = internal_format;
	obj->format = format;
	obj->type = type;
	*tex = (mrl_texture_3d_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_texture_3d(mrl_render_device_t* brd, mrl_texture_3d_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_3d_t* obj = (mrl_ogl_330_texture_3d_t*)tex;

	// Delete texture
	glDeleteTextures(1, &obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.texture_3d.pool,
		obj);
}

static void generate_texture_3d_mipmaps(mrl_render_device_t* brd, mrl_texture_2d_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_3d_t* obj = (mrl_ogl_330_texture_3d_t*)tex;

	glBindTexture(GL_TEXTURE_3D, obj->id);
	glGenerateMipmap(GL_TEXTURE_3D);
}

static void bind_texture_3d(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_texture_3d_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_3d_t* obj = (mrl_ogl_330_texture_3d_t*)tex;
	mrl_ogl_330_shader_binding_point_t* rbp = (mrl_ogl_330_shader_binding_point_t*)bp;

	// Bind texture
	glActiveTexture(GL_TEXTURE0 + rbp->loc);
	if (tex == NULL)
		glBindTexture(GL_TEXTURE_3D, 0);
	else
		glBindTexture(GL_TEXTURE_3D, obj->id);
	glUniform1i(rbp->loc, rbp->loc);
}

static mrl_error_t update_texture_3d(mrl_render_device_t* brd, mrl_texture_3d_t* tex, const mrl_texture_3d_update_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_texture_3d_t* obj = (mrl_ogl_330_texture_3d_t*)tex;

	// Update texture
	glBindTexture(GL_TEXTURE_3D, obj->id);
	glTexSubImage3D(GL_TEXTURE_3D, desc->mip_level, (GLint)desc->dst_x, (GLint)desc->dst_y, (GLint)desc->dst_z, (GLsizei)desc->width, (GLsizei)desc->height, (GLsizei)desc->depth, obj->format, obj->type, desc->data);

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

	return MRL_ERROR_NONE;
}

// ---------- Cube Map ----------

static mrl_error_t create_cube_map(mrl_render_device_t* brd, mrl_cube_map_t** tex, const mrl_cube_map_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Get internal format, format and type
	GLenum internal_format, format, type;

	switch (desc->format)
	{
		case MRL_TEXTURE_FORMAT_R8_UN: internal_format = GL_R8; format = GL_R; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_SN: internal_format = GL_R8_SNORM; format = GL_R; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_UI: internal_format = GL_R8UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_R8_SI: internal_format = GL_R8I; format = GL_RED_INTEGER; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_UN: internal_format = GL_RG8; format = GL_RG; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_SN: internal_format = GL_RG8_SNORM; format = GL_RG; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_UI: internal_format = GL_RG8UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RG8_SI: internal_format = GL_RG8I; format = GL_RG_INTEGER; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_UN: internal_format = GL_RGBA8; format = GL_RGBA; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_SN: internal_format = GL_RGBA8_SNORM; format = GL_RGBA; type = GL_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_UI: internal_format = GL_RGBA8UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_BYTE; break;
		case MRL_TEXTURE_FORMAT_RGBA8_SI: internal_format = GL_RGBA8I; format = GL_RGBA_INTEGER; type = GL_BYTE; break;

		case MRL_TEXTURE_FORMAT_R16_UN: internal_format = GL_R16; format = GL_R; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_SN: internal_format = GL_R16_SNORM; format = GL_R; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_UI: internal_format = GL_R16UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_R16_SI: internal_format = GL_R16I; format = GL_RED_INTEGER; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_UN: internal_format = GL_RG16; format = GL_RG; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_SN: internal_format = GL_RG16_SNORM; format = GL_RG; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_UI: internal_format = GL_RG16UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RG16_SI: internal_format = GL_RG16I; format = GL_RG_INTEGER; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_UN: internal_format = GL_RGBA16; format = GL_RGBA; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_SN: internal_format = GL_RGBA16_SNORM; format = GL_RGBA; type = GL_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_UI: internal_format = GL_RGBA16UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_SHORT; break;
		case MRL_TEXTURE_FORMAT_RGBA16_SI: internal_format = GL_RGBA16I; format = GL_RGBA_INTEGER; type = GL_SHORT; break;

		case MRL_TEXTURE_FORMAT_R32_UI: internal_format = GL_R32UI; format = GL_RED_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_R32_SI: internal_format = GL_R32I; format = GL_RED_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_R32_F: internal_format = GL_R32F; format = GL_R; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_RG32_UI: internal_format = GL_RG32UI; format = GL_RG_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_RG32_SI: internal_format = GL_RG32I; format = GL_RG_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_RG32_F: internal_format = GL_RG32F; format = GL_RG; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_UI: internal_format = GL_RGBA32UI; format = GL_RGBA_INTEGER; type = GL_UNSIGNED_INT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_SI: internal_format = GL_RGBA32I; format = GL_RGBA_INTEGER; type = GL_INT; break;
		case MRL_TEXTURE_FORMAT_RGBA32_F: internal_format = GL_RGBA32F; format = GL_RGBA; type = GL_FLOAT; break;

		case MRL_TEXTURE_FORMAT_D16: internal_format = GL_DEPTH; format = GL_DEPTH_COMPONENT; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_D32: internal_format = GL_DEPTH; format = GL_DEPTH_COMPONENT; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_D24S8: internal_format = GL_R32UI; format = GL_DEPTH_STENCIL; type = GL_FLOAT; break;
		case MRL_TEXTURE_FORMAT_D32S8: internal_format = GL_R32UI; format = GL_DEPTH_STENCIL; type = GL_FLOAT; break;

		default:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create 2D texture: invalid format");
			return MRL_ERROR_INVALID_PARAMS;
	}

	// Initialize texture
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	for (mgl_u32_t i = 0, div = 1; i < desc->mip_level_count; ++i, div *= 2)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, i, internal_format, (GLsizei)(desc->width / div), (GLsizei)(desc->height / div), 0, format, type, desc->data[MRL_CUBE_MAP_FACE_POSITIVE_X][i]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, i, internal_format, (GLsizei)(desc->width / div), (GLsizei)(desc->height / div), 0, format, type, desc->data[MRL_CUBE_MAP_FACE_NEGATIVE_X][i]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, i, internal_format, (GLsizei)(desc->width / div), (GLsizei)(desc->height / div), 0, format, type, desc->data[MRL_CUBE_MAP_FACE_POSITIVE_Y][i]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, i, internal_format, (GLsizei)(desc->width / div), (GLsizei)(desc->height / div), 0, format, type, desc->data[MRL_CUBE_MAP_FACE_NEGATIVE_Y][i]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, i, internal_format, (GLsizei)(desc->width / div), (GLsizei)(desc->height / div), 0, format, type, desc->data[MRL_CUBE_MAP_FACE_POSITIVE_Z][i]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, i, internal_format, (GLsizei)(desc->width / div), (GLsizei)(desc->height / div), 0, format, type, desc->data[MRL_CUBE_MAP_FACE_NEGATIVE_Z][i]);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteTextures(1, &id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

	// Allocate object
	mrl_ogl_330_cube_map_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.cube_map.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteTextures(1, &id);
		return mrl_make_mgl_error(err);
	}

	// Store texture info
	obj->id = id;
	obj->width = desc->width;
	obj->height = desc->height;
	obj->internal_format = internal_format;
	obj->format = format;
	obj->type = type;
	*tex = (mrl_cube_map_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_cube_map(mrl_render_device_t* brd, mrl_cube_map_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_cube_map_t* obj = (mrl_ogl_330_cube_map_t*)tex;

	// Delete texture
	glDeleteTextures(1, &obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.cube_map.pool,
		obj);
}

static void generate_cube_map_mipmaps(mrl_render_device_t* brd, mrl_cube_map_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_cube_map_t* obj = (mrl_ogl_330_cube_map_t*)tex;

	glBindTexture(GL_TEXTURE_CUBE_MAP, obj->id);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

static void bind_cube_map(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_cube_map_t* tex)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_cube_map_t* obj = (mrl_ogl_330_cube_map_t*)tex;
	mrl_ogl_330_shader_binding_point_t* rbp = (mrl_ogl_330_shader_binding_point_t*)bp;

	// Bind texture
	glActiveTexture(GL_TEXTURE0 + rbp->loc);
	if (tex == NULL)
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	else
		glBindTexture(GL_TEXTURE_CUBE_MAP, obj->id);
	glUniform1i(rbp->loc, rbp->loc);
}

static mrl_error_t update_cube_map(mrl_render_device_t* brd, mrl_cube_map_t* tex, const mrl_cube_map_update_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_cube_map_t* obj = (mrl_ogl_330_cube_map_t*)tex;

	// Get face
	GLenum face;
	switch (desc->face)
	{
		case MRL_CUBE_MAP_FACE_POSITIVE_X: face = GL_TEXTURE_CUBE_MAP_POSITIVE_X; break;
		case MRL_CUBE_MAP_FACE_NEGATIVE_X: face = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; break;
		case MRL_CUBE_MAP_FACE_POSITIVE_Y: face = GL_TEXTURE_CUBE_MAP_POSITIVE_Y; break;
		case MRL_CUBE_MAP_FACE_NEGATIVE_Y: face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; break;
		case MRL_CUBE_MAP_FACE_POSITIVE_Z: face = GL_TEXTURE_CUBE_MAP_POSITIVE_Z; break;
		case MRL_CUBE_MAP_FACE_NEGATIVE_Z: face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; break;
		default:
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to update cube map: invalid face");
			return MRL_ERROR_INVALID_PARAMS;
	}

	// Update texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, obj->id);
	glTexSubImage2D(face, desc->mip_level, (GLint)desc->dst_x, (GLint)desc->dst_y, (GLsizei)desc->width, (GLsizei)desc->height, obj->format, obj->type, desc->data);

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

	return MRL_ERROR_NONE;
}

// ---------- Constant buffers ----------

static mrl_error_t create_constant_buffer(mrl_render_device_t* brd, mrl_constant_buffer_t** cb, const mrl_constant_buffer_desc_t* desc)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Check for invalid input
	if (desc->usage == MRL_CONSTANT_BUFFER_USAGE_STATIC && desc->data == NULL)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create constant buffer: when the usage mode is set to static, the pointer to the initial data must not be NULL");
		return MRL_ERROR_INVALID_PARAMS;
	}

	// Get usage
	GLenum usage;

	if (desc->usage == MRL_CONSTANT_BUFFER_USAGE_DEFAULT)
		usage = GL_STATIC_DRAW;
	else if (desc->usage == MRL_CONSTANT_BUFFER_USAGE_STATIC)
		usage = GL_STATIC_DRAW;
	else if (desc->usage == MRL_CONSTANT_BUFFER_USAGE_DYNAMIC)
		usage = GL_DYNAMIC_DRAW;
	else if (desc->usage == MRL_CONSTANT_BUFFER_USAGE_STREAM)
		usage = GL_STREAM_DRAW;
	else
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create constant buffer: invalid usage mode");
		return MRL_ERROR_INVALID_PARAMS;
	}

	// Initialize constant buffer
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	if (desc->data == NULL)
		glBufferData(GL_UNIFORM_BUFFER, desc->size, NULL, usage);
	else
		glBufferData(GL_UNIFORM_BUFFER, desc->size, desc->data, usage);

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteBuffers(1, &id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

	// Allocate object
	mrl_ogl_330_constant_buffer_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.constant_buffer.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
	{
		glDeleteBuffers(1, &id);
		return mrl_make_mgl_error(err);
	}

	// Store constant buffer info
	obj->id = id;
	*cb = (mrl_constant_buffer_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_constant_buffer(mrl_render_device_t* brd, mrl_constant_buffer_t* cb)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_constant_buffer_t* obj = (mrl_ogl_330_constant_buffer_t*)cb;

	// Delete constant buffer
	glDeleteBuffers(1, &obj->id);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.constant_buffer.pool,
		obj);
}

static void bind_constant_buffer(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_constant_buffer_t* cb)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_constant_buffer_t* obj = (mrl_ogl_330_constant_buffer_t*)cb;
	mrl_ogl_330_shader_binding_point_t* rbp = (mrl_ogl_330_shader_binding_point_t*)bp;

	// Bind constant buffer
	if (cb == NULL)
		glBindBufferBase(GL_UNIFORM_BUFFER, rbp->loc, 0);
	else
		glBindBufferBase(GL_UNIFORM_BUFFER, rbp->loc, obj->id);
}

static void* map_constant_buffer(mrl_render_device_t* brd, mrl_constant_buffer_t* cb)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_constant_buffer_t* obj = (mrl_ogl_330_constant_buffer_t*)cb;

	// Map UBO
	glBindBuffer(GL_UNIFORM_BUFFER, obj->id);
	return glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
}

static void unmap_constant_buffer(mrl_render_device_t* brd, mrl_constant_buffer_t* cb)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_constant_buffer_t* obj = (mrl_ogl_330_constant_buffer_t*)cb;

	// Unmap UBO
	glBindBuffer(GL_UNIFORM_BUFFER, obj->id);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

static void update_constant_buffer(mrl_render_device_t* brd, mrl_constant_buffer_t* cb, mgl_u64_t offset, mgl_u64_t size, const void* data)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_constant_buffer_t* obj = (mrl_ogl_330_constant_buffer_t*)cb;

	// Update UBO
	glBindBuffer(GL_UNIFORM_BUFFER, obj->id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
	}
}

static void query_constant_buffer_structure(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_constant_buffer_structure_t* cbs)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_shader_binding_point_t* obj = (mrl_ogl_330_shader_binding_point_t*)bp;

	cbs->element_count = 0;

	GLint data_size = 0;
	glGetActiveUniformBlockiv(obj->pp->id, obj->loc, GL_UNIFORM_BLOCK_DATA_SIZE, &data_size);
	cbs->size = data_size;

	GLint element_count = 0;
	glGetActiveUniformBlockiv(obj->pp->id, obj->loc, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &element_count);
	cbs->element_count = element_count;
	
	MGL_DEBUG_ASSERT(cbs->element_count <= MRL_MAX_CONSTANT_BUFFER_ELEMENT_COUNT);

	GLint element_indices[MRL_MAX_CONSTANT_BUFFER_ELEMENT_COUNT];
	glGetActiveUniformBlockiv(obj->pp->id, obj->loc, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, element_indices);

	for (mgl_u64_t i = 0; i < element_count; ++i)
	{
		// Get name
		mgl_chr8_t element_name[128];

		GLint name_len;
		glGetActiveUniformsiv(obj->pp->id, 1, &element_indices[i], GL_UNIFORM_NAME_LENGTH, &name_len);
		MGL_DEBUG_ASSERT(name_len <= 128);
		glGetActiveUniformName(obj->pp->id, element_indices[i], sizeof(element_name), NULL, element_name);

		mgl_u64_t name_offset;
		for (name_offset = 0; name_offset < 128; ++name_offset)
			if (element_name[name_offset] == '.')
				break;
		name_offset += 1;
		mgl_str_copy(element_name + name_offset, cbs->elements[i].name, MRL_MAX_CONSTANT_BUFFER_ELEMENT_NAME_SIZE);

		// Get array stride, buffer offset and array size
		GLint stride, offset, size;
		glGetActiveUniformsiv(obj->pp->id, 1, &element_indices[i], GL_UNIFORM_ARRAY_STRIDE, &stride);
		glGetActiveUniformsiv(obj->pp->id, 1, &element_indices[i], GL_UNIFORM_OFFSET, &offset);
		glGetActiveUniformsiv(obj->pp->id, 1, &element_indices[i], GL_UNIFORM_SIZE, &size);

		cbs->elements[i].array_stride = stride;
		cbs->elements[i].offset = offset;
		cbs->elements[i].size = size;
	}

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
	}
}

// ---------- Index buffers ----------

static mrl_error_t create_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t** ib, const mrl_index_buffer_desc_t* desc)
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

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteBuffers(1, &id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

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

static void destroy_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t* ib)
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
	if (obj == NULL)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	else
	{
		rd->state.index_buffer_format = obj->format;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->id);
	}
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

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
	}
}

static mrl_error_t create_vertex_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t** vb, const mrl_vertex_buffer_desc_t* desc)
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

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteBuffers(1, &id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
	}

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

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
	}
}

static mrl_error_t create_vertex_array(mrl_render_device_t* brd, mrl_vertex_array_t** va, const mrl_vertex_array_desc_t* desc)
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

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteVertexArrays(1, &id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
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
	if (va == NULL)
		glBindVertexArray(0);
	else
		glBindVertexArray(obj->id);
}

// -------- Shaders ----------

static mrl_error_t create_shader_stage(mrl_render_device_t* brd, mrl_shader_stage_t** stage, const mrl_shader_stage_desc_t* desc)
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

	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteShader(id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
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

static mrl_error_t create_shader_pipeline(mrl_render_device_t* brd, mrl_shader_pipeline_t** pipeline, const mrl_shader_pipeline_desc_t* desc)
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

	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		glDeleteProgram(id);
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return MRL_ERROR_EXTERNAL;
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
	for (mgl_u64_t i = 0; i < MRL_OGL_330_SHADER_MAX_BINDING_POINT_COUNT; ++i)
	{
		obj->bps[i].pp = obj;
		obj->bps[i].loc = -1;
	}

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
	if (pipeline == NULL)
		glUseProgram(0);
	else
		glUseProgram(obj->id);
}

static mrl_shader_binding_point_t* get_shader_binding_point(mrl_render_device_t* brd, mrl_shader_pipeline_t* pipeline, const mgl_chr8_t* name)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	mrl_ogl_330_shader_pipeline_t* obj = (mrl_ogl_330_shader_pipeline_t*)pipeline;

	// Get binding point
	mrl_ogl_330_shader_binding_point_t* free_bp = NULL;
	for (mgl_u64_t i = 0; i < MRL_OGL_330_SHADER_MAX_BINDING_POINT_COUNT; ++i)
	{
		if (obj->bps[i].loc == -1)
		{
			free_bp = &obj->bps[i];
			continue;
		}

		if (mgl_str_equal(name, obj->bps[i].name))
			return (mrl_shader_binding_point_t*)&obj->bps[i];
	}

	// Add binding point
	GLint loc = glGetUniformLocation(obj->id, name);
	if (loc == -1)
	{
		loc = (GLint)glGetUniformBlockIndex(obj->id, name);
		glUniformBlockBinding(obj->id, (GLuint)loc, (GLuint)loc);
		if (loc == GL_INVALID_INDEX)
		{
			GLenum gl_err = glGetError();
			if (gl_err != 0)
			{
				if (rd->error_callback != NULL)
					rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
				return NULL;
			}
			return NULL;
		}
	}

	// Check if there are still free binding points
	if (free_bp == NULL)
	{
		MGL_DEBUG_ASSERT(MGL_FALSE); // Too many binding points
		return NULL;
	}

	// Check errors
	GLenum gl_err = glGetError();
	if (gl_err != 0)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, opengl_error_code_to_str(gl_err));
		return NULL;
	}

	mgl_str_copy(name, free_bp->name, MRL_OGL_330_SHADER_BINDING_POINT_MAX_NAME_SIZE);
	free_bp->loc = loc;

	return (mrl_shader_binding_point_t*)free_bp;
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

static void draw_triangles_instanced(mrl_render_device_t* brd, mgl_u64_t offset, mgl_u64_t count, mgl_u64_t instance_count)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	glDrawArraysInstanced(GL_TRIANGLES, (GLint)offset, (GLsizei)count, (GLsizei)instance_count);
}

static void draw_triangles_indexed_instanced(mrl_render_device_t* brd, mgl_u64_t offset, mgl_u64_t count, mgl_u64_t instance_count)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)count, rd->state.index_buffer_format, (const void*)offset, (GLsizei)instance_count);
}

static void set_viewport(mrl_render_device_t* brd, mgl_i32_t x, mgl_i32_t y, mgl_i32_t w, mgl_i32_t h)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	glViewport((GLint)x, (GLint)y, (GLint)w, (GLint)h);
}

static const mgl_chr8_t* get_type_name(mrl_render_device_t* brd)
{
	return u8"ogl_330";
}

static mgl_i64_t get_property_i(mrl_render_device_t* brd, mgl_enum_t name)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	if (name == MRL_PROPERTY_MAX_ANISTROPY)
	{
		if (GL_ARB_texture_filter_anisotropic)
		{
			GLfloat v = 1.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &v);
			return (mgl_i64_t)v;
		}
		else return 1;
	}

	return -1;
}

static mgl_f64_t get_property_f(mrl_render_device_t* brd, mgl_enum_t name)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	if (name == MRL_PROPERTY_MAX_ANISTROPY)
	{
		if (GL_ARB_texture_filter_anisotropic)
		{
			GLfloat v = 1.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &v);
			return (mgl_f64_t)v;
		}
		else return 1.0;
	}

	return MGL_F64_NAN;
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

	// Create constant buffer pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_constant_buffer_count, sizeof(mrl_ogl_330_constant_buffer_t)),
		(void**)&rd->memory.constant_buffer.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_6;
	mgl_init_pool_allocator(
		&rd->memory.constant_buffer.pool,
		desc->max_constant_buffer_count,
		sizeof(mrl_ogl_330_constant_buffer_t),
		rd->memory.constant_buffer.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_constant_buffer_count, sizeof(mrl_ogl_330_constant_buffer_t)));

	// Create cube map pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_cube_map_count, sizeof(mrl_ogl_330_cube_map_t)),
		(void**)&rd->memory.cube_map.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_7;
	mgl_init_pool_allocator(
		&rd->memory.cube_map.pool,
		desc->max_cube_map_count,
		sizeof(mrl_ogl_330_cube_map_t),
		rd->memory.cube_map.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_cube_map_count, sizeof(mrl_ogl_330_cube_map_t)));

	// Create texture 3D pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_3d_count, sizeof(mrl_ogl_330_texture_3d_t)),
		(void**)&rd->memory.texture_3d.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_8;
	mgl_init_pool_allocator(
		&rd->memory.texture_3d.pool,
		desc->max_texture_3d_count,
		sizeof(mrl_ogl_330_texture_3d_t),
		rd->memory.texture_3d.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_3d_count, sizeof(mrl_ogl_330_texture_3d_t)));

	// Create texture 2D pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_2d_count, sizeof(mrl_ogl_330_texture_2d_t)),
		(void**)&rd->memory.texture_2d.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_9;
	mgl_init_pool_allocator(
		&rd->memory.texture_2d.pool,
		desc->max_texture_2d_count,
		sizeof(mrl_ogl_330_texture_2d_t),
		rd->memory.texture_2d.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_2d_count, sizeof(mrl_ogl_330_texture_2d_t)));

	// Create texture 1D pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_1d_count, sizeof(mrl_ogl_330_texture_1d_t)),
		(void**)&rd->memory.texture_1d.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_10;
	mgl_init_pool_allocator(
		&rd->memory.texture_1d.pool,
		desc->max_texture_1d_count,
		sizeof(mrl_ogl_330_texture_1d_t),
		rd->memory.texture_1d.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_1d_count, sizeof(mrl_ogl_330_texture_1d_t)));

	// Create sampler pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_sampler_count, sizeof(mrl_ogl_330_sampler_t)),
		(void**)&rd->memory.sampler.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_11;
	mgl_init_pool_allocator(
		&rd->memory.sampler.pool,
		desc->max_sampler_count,
		sizeof(mrl_ogl_330_sampler_t),
		rd->memory.sampler.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_sampler_count, sizeof(mrl_ogl_330_sampler_t)));

	// Create blend state pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_blend_state_count, sizeof(mrl_ogl_330_blend_state_t)),
		(void**)&rd->memory.blend_state.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_12;
	mgl_init_pool_allocator(
		&rd->memory.blend_state.pool,
		desc->max_blend_state_count,
		sizeof(mrl_ogl_330_blend_state_t),
		rd->memory.blend_state.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_blend_state_count, sizeof(mrl_ogl_330_blend_state_t)));

	// Create depth stencil state pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_depth_stencil_state_count, sizeof(mrl_ogl_330_depth_stencil_state_t)),
		(void**)&rd->memory.depth_stencil_state.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_13;
	mgl_init_pool_allocator(
		&rd->memory.depth_stencil_state.pool,
		desc->max_depth_stencil_state_count,
		sizeof(mrl_ogl_330_depth_stencil_state_t),
		rd->memory.depth_stencil_state.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_depth_stencil_state_count, sizeof(mrl_ogl_330_depth_stencil_state_t)));

	// Create raster state pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_raster_state_count, sizeof(mrl_ogl_330_raster_state_t)),
		(void**)&rd->memory.raster_state.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_14;
	mgl_init_pool_allocator(
		&rd->memory.raster_state.pool,
		desc->max_raster_state_count,
		sizeof(mrl_ogl_330_raster_state_t),
		rd->memory.raster_state.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_raster_state_count, sizeof(mrl_ogl_330_raster_state_t)));

	// Create framebuffer pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_framebuffer_count, sizeof(mrl_ogl_330_framebuffer_t)),
		(void**)&rd->memory.framebuffer.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_15;
	mgl_init_pool_allocator(
		&rd->memory.framebuffer.pool,
		desc->max_framebuffer_count,
		sizeof(mrl_ogl_330_framebuffer_t),
		rd->memory.framebuffer.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_framebuffer_count, sizeof(mrl_ogl_330_framebuffer_t)));

	return MRL_ERROR_NONE;

mgl_error_15:
	mgl_deallocate(rd->allocator, rd->memory.raster_state.data);
mgl_error_14:
	mgl_deallocate(rd->allocator, rd->memory.depth_stencil_state.data);
mgl_error_13:
	mgl_deallocate(rd->allocator, rd->memory.blend_state.data);
mgl_error_12:
	mgl_deallocate(rd->allocator, rd->memory.sampler.data);
mgl_error_11:
	mgl_deallocate(rd->allocator, rd->memory.texture_1d.data);
mgl_error_10:
	mgl_deallocate(rd->allocator, rd->memory.texture_2d.data);
mgl_error_9:
	mgl_deallocate(rd->allocator, rd->memory.texture_3d.data);
mgl_error_8:
	mgl_deallocate(rd->allocator, rd->memory.cube_map.data);
mgl_error_7:
	mgl_deallocate(rd->allocator, rd->memory.constant_buffer.data);
mgl_error_6:
	mgl_deallocate(rd->allocator, rd->memory.index_buffer.data);
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
	mgl_deallocate(rd->allocator, rd->memory.framebuffer.data);
	mgl_deallocate(rd->allocator, rd->memory.raster_state.data);
	mgl_deallocate(rd->allocator, rd->memory.depth_stencil_state.data);
	mgl_deallocate(rd->allocator, rd->memory.blend_state.data);
	mgl_deallocate(rd->allocator, rd->memory.sampler.data);
	mgl_deallocate(rd->allocator, rd->memory.texture_1d.data);
	mgl_deallocate(rd->allocator, rd->memory.texture_2d.data);
	mgl_deallocate(rd->allocator, rd->memory.texture_3d.data);
	mgl_deallocate(rd->allocator, rd->memory.cube_map.data);
	mgl_deallocate(rd->allocator, rd->memory.constant_buffer.data);
	mgl_deallocate(rd->allocator, rd->memory.index_buffer.data);
	mgl_deallocate(rd->allocator, rd->memory.vertex_buffer.data);
	mgl_deallocate(rd->allocator, rd->memory.vertex_array.data);
	mgl_deallocate(rd->allocator, rd->memory.shader_stage.data);
	mgl_deallocate(rd->allocator, rd->memory.shader_pipeline.data);
}

static void set_rd_functions(mrl_ogl_330_render_device_t* rd)
{
	// Framebuffer functions
	rd->base.create_framebuffer = &create_framebuffer;
	rd->base.destroy_framebuffer = &destroy_framebuffer;
	rd->base.set_framebuffer = &set_framebuffer;

	// Sampler functions
	rd->base.create_sampler = &create_sampler;
	rd->base.destroy_sampler = &destroy_sampler;
	rd->base.bind_sampler = &bind_sampler;

	// Texture 1D functions
	rd->base.create_texture_1d = &create_texture_1d;
	rd->base.destroy_texture_1d = &destroy_texture_1d;
	rd->base.generate_texture_1d_mipmaps = &generate_texture_1d_mipmaps;
	rd->base.bind_texture_1d = &bind_texture_1d;
	rd->base.update_texture_1d = &update_texture_1d;

	// Texture 2D functions
	rd->base.create_texture_2d = &create_texture_2d;
	rd->base.destroy_texture_2d = &destroy_texture_2d;
	rd->base.generate_texture_2d_mipmaps = &generate_texture_2d_mipmaps;
	rd->base.bind_texture_2d = &bind_texture_2d;
	rd->base.update_texture_2d = &update_texture_2d;

	// Texture 3D functions
	rd->base.create_texture_3d = &create_texture_3d;
	rd->base.destroy_texture_3d = &destroy_texture_3d;
	rd->base.generate_texture_3d_mipmaps = &generate_texture_3d_mipmaps;
	rd->base.bind_texture_3d = &bind_texture_3d;
	rd->base.update_texture_3d = &update_texture_3d;

	// Cube map functions
	rd->base.create_cube_map = &create_cube_map;
	rd->base.destroy_cube_map = &destroy_cube_map;
	rd->base.generate_cube_map_mipmaps = &generate_cube_map_mipmaps;
	rd->base.bind_cube_map = &bind_cube_map;
	rd->base.update_cube_map = &update_cube_map;

	// Constant buffer functions
	rd->base.create_constant_buffer = &create_constant_buffer;
	rd->base.destroy_constant_buffer = &destroy_constant_buffer;
	rd->base.bind_constant_buffer = &bind_constant_buffer;
	rd->base.map_constant_buffer = &map_constant_buffer;
	rd->base.unmap_constant_buffer = &unmap_constant_buffer;
	rd->base.update_constant_buffer = &update_constant_buffer;
	rd->base.query_constant_buffer_structure = &query_constant_buffer_structure;

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
	rd->base.get_shader_binding_point = &get_shader_binding_point;

	// Draw functions
	rd->base.clear_color = &clear_color;
	rd->base.clear_depth = &clear_depth;
	rd->base.clear_stencil = &clear_stencil;
	rd->base.draw_triangles = &draw_triangles;
	rd->base.draw_triangles_indexed = &draw_triangles_indexed;
	rd->base.draw_triangles_instanced = &draw_triangles_instanced;
	rd->base.draw_triangles_indexed_instanced = &draw_triangles_indexed_instanced;
	rd->base.set_viewport = &set_viewport;

	// Getter functions
	rd->base.get_type_name = &get_type_name;
	rd->base.get_property_i = &get_property_i;
	rd->base.get_property_f = &get_property_f;

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

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

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
