#include <mrl/d3d_11_render_device.h>

#include <mgl/memory/allocator.h>
#include <mgl/memory/manipulation.h>
#include <mgl/memory/pool_allocator.h>
#include <mgl/string/manipulation.h>
#include <mgl/input/window.h>
#include <mgl/math/scalar.h>

#ifdef MRL_BUILD_D3D_11
#	ifdef MGL_SYSTEM_WINDOWS
#		include <mgl/input/windows_window.h>
#		include <windows.h>
#		include <windowsx.h>
#		include <d3d11_1.h>
#		include <d3dcompiler.h>
#		pragma comment (lib, "d3d11.lib")
#		pragma comment (lib, "d3dcompiler.lib")
#		pragma comment (lib, "dxguid.lib")
#	else
#		error Direct3D 11 Render Device can only be built for Windows
#	endif

#define MRL_MAX_D3D_11_RENDER_TARGET_COUNT 8

typedef struct
{
	ID3D11RenderTargetView* render_targets[MRL_MAX_D3D_11_RENDER_TARGET_COUNT];
	UINT render_target_count;
	ID3D11DepthStencilView* depth_stencil_view;
	D3D11_VIEWPORT viewport;
} mrl_d3d_11_framebuffer_t;

typedef struct
{
	ID3D11RasterizerState* id;
} mrl_d3d_11_raster_state_t;

typedef struct
{
	ID3D11DepthStencilState* id;
} mrl_d3d_11_depth_stencil_state_t;

typedef struct
{
	ID3D11BlendState* id;
} mrl_d3d_11_blend_state_t;

typedef struct
{
	ID3D11SamplerState* id;
} mrl_d3d_11_sampler_t;

typedef struct
{
	ID3D11Texture1D* id;
} mrl_d3d_11_texture_1d_t;

typedef struct
{
	ID3D11Texture2D* id;
} mrl_d3d_11_texture_2d_t;

typedef struct
{
	ID3D11Texture3D* id;
} mrl_d3d_11_texture_3d_t;

typedef struct
{
	ID3D11Texture2D* id;
} mrl_d3d_11_cube_map_t;

typedef struct
{
	ID3D11Buffer* id;
} mrl_d3d_11_constant_buffer_t;

typedef struct
{
	ID3D11Buffer* id;
	DXGI_FORMAT format;
} mrl_d3d_11_index_buffer_t;

typedef struct
{
	ID3D11Buffer* id;
} mrl_d3d_11_vertex_buffer_t;

typedef struct
{
	ID3D11InputLayout* input_layout;

	UINT offsets[MRL_MAX_VERTEX_ARRAY_ELEMENT_COUNT];
	UINT strides[MRL_MAX_VERTEX_ARRAY_ELEMENT_COUNT];
	UINT element_count;

	ID3D11Buffer* buffers[MRL_MAX_VERTEX_ARRAY_BUFFER_COUNT];
	UINT buffer_count;
} mrl_d3d_11_vertex_array_t;

typedef struct
{
	union
	{
		ID3D11VertexShader* vertex;
		ID3D11PixelShader* pixel;
	};

	mgl_enum_t stage;
	ID3DBlob* blob;
	ID3D11ShaderReflection* reflection;
} mrl_d3d_11_shader_stage_t;

#define MRL_D3D_11_SHADER_BINDING_POINT_MAX_NAME_SIZE 32
#define MRL_D3D_11_SHADER_MAX_BINDING_POINT_COUNT 32

typedef struct mrl_d3d_11_shader_binding_point_t mrl_d3d_11_shader_binding_point_t;
typedef struct mrl_d3d_11_shader_pipeline_t mrl_d3d_11_shader_pipeline_t;

struct mrl_d3d_11_shader_binding_point_t
{
	mgl_chr8_t name[MRL_D3D_11_SHADER_BINDING_POINT_MAX_NAME_SIZE];
	mrl_d3d_11_shader_pipeline_t* pp;
	mgl_bool_t vertex;
	mgl_bool_t pixel;
};

struct mrl_d3d_11_shader_pipeline_t
{
	mrl_d3d_11_shader_binding_point_t bps[MRL_D3D_11_SHADER_MAX_BINDING_POINT_COUNT];
	mrl_d3d_11_shader_stage_t* vertex;
	mrl_d3d_11_shader_stage_t* pixel;
};

typedef struct
{
	mrl_render_device_t base;

	void* allocator;
	void* window;

	HWND hwnd;

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

	mrl_d3d_11_raster_state_t default_raster_state;
	mrl_d3d_11_depth_stencil_state_t default_depth_stencil_state;
	mrl_d3d_11_blend_state_t default_blend_state;

	IDXGISwapChain* swap_chain;
	ID3D11Device1* device;
	ID3D11DeviceContext* device_context;

	ID3D11Texture2D* back_buffer;
	ID3D11RenderTargetView* default_render_target_view;
	ID3D11RenderTargetView** render_target_views;
	mgl_u64_t render_target_count;

	ID3D11Texture2D* depth_stencil_buffer;
	ID3D11DepthStencilView* default_depth_stencil_view;
	ID3D11DepthStencilView* depth_stencil_view;

	mrl_d3d_11_shader_pipeline_t* current_pipeline;
	mrl_d3d_11_framebuffer_t* current_framebuffer;
	mrl_d3d_11_vertex_array_t* current_vertex_array;
	mrl_d3d_11_index_buffer_t* current_index_buffer;

	mgl_bool_t vsync_on;

	mrl_render_device_hint_error_callback_t error_callback;
	mrl_render_device_hint_error_callback_t warning_callback;
} mrl_d3d_11_render_device_t;

// ---------- Framebuffers ----------

static mrl_error_t create_framebuffer(mrl_render_device_t* brd, mrl_framebuffer_t** fb, const mrl_framebuffer_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

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

	// Allocate object
	mrl_d3d_11_framebuffer_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.framebuffer.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store framebuffer info
	*fb = (mrl_framebuffer_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_framebuffer(mrl_render_device_t* brd, mrl_framebuffer_t* fb)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_framebuffer_t* obj = (mrl_d3d_11_framebuffer_t*)fb;

	// Delete framebuffer
	
	// Deallocate object
	mgl_deallocate(
		&rd->memory.framebuffer.pool,
		obj);
}

static void set_framebuffer(mrl_render_device_t* brd, mrl_framebuffer_t* fb)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_framebuffer_t* obj = (mrl_d3d_11_framebuffer_t*)fb;

	// Set framebuffer

}

// ---------- Raster states ----------

static mrl_error_t create_raster_state(mrl_render_device_t* brd, mrl_raster_state_t** rs, const mrl_raster_state_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Allocate object
	mrl_d3d_11_raster_state_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.raster_state.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store raster state info

	*rs = (mrl_raster_state_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_raster_state(mrl_render_device_t* brd, mrl_raster_state_t* rs)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_raster_state_t* obj = (mrl_d3d_11_raster_state_t*)rs;

	// Deallocate object
	mgl_deallocate(
		&rd->memory.raster_state.pool,
		obj);
}

static void set_raster_state(mrl_render_device_t* brd, mrl_raster_state_t* rs)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_raster_state_t* obj = (mrl_d3d_11_raster_state_t*)rs;

	if (obj == NULL)
		obj = &rd->default_raster_state;

	// Set raster state
}

// ---------- Depth stencil states ----------

static mrl_error_t create_depth_stencil_state(mrl_render_device_t* brd, mrl_depth_stencil_state_t** dss, const mrl_depth_stencil_state_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Allocate object
	mrl_d3d_11_depth_stencil_state_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.depth_stencil_state.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store depth state info

	*dss = (mrl_depth_stencil_state_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_depth_stencil_state(mrl_render_device_t* brd, mrl_depth_stencil_state_t* dss)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_depth_stencil_state_t* obj = (mrl_d3d_11_depth_stencil_state_t*)dss;

	// Deallocate object
	mgl_deallocate(
		&rd->memory.depth_stencil_state.pool,
		obj);
}

static void set_depth_stencil_state(mrl_render_device_t* brd, mrl_depth_stencil_state_t* dss)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_depth_stencil_state_t* obj = (mrl_d3d_11_depth_stencil_state_t*)dss;

	if (obj == NULL)
		obj = &rd->default_depth_stencil_state;

	// Set depth stencil state

}

// ---------- Blend states ----------

static mrl_error_t create_blend_state(mrl_render_device_t* brd, mrl_blend_state_t** bs, const mrl_blend_state_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Allocate object
	mrl_d3d_11_blend_state_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.blend_state.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store blend state info

	*bs = (mrl_blend_state_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_blend_state(mrl_render_device_t* brd, mrl_blend_state_t* bs)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_blend_state_t* obj = (mrl_d3d_11_blend_state_t*)bs;

	// Deallocate object
	mgl_deallocate(
		&rd->memory.blend_state.pool,
		obj);
}

static void set_blend_state(mrl_render_device_t* brd, mrl_blend_state_t* bs)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_blend_state_t* obj = (mrl_d3d_11_blend_state_t*)bs;

	if (obj == NULL)
		obj = &rd->default_blend_state;

	// Set blend state

}

// ---------- Samplers ----------

static mrl_error_t create_sampler(mrl_render_device_t* brd, mrl_sampler_t** s, const mrl_sampler_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Initialize sampler

	// Allocate object
	mrl_d3d_11_sampler_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.sampler.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store sampler info
	*s = (mrl_sampler_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_sampler(mrl_render_device_t* brd, mrl_sampler_t* s)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_sampler_t* obj = (mrl_d3d_11_sampler_t*)s;

	// Delete sampler

	// Deallocate object
	mgl_deallocate(
		&rd->memory.sampler.pool,
		obj);
}

static void bind_sampler(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_sampler_t* s)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_sampler_t* obj = (mrl_d3d_11_sampler_t*)s;
	mrl_d3d_11_shader_binding_point_t* rbp = (mrl_d3d_11_shader_binding_point_t*)bp;

	// Bind sampler
}

// ---------- Texture 1D ----------

static mrl_error_t create_texture_1d(mrl_render_device_t* brd, mrl_texture_1d_t** tex, const mrl_texture_1d_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Initialize texture

	// Allocate object
	mrl_d3d_11_texture_1d_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.texture_1d.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store texture info
	*tex = (mrl_texture_1d_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_texture_1d(mrl_render_device_t* brd, mrl_texture_1d_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_1d_t* obj = (mrl_d3d_11_texture_1d_t*)tex;

	// Delete texture

	// Deallocate object
	mgl_deallocate(
		&rd->memory.texture_1d.pool,
		obj);
}

static void generate_texture_1d_mipmaps(mrl_render_device_t* brd, mrl_texture_2d_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_1d_t* obj = (mrl_d3d_11_texture_1d_t*)tex;
}

static void bind_texture_1d(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_texture_1d_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_1d_t* obj = (mrl_d3d_11_texture_1d_t*)tex;
	mrl_d3d_11_shader_binding_point_t* rbp = (mrl_d3d_11_shader_binding_point_t*)bp;

	// Bind texture
	
}

static mrl_error_t update_texture_1d(mrl_render_device_t* brd, mrl_texture_1d_t* tex, const mrl_texture_1d_update_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_1d_t* obj = (mrl_d3d_11_texture_1d_t*)tex;

	// Update texture

	return MRL_ERROR_NONE;
}

// ---------- Texture 2D ----------

static mrl_error_t create_texture_2d(mrl_render_device_t* brd, mrl_texture_2d_t** tex, const mrl_texture_2d_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Initialize texture

	// Allocate object
	mrl_d3d_11_texture_2d_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.texture_2d.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store texture info
	*tex = (mrl_texture_2d_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_texture_2d(mrl_render_device_t* brd, mrl_texture_2d_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_2d_t* obj = (mrl_d3d_11_texture_2d_t*)tex;

	// Delete texture

	// Deallocate object
	mgl_deallocate(
		&rd->memory.texture_2d.pool,
		obj);
}

static void generate_texture_2d_mipmaps(mrl_render_device_t* brd, mrl_texture_2d_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_2d_t* obj = (mrl_d3d_11_texture_2d_t*)tex;

}

static void bind_texture_2d(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_texture_2d_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_2d_t* obj = (mrl_d3d_11_texture_2d_t*)tex;
	mrl_d3d_11_shader_binding_point_t* rbp = (mrl_d3d_11_shader_binding_point_t*)bp;

	// Bind texture
}

static mrl_error_t update_texture_2d(mrl_render_device_t* brd, mrl_texture_2d_t* tex, const mrl_texture_2d_update_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_2d_t* obj = (mrl_d3d_11_texture_2d_t*)tex;

	// Update texture

	return MRL_ERROR_NONE;
}

// ---------- Texture 3D ----------

static mrl_error_t create_texture_3d(mrl_render_device_t* brd, mrl_texture_3d_t** tex, const mrl_texture_3d_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Initialize texture

	// Check errors

	// Allocate object
	mrl_d3d_11_texture_3d_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.texture_3d.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);
	
	// Store texture info
	*tex = (mrl_texture_3d_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_texture_3d(mrl_render_device_t* brd, mrl_texture_3d_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_3d_t* obj = (mrl_d3d_11_texture_3d_t*)tex;

	// Delete texture

	// Deallocate object
	mgl_deallocate(
		&rd->memory.texture_3d.pool,
		obj);
}

static void generate_texture_3d_mipmaps(mrl_render_device_t* brd, mrl_texture_2d_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_3d_t* obj = (mrl_d3d_11_texture_3d_t*)tex;

}

static void bind_texture_3d(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_texture_3d_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_3d_t* obj = (mrl_d3d_11_texture_3d_t*)tex;
	mrl_d3d_11_shader_binding_point_t* rbp = (mrl_d3d_11_shader_binding_point_t*)bp;

	// Bind texture

}

static mrl_error_t update_texture_3d(mrl_render_device_t* brd, mrl_texture_3d_t* tex, const mrl_texture_3d_update_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_texture_3d_t* obj = (mrl_d3d_11_texture_3d_t*)tex;

	// Update texture
	
	// Check errors

	return MRL_ERROR_NONE;
}

// ---------- Cube Map ----------

static mrl_error_t create_cube_map(mrl_render_device_t* brd, mrl_cube_map_t** tex, const mrl_cube_map_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Initialize texture

	// Allocate object
	mrl_d3d_11_cube_map_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.cube_map.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store texture info
	*tex = (mrl_cube_map_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_cube_map(mrl_render_device_t* brd, mrl_cube_map_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_cube_map_t* obj = (mrl_d3d_11_cube_map_t*)tex;

	// Delete texture

	// Deallocate object
	mgl_deallocate(
		&rd->memory.cube_map.pool,
		obj);
}

static void generate_cube_map_mipmaps(mrl_render_device_t* brd, mrl_cube_map_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_cube_map_t* obj = (mrl_d3d_11_cube_map_t*)tex;
}

static void bind_cube_map(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_cube_map_t* tex)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_cube_map_t* obj = (mrl_d3d_11_cube_map_t*)tex;
	mrl_d3d_11_shader_binding_point_t* rbp = (mrl_d3d_11_shader_binding_point_t*)bp;

	// Bind texture
}

static mrl_error_t update_cube_map(mrl_render_device_t* brd, mrl_cube_map_t* tex, const mrl_cube_map_update_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_cube_map_t* obj = (mrl_d3d_11_cube_map_t*)tex;

	// Update texture

	return MRL_ERROR_NONE;
}

// ---------- Constant buffers ----------

static mrl_error_t create_constant_buffer(mrl_render_device_t* brd, mrl_constant_buffer_t** cb, const mrl_constant_buffer_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Check for invalid input
	if (desc->usage == MRL_CONSTANT_BUFFER_USAGE_STATIC && desc->data == NULL)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create constant buffer: when the usage mode is set to static, the pointer to the initial data must not be NULL");
		return MRL_ERROR_INVALID_PARAMS;
	}

	// Initialize constant buffer

	// Allocate object
	mrl_d3d_11_constant_buffer_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.constant_buffer.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store constant buffer info
	*cb = (mrl_constant_buffer_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_constant_buffer(mrl_render_device_t* brd, mrl_constant_buffer_t* cb)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_constant_buffer_t* obj = (mrl_d3d_11_constant_buffer_t*)cb;

	// Delete constant buffer

	// Deallocate object
	mgl_deallocate(
		&rd->memory.constant_buffer.pool,
		obj);
}

static void bind_constant_buffer(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_constant_buffer_t* cb)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_constant_buffer_t* obj = (mrl_d3d_11_constant_buffer_t*)cb;
	mrl_d3d_11_shader_binding_point_t* rbp = (mrl_d3d_11_shader_binding_point_t*)bp;

	// Bind constant buffer

}

static void* map_constant_buffer(mrl_render_device_t* brd, mrl_constant_buffer_t* cb)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_constant_buffer_t* obj = (mrl_d3d_11_constant_buffer_t*)cb;

	// Map UBO
	return NULL;
}

static void unmap_constant_buffer(mrl_render_device_t* brd, mrl_constant_buffer_t* cb)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_constant_buffer_t* obj = (mrl_d3d_11_constant_buffer_t*)cb;

	// Unmap UBO

}

static void update_constant_buffer(mrl_render_device_t* brd, mrl_constant_buffer_t* cb, mgl_u64_t offset, mgl_u64_t size, const void* data)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_constant_buffer_t* obj = (mrl_d3d_11_constant_buffer_t*)cb;

	// Update UBO

}

static void query_constant_buffer_structure(mrl_render_device_t* brd, mrl_shader_binding_point_t* bp, mrl_constant_buffer_structure_t* cbs)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_shader_binding_point_t* obj = (mrl_d3d_11_shader_binding_point_t*)bp;
}

// ---------- Index buffers ----------

static mrl_error_t create_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t** ib, const mrl_index_buffer_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Check for invalid input
	if (desc->usage == MRL_INDEX_BUFFER_USAGE_STATIC && desc->data == NULL)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create index buffer: when the usage mode is set to static, the pointer to the initial data must not be NULL");
		return MRL_ERROR_INVALID_PARAMS;
	}

	// Allocate object
	mrl_d3d_11_index_buffer_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.index_buffer.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store index buffer info
	*ib = (mrl_index_buffer_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t* ib)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_index_buffer_t* obj = (mrl_d3d_11_index_buffer_t*)ib;

	// Delete index buffer

	// Deallocate object
	mgl_deallocate(
		&rd->memory.index_buffer.pool,
		obj);
}

static void set_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t* ib)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_index_buffer_t* obj = (mrl_d3d_11_index_buffer_t*)ib;

	// Set index buffer
}

static void* map_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t* ib)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_index_buffer_t* obj = (mrl_d3d_11_index_buffer_t*)ib;

	// Map IBO
	return NULL;
}

static void unmap_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t* ib)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_index_buffer_t* obj = (mrl_d3d_11_index_buffer_t*)ib;

	// Unmap IBO
}

static void update_index_buffer(mrl_render_device_t* brd, mrl_index_buffer_t* ib, mgl_u64_t offset, mgl_u64_t size, const void* data)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_index_buffer_t* obj = (mrl_d3d_11_index_buffer_t*)ib;

	// Update IBO
}

static mrl_error_t create_vertex_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t** vb, const mrl_vertex_buffer_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Check for invalid input
	if (desc->usage == MRL_VERTEX_BUFFER_USAGE_STATIC && desc->data == NULL)
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_INVALID_PARAMS, u8"Failed to create vertex buffer: when the usage mode is set to static, the pointer to the initial data must not be NULL");
		return MRL_ERROR_INVALID_PARAMS;
	}

	// Get usage
	//UINT usage;

	/*if (desc->usage == MRL_VERTEX_BUFFER_USAGE_DEFAULT)
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
	}*/

	// Initialize vertex buffer

	// Allocate object
	mrl_d3d_11_vertex_buffer_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.vertex_buffer.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store vertex buffer info
	*vb = (mrl_vertex_buffer_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_vertex_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t* vb)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_vertex_buffer_t* obj = (mrl_d3d_11_vertex_buffer_t*)vb;

	// Delete vertex buffer

	// Deallocate object
	mgl_deallocate(
		&rd->memory.vertex_buffer.pool,
		obj);
}

static void* map_vertex_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t* vb)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_vertex_buffer_t* obj = (mrl_d3d_11_vertex_buffer_t*)vb;

	// Map VBO
	return NULL;
}

static void unmap_vertex_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t* vb)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_vertex_buffer_t* obj = (mrl_d3d_11_vertex_buffer_t*)vb;

	// Unmap VBO

}

static void update_vertex_buffer(mrl_render_device_t* brd, mrl_vertex_buffer_t* vb, mgl_u64_t offset, mgl_u64_t size, const void* data)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_vertex_buffer_t* obj = (mrl_d3d_11_vertex_buffer_t*)vb;

	// Update VBO

}

static mrl_error_t create_vertex_array(mrl_render_device_t* brd, mrl_vertex_array_t** va, const mrl_vertex_array_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	MGL_DEBUG_ASSERT(desc->shader_pipeline != NULL);
	mrl_d3d_11_shader_pipeline_t* pp = (mrl_d3d_11_shader_pipeline_t*)desc->shader_pipeline;

	// Initialize vertex array

	// Allocate object
	mrl_d3d_11_vertex_array_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.vertex_array.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store vertex array info
	*va = (mrl_vertex_array_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_vertex_array(mrl_render_device_t* brd, mrl_shader_pipeline_t* va)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_vertex_array_t* obj = (mrl_d3d_11_vertex_array_t*)va;

	// Delete vertex array

	// Deallocate object
	mgl_deallocate(
		&rd->memory.vertex_array.pool,
		obj);
}

static void set_vertex_array(mrl_render_device_t* brd, mrl_shader_pipeline_t* va)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_vertex_array_t* obj = (mrl_d3d_11_vertex_array_t*)va;

	// Set vertex array
}

// -------- Shaders ----------

static mrl_error_t create_shader_stage(mrl_render_device_t* brd, mrl_shader_stage_t** stage, const mrl_shader_stage_desc_t* desc)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	
	// Check if the source is supported (only HLSL is supported for now)
	if (desc->src_type != MRL_SHADER_SOURCE_HLSL)
		return MRL_ERROR_UNSUPPORTED_SHADER_SOURCE;

	// Allocate object
	mrl_d3d_11_shader_stage_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.shader_stage.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Initialize shader
	obj->stage = desc->stage;
	if (desc->stage == MRL_SHADER_STAGE_VERTEX)
	{
		ID3DBlob* error_messages;
		HRESULT hr = D3DCompile(desc->src, mgl_str_size((const mgl_chr8_t*)desc->src) + 1, NULL, NULL, NULL, "vs", "vs_4_0", D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR, 0, &obj->blob, &error_messages);
		if (FAILED(hr))
		{
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_EXTERNAL, (const mgl_chr8_t*)error_messages->lpVtbl->GetBufferPointer(error_messages));
			mgl_deallocate(&rd->memory.shader_stage.pool, obj);
			return MRL_ERROR_EXTERNAL;
		}

		hr = rd->device->lpVtbl->CreateVertexShader(rd->device,
												    obj->blob->lpVtbl->GetBufferPointer(obj->blob),
												    obj->blob->lpVtbl->GetBufferSize(obj->blob),
												    NULL,
												    &obj->vertex);
		if (FAILED(hr))
		{
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_EXTERNAL, u8"Failed to create vertex shader stage: CreateVertexShader failed");
			obj->blob->lpVtbl->Release(obj->blob);
			mgl_deallocate(&rd->memory.shader_stage.pool, obj);
			return MRL_ERROR_EXTERNAL;
		}

		// Initialize reflector
		hr = D3DReflect(obj->blob->lpVtbl->GetBufferPointer(obj->blob),
						obj->blob->lpVtbl->GetBufferSize(obj->blob),
						&IID_ID3D11ShaderReflection,
						(void**)&obj->reflection);
		if (FAILED(hr))
		{
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_EXTERNAL, u8"Failed to create vertex shader stage reflector: D3DReflect failed");
			obj->vertex->lpVtbl->Release(obj->vertex);
			obj->blob->lpVtbl->Release(obj->blob);
			mgl_deallocate(&rd->memory.shader_stage.pool, obj);
			return MRL_ERROR_EXTERNAL;
		}
	}
	else if (desc->stage == MRL_SHADER_STAGE_PIXEL)
	{
		ID3DBlob* error_messages;
		HRESULT hr = D3DCompile(desc->src, mgl_str_size((const mgl_chr8_t*)desc->src) + 1, NULL, NULL, NULL, "ps", "ps_4_0", D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR, 0, &obj->blob, &error_messages);
		if (FAILED(hr))
		{
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_EXTERNAL, (const mgl_chr8_t*)error_messages->lpVtbl->GetBufferPointer(error_messages));
			mgl_deallocate(&rd->memory.shader_stage.pool, obj);
			return MRL_ERROR_EXTERNAL;
		}

		hr = rd->device->lpVtbl->CreatePixelShader(rd->device,
												   obj->blob->lpVtbl->GetBufferPointer(obj->blob),
												   obj->blob->lpVtbl->GetBufferSize(obj->blob),
												   NULL,
												   &obj->pixel);
		if (FAILED(hr))
		{
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_EXTERNAL, u8"Failed to create pixel shader stage: CreatePixelShader failed");
			obj->blob->lpVtbl->Release(obj->blob);
			mgl_deallocate(&rd->memory.shader_stage.pool, obj);
			return MRL_ERROR_EXTERNAL;
		}

		// Initialize reflector
		hr = D3DReflect(obj->blob->lpVtbl->GetBufferPointer(obj->blob),
						obj->blob->lpVtbl->GetBufferSize(obj->blob),
						&IID_ID3D11ShaderReflection,
						(void**)&obj->reflection);
		if (FAILED(hr))
		{
			if (rd->error_callback != NULL)
				rd->error_callback(MRL_ERROR_EXTERNAL, u8"Failed to create pixel shader stage reflector: D3DReflect failed");
			obj->pixel->lpVtbl->Release(obj->pixel);
			obj->blob->lpVtbl->Release(obj->blob);
			mgl_deallocate(&rd->memory.shader_stage.pool, obj);
			return MRL_ERROR_EXTERNAL;
		}
	}
	else
	{
		if (rd->error_callback != NULL)
			rd->error_callback(MRL_ERROR_EXTERNAL, u8"Failed to create shader stage: Invalid shader stage");
		mgl_deallocate(&rd->memory.shader_stage.pool, obj);
		return MRL_ERROR_UNSUPPORTED_SHADER_STAGE;
	}

	*stage = (mrl_shader_stage_t*)obj;

	return MRL_ERROR_NONE;
}

static void destroy_shader_stage(mrl_render_device_t* brd, mrl_shader_stage_t* stage)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_shader_stage_t* obj = (mrl_d3d_11_shader_stage_t*)stage;

	// Delete shader
	obj->reflection->lpVtbl->Release(obj->reflection);
	obj->blob->lpVtbl->Release(obj->blob);
	if (obj->stage == MRL_SHADER_STAGE_VERTEX)
		obj->vertex->lpVtbl->Release(obj->vertex);
	else if (obj->stage == MRL_SHADER_STAGE_PIXEL)
		obj->pixel->lpVtbl->Release(obj->pixel);

	// Deallocate object
	mgl_deallocate(
		&rd->memory.shader_stage.pool,
		obj);
}

static mrl_error_t create_shader_pipeline(mrl_render_device_t* brd, mrl_shader_pipeline_t** pipeline, const mrl_shader_pipeline_desc_t* desc)
{
	MGL_DEBUG_ASSERT(desc->vertex != NULL && desc->pixel != NULL);

	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Allocate object
	mrl_d3d_11_shader_pipeline_t* obj;
	mgl_error_t err = mgl_allocate(
		&rd->memory.shader_pipeline.pool,
		sizeof(*obj),
		(void**)&obj);
	if (err != MGL_ERROR_NONE)
		return mrl_make_mgl_error(err);

	// Store pipeline info
	obj->vertex = desc->vertex;
	obj->pixel = desc->pixel;

	*pipeline = (mrl_shader_pipeline_t*)obj;
	for (mgl_u64_t i = 0; i < MRL_D3D_11_SHADER_MAX_BINDING_POINT_COUNT; ++i)
	{
		obj->bps[i].pp = obj;
		obj->bps[i].name[0] = '\0';
	}

	return MRL_ERROR_NONE;
}

static void destroy_shader_pipeline(mrl_render_device_t* brd, mrl_shader_pipeline_t* pipeline)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_shader_pipeline_t* obj = (mrl_d3d_11_shader_pipeline_t*)pipeline;

	// Deallocate object
	mgl_deallocate(
		&rd->memory.shader_pipeline.pool,
		obj);
}

static void set_shader_pipeline(mrl_render_device_t* brd, mrl_shader_pipeline_t* pipeline)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_shader_pipeline_t* obj = (mrl_d3d_11_shader_pipeline_t*)pipeline;

	// Set shaders
	rd->device_context->lpVtbl->VSSetShader(rd->device_context, obj->vertex->vertex, NULL, 0);
	rd->device_context->lpVtbl->PSSetShader(rd->device_context, obj->pixel->pixel, NULL, 0);
}

static mgl_bool_t get_shader_stage_binding_point(mrl_d3d_11_render_device_t* rd, mrl_d3d_11_shader_stage_t* stage, const mgl_chr8_t* name, mrl_d3d_11_shader_binding_point_t* out_bp)
{
	ID3D11ShaderReflectionConstantBuffer* cb = stage->reflection->lpVtbl->GetConstantBufferByName(stage->reflection, (LPCSTR)name);
	if (cb != NULL)
	{
		
		return MGL_TRUE;
	}

	//ID3D11ShaderReflectionVariable* tex = stage->reflection->lpVtbl->get

	return MGL_FALSE;
}

static mrl_shader_binding_point_t* get_shader_binding_point(mrl_render_device_t* brd, mrl_shader_pipeline_t* pipeline, const mgl_chr8_t* name)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	mrl_d3d_11_shader_pipeline_t* obj = (mrl_d3d_11_shader_pipeline_t*)pipeline;

	// Get binding point
	mrl_d3d_11_shader_binding_point_t* free_bp = NULL;
	for (mgl_u64_t i = 0; i < MRL_D3D_11_SHADER_MAX_BINDING_POINT_COUNT; ++i)
	{
		if (obj->bps[i].name[0] == '\0')
		{
			free_bp = &obj->bps[i];
			continue;
		}

		if (mgl_str_equal(name, obj->bps[i].name))
			return (mrl_shader_binding_point_t*)&obj->bps[i];
	}

	// Add binding point
	mgl_bool_t success = get_shader_stage_binding_point(rd, obj->vertex, name, free_bp);
	if (!success)
		success = get_shader_stage_binding_point(rd, obj->pixel, name, free_bp);
	if (!success)
	{
		if (rd->warning_callback != NULL)
			rd->warning_callback(MRL_ERROR_NONE, u8"Failed to get shader binding point: couldn't find any binding point with this name");
		return NULL;
	}

	// Check if there are still free binding points
	if (free_bp == NULL)
	{
		MGL_DEBUG_ASSERT(MGL_FALSE); // Too many binding points
		return NULL;
	}

	mgl_str_copy(name, free_bp->name, MRL_D3D_11_SHADER_BINDING_POINT_MAX_NAME_SIZE);

	return (mrl_shader_binding_point_t*)free_bp;
}

// --------- Draw functions ----------

static void clear_color(mrl_render_device_t* brd, mgl_f32_t r, mgl_f32_t g, mgl_f32_t b, mgl_f32_t a)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	FLOAT color[4] = { r, g, b, a };

	for (mgl_u64_t i = 0; i < rd->render_target_count; ++i)
		rd->device_context->lpVtbl->ClearRenderTargetView(rd->device_context, rd->render_target_views[i], color);
}

static void clear_depth(mrl_render_device_t* brd, mgl_f32_t depth)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	rd->device_context->lpVtbl->ClearDepthStencilView(rd->device_context, rd->depth_stencil_view, D3D11_CLEAR_DEPTH, depth, 0);
}

static void clear_stencil(mrl_render_device_t* brd, mgl_i32_t stencil)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	rd->device_context->lpVtbl->ClearDepthStencilView(rd->device_context, rd->depth_stencil_view, D3D11_CLEAR_STENCIL, 0.0f, stencil);
}

static void swap_buffers(mrl_render_device_t* brd)
{
#	ifdef MGL_SYSTEM_WINDOWS
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	if (rd->vsync_on)
		rd->swap_chain->lpVtbl->Present(rd->swap_chain, 1, 0);
	else
		rd->swap_chain->lpVtbl->Present(rd->swap_chain, 0, 0);
#	endif
}

static void draw_triangles(mrl_render_device_t* brd, mgl_u64_t offset, mgl_u64_t count)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	rd->device_context->lpVtbl->Draw(rd->device_context, (UINT)count, (UINT)offset);
}

static void draw_triangles_indexed(mrl_render_device_t* brd, mgl_u64_t offset, mgl_u64_t count)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	rd->device_context->lpVtbl->DrawIndexed(rd->device_context, (UINT)count, (UINT)offset, 0);
}

static void draw_triangles_instanced(mrl_render_device_t* brd, mgl_u64_t offset, mgl_u64_t count, mgl_u64_t instance_count)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	rd->device_context->lpVtbl->DrawInstanced(rd->device_context, (UINT)count, (UINT)instance_count, (UINT)offset, 0);
}

static void draw_triangles_indexed_instanced(mrl_render_device_t* brd, mgl_u64_t offset, mgl_u64_t count, mgl_u64_t instance_count)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;
	rd->device_context->lpVtbl->DrawIndexedInstanced(rd->device_context, (UINT)count, (UINT)instance_count, (UINT)offset, 0, 0);
}

static void set_viewport(mrl_render_device_t* brd, mgl_i32_t x, mgl_i32_t y, mgl_i32_t w, mgl_i32_t h)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(viewport));

	viewport.TopLeftX = (FLOAT)x;
	viewport.TopLeftY = (FLOAT)(mgl_get_window_height(rd->window) - y);
	viewport.Width = (FLOAT)w;
	viewport.Height = (FLOAT)h;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	rd->device_context->lpVtbl->RSSetViewports(rd->device_context, 1, &viewport);
}

static const mgl_chr8_t* get_type_name(mrl_render_device_t* brd)
{
	return u8"d3d_11";
}

static mgl_i64_t get_property_i(mrl_render_device_t* brd, mgl_enum_t name)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	if (name == MRL_PROPERTY_MAX_ANISTROPY)
		return 16;

	return -1;
}

static mgl_f64_t get_property_f(mrl_render_device_t* brd, mgl_enum_t name)
{
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	if (name == MRL_PROPERTY_MAX_ANISTROPY)
		return 16.0;

	return MGL_F64_NAN;
}

static mrl_error_t create_rd_allocators(mrl_d3d_11_render_device_t* rd, const mrl_render_device_desc_t* desc)
{
	mgl_error_t err;

	// Create shader stage pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_shader_stage_count, sizeof(mrl_d3d_11_shader_stage_t)),
		(void**)&rd->memory.shader_stage.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_1;
	mgl_init_pool_allocator(
		&rd->memory.shader_stage.pool,
		desc->max_shader_stage_count,
		sizeof(mrl_d3d_11_shader_stage_t),
		rd->memory.shader_stage.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_shader_stage_count, sizeof(mrl_d3d_11_shader_stage_t)));

	// Create shader stage pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_shader_pipeline_count, sizeof(mrl_d3d_11_shader_pipeline_t)),
		(void**)&rd->memory.shader_pipeline.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_2;
	mgl_init_pool_allocator(
		&rd->memory.shader_pipeline.pool,
		desc->max_shader_pipeline_count,
		sizeof(mrl_d3d_11_shader_pipeline_t),
		rd->memory.shader_pipeline.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_shader_pipeline_count, sizeof(mrl_d3d_11_shader_pipeline_t)));

	// Create vertex buffer pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_vertex_buffer_count, sizeof(mrl_d3d_11_vertex_buffer_t)),
		(void**)&rd->memory.vertex_buffer.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_3;
	mgl_init_pool_allocator(
		&rd->memory.vertex_buffer.pool,
		desc->max_vertex_buffer_count,
		sizeof(mrl_d3d_11_vertex_buffer_t),
		rd->memory.vertex_buffer.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_vertex_buffer_count, sizeof(mrl_d3d_11_vertex_buffer_t)));

	// Create vertex array pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_vertex_array_count, sizeof(mrl_d3d_11_vertex_array_t)),
		(void**)&rd->memory.vertex_array.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_4;
	mgl_init_pool_allocator(
		&rd->memory.vertex_array.pool,
		desc->max_vertex_array_count,
		sizeof(mrl_d3d_11_vertex_array_t),
		rd->memory.vertex_array.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_vertex_array_count, sizeof(mrl_d3d_11_vertex_array_t)));

	// Create index buffer pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_index_buffer_count, sizeof(mrl_d3d_11_index_buffer_t)),
		(void**)&rd->memory.index_buffer.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_5;
	mgl_init_pool_allocator(
		&rd->memory.index_buffer.pool,
		desc->max_index_buffer_count,
		sizeof(mrl_d3d_11_index_buffer_t),
		rd->memory.index_buffer.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_index_buffer_count, sizeof(mrl_d3d_11_index_buffer_t)));

	// Create constant buffer pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_constant_buffer_count, sizeof(mrl_d3d_11_constant_buffer_t)),
		(void**)&rd->memory.constant_buffer.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_6;
	mgl_init_pool_allocator(
		&rd->memory.constant_buffer.pool,
		desc->max_constant_buffer_count,
		sizeof(mrl_d3d_11_constant_buffer_t),
		rd->memory.constant_buffer.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_constant_buffer_count, sizeof(mrl_d3d_11_constant_buffer_t)));

	// Create cube map pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_cube_map_count, sizeof(mrl_d3d_11_cube_map_t)),
		(void**)&rd->memory.cube_map.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_7;
	mgl_init_pool_allocator(
		&rd->memory.cube_map.pool,
		desc->max_cube_map_count,
		sizeof(mrl_d3d_11_cube_map_t),
		rd->memory.cube_map.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_cube_map_count, sizeof(mrl_d3d_11_cube_map_t)));

	// Create texture 3D pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_3d_count, sizeof(mrl_d3d_11_texture_3d_t)),
		(void**)&rd->memory.texture_3d.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_8;
	mgl_init_pool_allocator(
		&rd->memory.texture_3d.pool,
		desc->max_texture_3d_count,
		sizeof(mrl_d3d_11_texture_3d_t),
		rd->memory.texture_3d.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_3d_count, sizeof(mrl_d3d_11_texture_3d_t)));

	// Create texture 2D pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_2d_count, sizeof(mrl_d3d_11_texture_2d_t)),
		(void**)&rd->memory.texture_2d.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_9;
	mgl_init_pool_allocator(
		&rd->memory.texture_2d.pool,
		desc->max_texture_2d_count,
		sizeof(mrl_d3d_11_texture_2d_t),
		rd->memory.texture_2d.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_2d_count, sizeof(mrl_d3d_11_texture_2d_t)));

	// Create texture 1D pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_1d_count, sizeof(mrl_d3d_11_texture_1d_t)),
		(void**)&rd->memory.texture_1d.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_10;
	mgl_init_pool_allocator(
		&rd->memory.texture_1d.pool,
		desc->max_texture_1d_count,
		sizeof(mrl_d3d_11_texture_1d_t),
		rd->memory.texture_1d.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_texture_1d_count, sizeof(mrl_d3d_11_texture_1d_t)));

	// Create sampler pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_sampler_count, sizeof(mrl_d3d_11_sampler_t)),
		(void**)&rd->memory.sampler.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_11;
	mgl_init_pool_allocator(
		&rd->memory.sampler.pool,
		desc->max_sampler_count,
		sizeof(mrl_d3d_11_sampler_t),
		rd->memory.sampler.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_sampler_count, sizeof(mrl_d3d_11_sampler_t)));

	// Create blend state pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_blend_state_count, sizeof(mrl_d3d_11_blend_state_t)),
		(void**)&rd->memory.blend_state.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_12;
	mgl_init_pool_allocator(
		&rd->memory.blend_state.pool,
		desc->max_blend_state_count,
		sizeof(mrl_d3d_11_blend_state_t),
		rd->memory.blend_state.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_blend_state_count, sizeof(mrl_d3d_11_blend_state_t)));

	// Create depth stencil state pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_depth_stencil_state_count, sizeof(mrl_d3d_11_depth_stencil_state_t)),
		(void**)&rd->memory.depth_stencil_state.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_13;
	mgl_init_pool_allocator(
		&rd->memory.depth_stencil_state.pool,
		desc->max_depth_stencil_state_count,
		sizeof(mrl_d3d_11_depth_stencil_state_t),
		rd->memory.depth_stencil_state.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_depth_stencil_state_count, sizeof(mrl_d3d_11_depth_stencil_state_t)));

	// Create raster state pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_raster_state_count, sizeof(mrl_d3d_11_raster_state_t)),
		(void**)&rd->memory.raster_state.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_14;
	mgl_init_pool_allocator(
		&rd->memory.raster_state.pool,
		desc->max_raster_state_count,
		sizeof(mrl_d3d_11_raster_state_t),
		rd->memory.raster_state.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_raster_state_count, sizeof(mrl_d3d_11_raster_state_t)));

	// Create framebuffer pool
	err = mgl_allocate(
		rd->allocator,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_framebuffer_count, sizeof(mrl_d3d_11_framebuffer_t)),
		(void**)&rd->memory.framebuffer.data);
	if (err != MGL_ERROR_NONE)
		goto mgl_error_15;
	mgl_init_pool_allocator(
		&rd->memory.framebuffer.pool,
		desc->max_framebuffer_count,
		sizeof(mrl_d3d_11_framebuffer_t),
		rd->memory.framebuffer.data,
		MGL_POOL_ALLOCATOR_SIZE(desc->max_framebuffer_count, sizeof(mrl_d3d_11_framebuffer_t)));

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

static void destroy_rd_allocators(mrl_d3d_11_render_device_t* rd)
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

static void set_rd_functions(mrl_d3d_11_render_device_t* rd)
{
	// Framebuffer functions
	rd->base.create_framebuffer = &create_framebuffer;
	rd->base.destroy_framebuffer = &destroy_framebuffer;
	rd->base.set_framebuffer = &set_framebuffer;

	// Raster state functions
	rd->base.create_raster_state = &create_raster_state;
	rd->base.destroy_raster_state = &destroy_raster_state;
	rd->base.set_raster_state = &set_raster_state;

	// Depth stencil state functions
	rd->base.create_depth_stencil_state = &create_depth_stencil_state;
	rd->base.destroy_depth_stencil_state = &destroy_depth_stencil_state;
	rd->base.set_depth_stencil_state = &set_depth_stencil_state;

	// Blend state functions
	rd->base.create_blend_state = &create_blend_state;
	rd->base.destroy_blend_state = &destroy_blend_state;
	rd->base.set_blend_state = &set_blend_state;

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

static void extract_hints(mrl_d3d_11_render_device_t* rd, const mrl_render_device_desc_t* desc)
{
	for (const mrl_hint_t* hint = desc->hints; hint != NULL; hint = hint->next)
	{
		// Check if the hint should be skipped
		if (hint->device_type != NULL && !mgl_str_equal(hint->device_type, u8"d3d_11"))
			continue;

		// Extract hint info
		switch (hint->type)
		{
			case MRL_HINT_RENDER_DEVICE_WARNING_CALLBACK:
				MGL_DEBUG_ASSERT(hint->data != NULL);
				rd->warning_callback = *(const mrl_render_device_hint_warning_callback_t*)hint->data;
				break;

			case MRL_HINT_RENDER_DEVICE_ERROR_CALLBACK:
				MGL_DEBUG_ASSERT(hint->data != NULL);
				rd->error_callback = *(const mrl_render_device_hint_error_callback_t*)hint->data;
				break;

			default:
				// Unsupported hint type, ignore it
				continue;
		}
	}
}

static mrl_error_t create_d3d_context(mrl_d3d_11_render_device_t* rd, const mrl_render_device_desc_t* rd_desc)
{
	if (mgl_str_equal(u8"win32", mgl_get_window_type(rd->window)))
	{
#	ifndef MGL_SYSTEM_WINDOWS
		return MRL_ERROR_UNSUPPORTED_WINDOW;
#	else
		rd->hwnd = (HWND)((mgl_windows_window_t*)rd->window)->hwnd;
		
		HRESULT hr;

		// Create device and swap chain
		{
			DXGI_SWAP_CHAIN_DESC desc;
			ZeroMemory(&desc, sizeof(desc));

			desc.BufferCount = 1;
			desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.OutputWindow = (HWND)((mgl_windows_window_t*)rd->window)->hwnd;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Windowed = (mgl_get_window_mode(rd->window) == MGL_WINDOW_MODE_FULLSCREEN) ? FALSE : TRUE;
			
			UINT flags = 0;
			if (rd_desc->debug_mode)
				flags |= D3D11_CREATE_DEVICE_DEBUG;

			D3D_FEATURE_LEVEL levels[] =
			{
				D3D_FEATURE_LEVEL_11_1,
			};

			hr = D3D11CreateDeviceAndSwapChain(
				NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				flags,
				levels,
				1,
				D3D11_SDK_VERSION,
				&desc,
				&rd->swap_chain,
				(ID3D11Device**)&rd->device,
				NULL,
				(ID3D11DeviceContext**)&rd->device_context);

			if (FAILED(hr))
				return MRL_ERROR_EXTERNAL;
		}

		// Get backbuffer
		hr = rd->swap_chain->lpVtbl->GetBuffer(rd->swap_chain, 0, &IID_ID3D11Texture2D, (LPVOID*)&rd->back_buffer);
		if (FAILED(hr))
		{
			rd->swap_chain->lpVtbl->Release(rd->swap_chain);
			rd->device->lpVtbl->Release(rd->device);
			rd->device_context->lpVtbl->Release(rd->device_context);
			return MRL_ERROR_EXTERNAL;
		}

		// Create render target view
		hr = rd->device->lpVtbl->CreateRenderTargetView(rd->device, (ID3D11Resource*)rd->back_buffer, NULL, &rd->default_render_target_view);
		if (FAILED(hr))
		{
			rd->back_buffer->lpVtbl->Release(rd->back_buffer);
			rd->swap_chain->lpVtbl->Release(rd->swap_chain);
			rd->device->lpVtbl->Release(rd->device);
			rd->device_context->lpVtbl->Release(rd->device_context);
			return MRL_ERROR_EXTERNAL;
		}

		// Create depth stencil buffer
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = mgl_get_window_width(rd->window);
			desc.Height = mgl_get_window_height(rd->window);
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;

			hr = rd->device->lpVtbl->CreateTexture2D(rd->device, &desc, NULL, &rd->depth_stencil_buffer);

			if (FAILED(hr))
			{
				rd->default_render_target_view->lpVtbl->Release(rd->default_render_target_view);
				rd->back_buffer->lpVtbl->Release(rd->back_buffer);
				rd->swap_chain->lpVtbl->Release(rd->swap_chain);
				rd->device->lpVtbl->Release(rd->device);
				rd->device_context->lpVtbl->Release(rd->device_context);
				return MRL_ERROR_EXTERNAL;
			}
		}

		// Create depth stencil view
		hr = rd->device->lpVtbl->CreateDepthStencilView(rd->device, (ID3D11Resource*)rd->depth_stencil_buffer, NULL, &rd->default_depth_stencil_view);
		if (FAILED(hr))
		{
			rd->depth_stencil_buffer->lpVtbl->Release(rd->depth_stencil_buffer);
			rd->default_render_target_view->lpVtbl->Release(rd->default_render_target_view);
			rd->back_buffer->lpVtbl->Release(rd->back_buffer);
			rd->swap_chain->lpVtbl->Release(rd->swap_chain);
			rd->device->lpVtbl->Release(rd->device);
			rd->device_context->lpVtbl->Release(rd->device_context);
			return MRL_ERROR_EXTERNAL;
		}

		// Set default render target
		rd->device_context->lpVtbl->OMSetRenderTargets(rd->device_context, 1, &rd->default_render_target_view, rd->default_depth_stencil_view);
		rd->depth_stencil_view = rd->default_depth_stencil_view;
		rd->render_target_views = &rd->default_render_target_view;
		rd->render_target_count = 1;

		// Set default viewport
		{
			D3D11_VIEWPORT viewport;
			ZeroMemory(&viewport, sizeof(viewport));

			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Width = (FLOAT)mgl_get_window_width(rd->window);
			viewport.Height = (FLOAT)mgl_get_window_height(rd->window);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;

			rd->device_context->lpVtbl->RSSetViewports(rd->device_context, 1, &viewport);
		}

		return MRL_ERROR_NONE;
#	endif
	}
	else
		return MRL_ERROR_UNSUPPORTED_WINDOW;
}

static void destroy_d3d_context(mrl_d3d_11_render_device_t* rd)
{
	if (mgl_str_equal(u8"win32", mgl_get_window_type(rd->window)))
	{
#	ifndef MGL_SYSTEM_WINDOWS
		mgl_abort();
#	else
		rd->default_depth_stencil_view->lpVtbl->Release(rd->default_depth_stencil_view);
		rd->depth_stencil_buffer->lpVtbl->Release(rd->depth_stencil_buffer);
		rd->default_render_target_view->lpVtbl->Release(rd->default_render_target_view);
		rd->back_buffer->lpVtbl->Release(rd->back_buffer);
		rd->swap_chain->lpVtbl->Release(rd->swap_chain);
		rd->device_context->lpVtbl->Release(rd->device_context);
		rd->device->lpVtbl->Release(rd->device);
#	endif
	}
	else
		mgl_abort();
}
#endif

MRL_API mrl_error_t mrl_init_d3d_11_render_device(const mrl_render_device_desc_t* desc, mrl_render_device_t ** out_rd)
{
	MGL_DEBUG_ASSERT(desc != NULL && out_rd != NULL);
	MGL_DEBUG_ASSERT(desc->allocator != NULL && desc->window != NULL);

#ifndef MRL_BUILD_D3D_11
	return MRL_ERROR_UNSUPPORTED_DEVICE;
#else

	// Allocate render device
	mrl_d3d_11_render_device_t* rd;
	mgl_error_t mglerr = mgl_allocate(desc->allocator, sizeof(mrl_d3d_11_render_device_t), (void**)&rd);
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
	err = create_d3d_context(rd, desc);
	if (err != MRL_ERROR_NONE)
	{
		destroy_rd_allocators(rd);
		mgl_deallocate(rd->allocator, rd);
		return err;
	}

	// Set render device funcs
	set_rd_functions(rd);

	// Set default states
	{
		//mrl_raster_state_desc_t desc = MRL_DEFAULT_RASTER_STATE_DESC;
		//mrl_create_raster_state()
	}

	if (desc->vsync_mode == MRL_VSYNC_OFF)
		rd->vsync_on = MGL_FALSE;
	else
		rd->vsync_on = MGL_TRUE;

	*out_rd = (mrl_render_device_t*)rd;

	return MRL_ERROR_NONE;
#endif
}

MRL_API void mrl_terminate_d3d_11_render_device(mrl_render_device_t * brd)
{
	MGL_DEBUG_ASSERT(brd != NULL);

#ifndef MRL_BUILD_D3D_11
	mgl_abort();
#else
	mrl_d3d_11_render_device_t* rd = (mrl_d3d_11_render_device_t*)brd;

	// Terminate context
	destroy_d3d_context(rd);

	// Destroy allocators
	destroy_rd_allocators(rd);

	// Deallocate
	MGL_DEBUG_ASSERT(mgl_deallocate(rd->allocator, rd) == MRL_ERROR_NONE);
#endif
}
