#ifndef MRL_RENDER_DEVICE_H
#define MRL_RENDER_DEVICE_H
#ifdef __cplusplus
extern "C" {
#endif 

#include <mrl/error.h>

	typedef void(*mrl_render_device_hint_warning_callback_t)(mrl_error_t error, const mgl_chr8_t* msg);
	typedef void(*mrl_render_device_hint_error_callback_t)(mrl_error_t error, const mgl_chr8_t* msg);

	typedef struct mrl_hint_t mrl_hint_t;
	typedef struct mrl_framebuffer_desc_t mrl_framebuffer_desc_t;
	typedef struct mrl_raster_state_desc_t mrl_raster_state_desc_t;
	typedef struct mrl_depth_stencil_state_desc_t mrl_depth_stencil_state_desc_t;
	typedef struct mrl_blend_state_desc_t mrl_blend_state_desc_t;
	typedef struct mrl_sampler_desc_t mrl_sampler_desc_t;
	typedef struct mrl_texture_1d_desc_t mrl_texture_1d_desc_t;
	typedef struct mrl_texture_1d_update_desc_t mrl_texture_1d_update_desc_t;
	typedef struct mrl_texture_2d_desc_t mrl_texture_2d_desc_t;
	typedef struct mrl_texture_2d_update_desc_t mrl_texture_2d_update_desc_t;
	typedef struct mrl_texture_3d_desc_t mrl_texture_3d_desc_t;
	typedef struct mrl_texture_3d_update_desc_t mrl_texture_3d_update_desc_t;
	typedef struct mrl_cube_map_desc_t mrl_cube_map_desc_t;
	typedef struct mrl_cube_map_update_desc_t mrl_cube_map_update_desc_t;
	typedef struct mrl_constant_buffer_structure_element_t mrl_constant_buffer_structure_element_t;
	typedef struct mrl_constant_buffer_structure_t mrl_constant_buffer_structure_t;
	typedef struct mrl_constant_buffer_desc_t mrl_constant_buffer_desc_t;
	typedef struct mrl_index_buffer_desc_t mrl_index_buffer_desc_t;
	typedef struct mrl_vertex_buffer_desc_t mrl_vertex_buffer_desc_t;
	typedef struct mrl_vertex_element_t mrl_vertex_element_t;
	typedef struct mrl_vertex_array_desc_t mrl_vertex_array_desc_t;
	typedef struct mrl_shader_stage_desc_t mrl_shader_stage_desc_t;
	typedef struct mrl_shader_pipeline_desc_t mrl_shader_pipeline_desc_t;
	typedef struct mrl_render_device_desc_t mrl_render_device_desc_t;

	typedef void mrl_framebuffer_t;
	typedef void mrl_raster_state_t;
	typedef void mrl_depth_stencil_state_t;
	typedef void mrl_blend_state_t;
	typedef void mrl_sampler_t;
	typedef void mrl_texture_1d_t;
	typedef void mrl_texture_2d_t;
	typedef void mrl_texture_3d_t;
	typedef void mrl_cube_map_t;
	typedef void mrl_constant_buffer_t;
	typedef void mrl_index_buffer_t;
	typedef void mrl_vertex_buffer_t;
	typedef void mrl_vertex_array_t;
	typedef void mrl_shader_stage_t;
	typedef void mrl_shader_pipeline_t;
	typedef void mrl_shader_binding_point_t;

	// ----- Property names -----
	
	enum
	{
		MRL_PROPERTY_MAX_ANISTROPY
	};

	// ----- Hints -----

	enum
	{
		/// <summary>
		///		Hints that the render device should call a function when a warning is issued.
		///		The pointer to the warning callback function pointer is stored on the 'data' member of the hint.
		///		The function pointer is of the type mrl_render_device_hint_warning_callback_t.
		/// </summary>
		MRL_HINT_RENDER_DEVICE_WARNING_CALLBACK,

		/// <summary>
		///		Hints that the render device should call a function when an error occurs.
		///		The pointer to the error callback function pointer is stored on the 'data' member of the hint.
		/// The function pointer is of the type mrl_render_device_hint_error_callback_t.
		/// </summary>
		MRL_HINT_RENDER_DEVICE_ERROR_CALLBACK,
	};

	struct mrl_hint_t
	{
		/// <summary>
		///		Hint type.
		/// </summary>
		mgl_enum_t type;
		
		/// <summary>
		///		Hint render device type.
		///		This pointer is optional, and can be set to NULL.
		///		If the pointer is not NULL, the hint is only read by the device type specified here.
		/// </summary>
		const mgl_chr8_t* device_type;
		
		/// <summary>
		///		Pointer to next hint.
		/// </summary>
		const mrl_hint_t* next;

		/// <summary>
		///		Pointer to hint data.
		/// </summary>
		const void* data;
	};

#define MRL_DEFAULT_HINT ((mrl_hint_t) {\
	0,\
	NULL,\
	NULL,\
	NULL,\
})

	// ---- Framebuffer ----

#define MRL_MAX_FRAMEBUFFER_RENDER_TARGET_COUNT 8

	enum
	{
		MRL_RENDER_TARGET_TYPE_TEXTURE_2D,
		MRL_RENDER_TARGET_TYPE_CUBE_MAP,
	};

	struct mrl_framebuffer_desc_t
	{
		/// <summary>
		///		Framebuffer render targets.
		///		These textures/cube maps must have the usage set to MRL_TEXTURE_USAGE_RENDER_TARGET.
		/// </summary>
		struct
		{
			/// <summary>
			///		The type of the render target.
			///		Valid values:
			///		- MRL_RENDER_TARGET_TYPE_TEXTURE_2D;
			///		- MRL_RENDER_TARGET_TYPE_CUBE_MAP;
			/// </summary>
			mgl_enum_t type;

			/// <summary>
			///		Mip level of the texture which will be set as a render target.
			/// </summary>
			mgl_u32_t mip_level;

			union
			{
				struct
				{
					/// <summary>
					///		Texture 2D handle.
					/// </summary>
					mrl_texture_2d_t* handle;
				} tex_2d;

				struct
				{
					/// <summary>
					///		Cube map handle.
					/// </summary>
					mrl_cube_map_t* handle;

					/// <summary>
					///		Cube map face.
					/// </summary>
					mgl_enum_t face;
				} cube_map;
			};

		} targets[MRL_MAX_FRAMEBUFFER_RENDER_TARGET_COUNT];

		/// <summary>
		///		Number of render targets.
		///		Valid values: 1 - MRL_MAX_FRAMEBUFFER_RENDER_TARGET_COUNT;		
		/// </summary>
		mgl_u32_t target_count;

		/// <summary>
		///		Depth stencil texture.
		///		Optional (can be NULL).
		/// </summary>
		mrl_texture_2d_t* depth_stencil;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_FRAMEBUFFER_DESC ((mrl_framebuffer_desc_t) {\
	{\
		-1,\
		1,\
	},\
	0,\
	NULL,\
	NULL,\
})

	// ---- Raster state ----

	enum
	{
		MRL_WINDING_CW,
		MRL_WINDING_CCW,
	};

	enum
	{
		MRL_FACE_FRONT,
		MRL_FACE_BACK,
		MRL_FACE_FRONT_AND_BACK,
	};

	enum
	{
		MRL_RASTER_MODE_FILL,
		MRL_RASTER_MODE_WIREFRAME,
	};

	struct mrl_raster_state_desc_t
	{
		/// <summary>
		///		Is culling enabled?
		///		Valid values:
		///			MGL_FALSE;
		///			MGL_TRUE;
		/// </summary>
		mgl_bool_t cull_enabled;

		/// <summary>
		///		Specifies the front face winding order.
		///		Valid values:
		///			MRL_WINDING_CW;
		///			MRK_WINDING_CCW;
		/// </summary>
		mgl_enum_t front_face;

		/// <summary>
		///		Specifies the face that should be culled.
		///		Valid values:
		///			MRL_FACE_FRONT;
		///			MRL_FACE_BACK;
		///			MRL_FACE_FRONT_AND_BACK;
		/// </summary>
		mgl_enum_t cull_face;

		/// <summary>
		///		Specifies the rasterizer mode.
		///		Valid values:
		///			MRL_RASTER_MODE_FILL;
		///			MRL_RASTER_MODE_WIREFRAME;
		/// </summary>
		mgl_enum_t raster_mode;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_RASTER_STATE_DESC ((mrl_raster_state_desc_t) {\
	MGL_FALSE,\
	MRL_WINDING_CCW,\
	MRL_FACE_BACK,\
	MRL_RASTER_MODE_FILL,\
	NULL,\
})

	// ---- Depth stencil state ----

	enum
	{
		MRL_COMPARE_NEVER,
		MRL_COMPARE_LESS,
		MRL_COMPARE_LEQUAL,
		MRL_COMPARE_GREATER,
		MRL_COMPARE_GEQUAL,
		MRL_COMPARE_EQUAL,
		MRL_COMPARE_NEQUAL,
		MRL_COMPARE_ALWAYS,
	};

	enum
	{
		MRL_ACTION_KEEP,
		MRL_ACTION_ZERO,
		MRL_ACTION_REPLACE,
		MRL_ACTION_INCREMENT,
		MRL_ACTION_DECREMENT,
		MRL_ACTION_INCREMENT_WRAP,
		MRL_ACTION_DECREMENT_WRAP,
		MRL_ACTION_INVERT,
	};

	struct mrl_depth_stencil_state_desc_t
	{
		struct
		{
			/// <summary>
			///		Perform depth checks?
			///		Valid values:
			///		- MGL_FALSE;
			///		- MGL_TRUE;
			/// </summary>
			mgl_bool_t enabled;

			/// <summary>
			///		Writes to depth?
			///		Valid values:
			///		- MGL_FALSE;
			///		- MGL_TRUE;
			/// </summary>
			mgl_bool_t write_enabled;

			/// <summary>
			///		Depth 'near' value.
			/// </summary>
			mgl_f32_t near;

			/// <summary>
			///		Depth 'far' value.
			/// </summary>
			mgl_f32_t far;

			/// <summary>
			///		Depth comparison function.
			///		Valid values:
			///		- MRL_COMPARE_NEVER;
			///		- MRL_COMPARE_LESS;
			///		- MRL_COMPARE_LEQUAL;
			///		- MRL_COMPARE_GREATER;
			///		- MRL_COMPARE_GEQUAL;
			///		- MRL_COMPARE_EQUAL;
			///		- MRL_COMPARE_NEQUAL;
			///		- MRL_COMPARE_ALWAYS;
			/// </summary>
			mgl_enum_t compare;
		} depth;

		struct
		{
			/// <summary>
			///		Stencil ref value used on stencil comparison functions.
			/// </summary>
			mgl_u32_t ref;

			/// <summary>
			///		Is stencil enabled?
			///		Valid values:
			///		- MGL_FALSE;
			///		- MGL_TRUE;
			/// </summary>
			mgl_bool_t enabled;

			/// <summary>
			///		Stencil read mask.
			/// </summary>
			mgl_u32_t read_mask;

			/// <summary>
			///		Stencil write mask.
			/// </summary>
			mgl_u32_t write_mask;

			struct
			{
				/// <summary>
				///		Stencil comparison function.
				///		Valid values:
				///		- MRL_COMPARE_NEVER;
				///		- MRL_COMPARE_LESS;
				///		- MRL_COMPARE_LEQUAL;
				///		- MRL_COMPARE_GREATER;
				///		- MRL_COMPARE_GEQUAL;
				///		- MRL_COMPARE_EQUAL;
				///		- MRL_COMPARE_NEQUAL;
				///		- MRL_COMPARE_ALWAYS;
				/// </summary>
				mgl_enum_t compare;

				/// <summary>
				///		Stencil comparison fail action.
				///		Valid values:
				///		- MRL_ACTION_KEEP;
				///		- MRL_ACTION_ZERO;
				///		- MRL_ACTION_REPLACE;
				///		- MRL_ACTION_INCREMENT;
				///		- MRL_ACTION_DECREMENT;
				///		- MRL_ACTION_INCREMENT_WRAP;
				///		- MRL_ACTION_DECREMENT_WRAP;
				///		- MRL_ACTION_INVERT;
				/// </summary>
				mgl_enum_t fail;

				/// <summary>
				///		Stencil comparison pass action.
				///		Valid values:
				///		- MRL_ACTION_KEEP;
				///		- MRL_ACTION_ZERO;
				///		- MRL_ACTION_REPLACE;
				///		- MRL_ACTION_INCREMENT;
				///		- MRL_ACTION_DECREMENT;
				///		- MRL_ACTION_INCREMENT_WRAP;
				///		- MRL_ACTION_DECREMENT_WRAP;
				///		- MRL_ACTION_INVERT;
				/// </summary>
				mgl_enum_t pass;

				/// <summary>
				///		Stencil depth comparison fail action.
				///		Valid values:
				///		- MRL_ACTION_KEEP;
				///		- MRL_ACTION_ZERO;
				///		- MRL_ACTION_REPLACE;
				///		- MRL_ACTION_INCREMENT;
				///		- MRL_ACTION_DECREMENT;
				///		- MRL_ACTION_INCREMENT_WRAP;
				///		- MRL_ACTION_DECREMENT_WRAP;
				///		- MRL_ACTION_INVERT;
				/// </summary>
				mgl_enum_t depth_fail;
			} front_face;

			struct
			{
				/// <summary>
				///		Stencil comparison function.
				///		Valid values:
				///		- MRL_COMPARE_NEVER;
				///		- MRL_COMPARE_LESS;
				///		- MRL_COMPARE_LEQUAL;
				///		- MRL_COMPARE_GREATER;
				///		- MRL_COMPARE_GEQUAL;
				///		- MRL_COMPARE_EQUAL;
				///		- MRL_COMPARE_NEQUAL;
				///		- MRL_COMPARE_ALWAYS;
				/// </summary>
				mgl_enum_t compare;

				/// <summary>
				///		Stencil comparison fail action.
				///		Valid values:
				///		- MRL_ACTION_KEEP;
				///		- MRL_ACTION_ZERO;
				///		- MRL_ACTION_REPLACE;
				///		- MRL_ACTION_INCREMENT;
				///		- MRL_ACTION_DECREMENT;
				///		- MRL_ACTION_INCREMENT_WRAP;
				///		- MRL_ACTION_DECREMENT_WRAP;
				///		- MRL_ACTION_INVERT;
				/// </summary>
				mgl_enum_t fail;

				/// <summary>
				///		Stencil comparison pass action.
				///		Valid values:
				///		- MRL_ACTION_KEEP;
				///		- MRL_ACTION_ZERO;
				///		- MRL_ACTION_REPLACE;
				///		- MRL_ACTION_INCREMENT;
				///		- MRL_ACTION_DECREMENT;
				///		- MRL_ACTION_INCREMENT_WRAP;
				///		- MRL_ACTION_DECREMENT_WRAP;
				///		- MRL_ACTION_INVERT;
				/// </summary>
				mgl_enum_t pass;

				/// <summary>
				///		Stencil depth comparison fail action.
				///		Valid values:
				///		- MRL_ACTION_KEEP;
				///		- MRL_ACTION_ZERO;
				///		- MRL_ACTION_REPLACE;
				///		- MRL_ACTION_INCREMENT;
				///		- MRL_ACTION_DECREMENT;
				///		- MRL_ACTION_INCREMENT_WRAP;
				///		- MRL_ACTION_DECREMENT_WRAP;
				///		- MRL_ACTION_INVERT;
				/// </summary>
				mgl_enum_t depth_fail;
			} back_face;
		} stencil;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_DEPTH_STENCIL_STATE_DESC ((mrl_depth_stencil_state_desc_t) {\
	{\
		MGL_FALSE,\
		MGL_FALSE,\
		0.1f,\
		1000.0f,\
		MRL_COMPARE_LESS,\
	},\
	{\
		0,\
		MGL_FALSE,\
		0xFFFFFFFFFFFFFFFF,\
		0xFFFFFFFFFFFFFFFF,\
		{\
			MRL_COMPARE_ALWAYS,\
			MRL_ACTION_KEEP,\
			MRL_ACTION_KEEP,\
			MRL_ACTION_KEEP,\
		},\
		{\
			MRL_COMPARE_ALWAYS,\
			MRL_ACTION_KEEP,\
			MRL_ACTION_KEEP,\
			MRL_ACTION_KEEP,\
		},\
	},\
	NULL,\
})

	// ---- Blend state ----

	enum
	{
		MRL_BLEND_FACTOR_ZERO,
		MRL_BLEND_FACTOR_ONE,
		MRL_BLEND_FACTOR_SRC_COLOR,
		MRL_BLEND_FACTOR_INV_SRC_COLOR,
		MRL_BLEND_FACTOR_DST_COLOR,
		MRL_BLEND_FACTOR_INV_DST_COLOR,
		MRL_BLEND_FACTOR_SRC_ALPHA,
		MRL_BLEND_FACTOR_INV_SRC_ALPHA,
		MRL_BLEND_FACTOR_DST_ALPHA,
		MRL_BLEND_FACTOR_INV_DST_ALPHA,
	};

	enum
	{
		MRL_BLEND_OP_ADD,
		MRL_BLEND_OP_SUBTRACT,
		MRL_BLEND_OP_REV_SUBTRACT,
		MRL_BLEND_OP_MIN,
		MRL_BLEND_OP_MAX,
	};

	struct mrl_blend_state_desc_t
	{
		/// <summary>
		///		Is blend enabled?
		///		Valid values:
		///		- MGL_FALSE;
		///		- MGL_TRUE;
		/// </summary>
		mgl_bool_t blend_enabled;

		struct
		{
			/// <summary>
			///		Color blend source factor.
			///		Valid values:
			///		- MRL_BLEND_FACTOR_ZERO;
			///		- MRL_BLEND_FACTOR_ONE;
			///		- MRL_BLEND_FACTOR_SRC_COLOR;
			///		- MRL_BLEND_FACTOR_INV_SRC_COLOR;
			///		- MRL_BLEND_FACTOR_DST_COLOR;
			///		- MRL_BLEND_FACTOR_INV_DST_COLOR;
			///		- MRL_BLEND_FACTOR_SRC_ALPHA;
			///		- MRL_BLEND_FACTOR_INV_SRC_ALPHA;
			///		- MRL_BLEND_FACTOR_DST_ALPHA;
			///		- MRL_BLEND_FACTOR_INV_DST_ALPHA;
			/// </summary>
			mgl_enum_t src;

			/// <summary>
			///		Color blend destination factor.
			///		Valid values:
			///		- MRL_BLEND_FACTOR_ZERO;
			///		- MRL_BLEND_FACTOR_ONE;
			///		- MRL_BLEND_FACTOR_SRC_COLOR;
			///		- MRL_BLEND_FACTOR_INV_SRC_COLOR;
			///		- MRL_BLEND_FACTOR_DST_COLOR;
			///		- MRL_BLEND_FACTOR_INV_DST_COLOR;
			///		- MRL_BLEND_FACTOR_SRC_ALPHA;
			///		- MRL_BLEND_FACTOR_INV_SRC_ALPHA;
			///		- MRL_BLEND_FACTOR_DST_ALPHA;
			///		- MRL_BLEND_FACTOR_INV_DST_ALPHA;
			/// </summary>
			mgl_enum_t dst;

			/// <summary>
			///		Color blend operation.
			///		Valid values:
			///		- MRL_BLEND_OP_ADD;
			///		- MRL_BLEND_OP_SUBTRACT;
			///		- MRL_BLEND_OP_REV_SUBTRACT;
			///		- MRL_BLEND_OP_MIN;
			///		- MRL_BLEND_OP_MAX;
			/// </summary>
			mgl_enum_t op;
		} color;

		struct
		{
			/// <summary>
			///		Alpha blend source factor.
			///		Valid values:
			///		- MRL_BLEND_FACTOR_ZERO;
			///		- MRL_BLEND_FACTOR_ONE;
			///		- MRL_BLEND_FACTOR_SRC_COLOR;
			///		- MRL_BLEND_FACTOR_INV_SRC_COLOR;
			///		- MRL_BLEND_FACTOR_DST_COLOR;
			///		- MRL_BLEND_FACTOR_INV_DST_COLOR;
			///		- MRL_BLEND_FACTOR_SRC_ALPHA;
			///		- MRL_BLEND_FACTOR_INV_SRC_ALPHA;
			///		- MRL_BLEND_FACTOR_DST_ALPHA;
			///		- MRL_BLEND_FACTOR_INV_DST_ALPHA;
			/// </summary>
			mgl_enum_t src;

			/// <summary>
			///		Alpha blend destination factor.
			///		Valid values:
			///		- MRL_BLEND_FACTOR_ZERO;
			///		- MRL_BLEND_FACTOR_ONE;
			///		- MRL_BLEND_FACTOR_SRC_COLOR;
			///		- MRL_BLEND_FACTOR_INV_SRC_COLOR;
			///		- MRL_BLEND_FACTOR_DST_COLOR;
			///		- MRL_BLEND_FACTOR_INV_DST_COLOR;
			///		- MRL_BLEND_FACTOR_SRC_ALPHA;
			///		- MRL_BLEND_FACTOR_INV_SRC_ALPHA;
			///		- MRL_BLEND_FACTOR_DST_ALPHA;
			///		- MRL_BLEND_FACTOR_INV_DST_ALPHA;
			/// </summary>
			mgl_enum_t dst;

			/// <summary>
			///		Alpha blend operation.
			///		Valid values:
			///		- MRL_BLEND_OP_ADD;
			///		- MRL_BLEND_OP_SUBTRACT;
			///		- MRL_BLEND_OP_REV_SUBTRACT;
			///		- MRL_BLEND_OP_MIN;
			///		- MRL_BLEND_OP_MAX;
			/// </summary>
			mgl_enum_t op;
		} alpha;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_BLEND_STATE_DESC ((mrl_blend_state_desc_t) {\
	MGL_FALSE,\
	{\
		MRL_BLEND_FACTOR_SRC_ALPHA,\
		MRL_BLEND_FACTOR_INV_SRC_ALPHA,\
		MRL_BLEND_OP_ADD,\
	},\
	{\
		MRL_BLEND_FACTOR_ONE,\
		MRL_BLEND_FACTOR_ZERO,\
		MRL_BLEND_OP_ADD,\
	},\
	NULL,\
})

	// ---- Sampler ----

	enum
	{
		MRL_SAMPLER_FILTER_NONE,
		MRL_SAMPLER_FILTER_NEAREST,
		MRL_SAMPLER_FILTER_LINEAR,
	};

	enum
	{
		MRL_SAMPLER_ADDRESS_REPEAT,
		MRL_SAMPLER_ADDRESS_MIRROR,
		MRL_SAMPLER_ADDRESS_CLAMP,
		MRL_SAMPLER_ADDRESS_BORDER,
	};

	struct mrl_sampler_desc_t
	{
		/// <summary>
		///		Specifies the border color of the texture used by the MRL_SAMPLER_ADDRESS_BORDER address mode.
		/// </summary>
		mgl_f32_t border_color[4];

		/// <summary>
		///		Minifying filter.
		///		Valid values:
		///		- MRL_SAMPLER_FILTER_NEAREST;
		///		- MRL_SAMPLER_FILTER_LINEAR;
		/// </summary>
		mgl_enum_t min_filter;

		/// <summary>
		///		Magnifying filter.
		///		Valid values:
		///		- MRL_SAMPLER_FILTER_NEAREST;
		///		- MRL_SAMPLER_FILTER_LINEAR;
		/// </summary>
		mgl_enum_t mag_filter;

		/// <summary>
		///		Mipmapping filter.
		///		Valid values:
		///		- MRL_SAMPLER_FILTER_NONE: deactivates mipmapping;
		///		- MRL_SAMPLER_FILTER_NEAREST;
		///		- MRL_SAMPLER_FILTER_LINEAR;
		/// </summary>
		mgl_enum_t mip_filter;

		/// <summary>
		///		U address mode.
		///		Valid values:
		///		- MRL_SAMPLER_ADDRESS_REPEAT;
		///		- MRL_SAMPLER_ADDRESS_MIRROR;
		///		- MRL_SAMPLER_ADDRESS_CLAMP;
		///		- MRL_SAMPLER_ADDRESS_BORDER;
		/// </summary>
		mgl_enum_t address_u;

		/// <summary>
		///		V address mode.
		///		Valid values:
		///		- MRL_SAMPLER_ADDRESS_REPEAT;
		///		- MRL_SAMPLER_ADDRESS_MIRROR;
		///		- MRL_SAMPLER_ADDRESS_CLAMP;
		///		- MRL_SAMPLER_ADDRESS_BORDER;
		/// </summary>
		mgl_enum_t address_v;

		/// <summary>
		///		W address mode.
		///		Valid values:
		///		- MRL_SAMPLER_ADDRESS_REPEAT;
		///		- MRL_SAMPLER_ADDRESS_MIRROR;
		///		- MRL_SAMPLER_ADDRESS_CLAMP;
		///		- MRL_SAMPLER_ADDRESS_BORDER;
		/// </summary>
		mgl_enum_t address_w;

		/// <summary>
		///		Max anisotropic samples used while filtering.
		///		Set to 1 to disable anisotropic filtering.
		///		This feature may not be supported on some GPUs, and if it is not, it will be ignored.
		///		Valid values: 1 - MRL_PROPERTY_MAX_ANISTROPY. 
		/// </summary>
		mgl_u32_t max_anisotropy;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_SAMPLER_DESC ((mrl_sampler_desc_t) {\
	{ 0.0f, 0.0f, 0.0f, 0.0f },\
	MRL_SAMPLER_FILTER_NEAREST,\
	MRL_SAMPLER_FILTER_NEAREST,\
	MRL_SAMPLER_FILTER_NONE,\
	MRL_SAMPLER_ADDRESS_REPEAT,\
	MRL_SAMPLER_ADDRESS_REPEAT,\
	MRL_SAMPLER_ADDRESS_REPEAT,\
	1,\
	NULL,\
})

	// ---- Texture formats ----

	enum
	{
		/// <summary>
		///		8-bits normalized unsigned integer R color component.
		/// </summary>
		MRL_TEXTURE_FORMAT_R8_UN,

		/// <summary>
		///		8-bits normalized signed integer R color component.
		/// </summary>
		MRL_TEXTURE_FORMAT_R8_SN,

		/// <summary>
		///		8-bits unsigned integer R color component.
		/// </summary>
		MRL_TEXTURE_FORMAT_R8_UI,

		/// <summary>
		///		8-bits signed integer R color component.
		/// </summary>
		MRL_TEXTURE_FORMAT_R8_SI,

		/// <summary>
		///		8-bits normalized unsigned integer RG color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RG8_UN,

		/// <summary>
		///		8-bits normalized signed integer RG color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RG8_SN,

		/// <summary>
		///		8-bits unsigned integer RG color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RG8_UI,

		/// <summary>
		///		8-bits signed integer RG color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RG8_SI,

		/// <summary>
		///		8-bits normalized unsigned integer RGBA color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RGBA8_UN,

		/// <summary>
		///		8-bits normalized signed integer RGBA color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RGBA8_SN,

		/// <summary>
		///		8-bits unsigned integer RGBA color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RGBA8_UI,

		/// <summary>
		///		8-bits signed integer RGBA color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RGBA8_SI,

		/// <summary>
		///		16-bits normalized unsigned integer R color component.
		/// </summary>
		MRL_TEXTURE_FORMAT_R16_UN,

		/// <summary>
		///		16-bits normalized signed integer R color component.
		/// </summary>
		MRL_TEXTURE_FORMAT_R16_SN,

		/// <summary>
		///		16-bits unsigned integer R color component.
		/// </summary>
		MRL_TEXTURE_FORMAT_R16_UI,

		/// <summary>
		///		16-bits signed integer R color component.
		/// </summary>
		MRL_TEXTURE_FORMAT_R16_SI,

		/// <summary>
		///		16-bits normalized unsigned integer RG color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RG16_UN,

		/// <summary>
		///		16-bits normalized signed integer RG color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RG16_SN,

		/// <summary>
		///		16-bits unsigned integer RG color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RG16_UI,

		/// <summary>
		///		16-bits signed integer RG color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RG16_SI,

		/// <summary>
		///		16-bits normalized unsigned integer RGBA color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RGBA16_UN,

		/// <summary>
		///		16-bits normalized signed integer RGBA color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RGBA16_SN,

		/// <summary>
		///		16-bits unsigned integer RGBA color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RGBA16_UI,

		/// <summary>
		///		16-bits signed integer RGBA color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RGBA16_SI,

		/// <summary>
		///		32-bits unsigned integer R color component.
		/// </summary>
		MRL_TEXTURE_FORMAT_R32_UI,

		/// <summary>
		///		32-bits signed integer R color component.
		/// </summary>
		MRL_TEXTURE_FORMAT_R32_SI,

		/// <summary>
		///		32-bits floating point R color component.
		/// </summary>
		MRL_TEXTURE_FORMAT_R32_F,

		/// <summary>
		///		32-bits unsigned integer RG color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RG32_UI,

		/// <summary>
		///		32-bits signed integer RG color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RG32_SI,

		/// <summary>
		///		32-bits floating point RG color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RG32_F,

		/// <summary>
		///		32-bits unsigned integer RGBA color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RGBA32_UI,

		/// <summary>
		///		32-bits signed integer RGBA color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RGBA32_SI,

		/// <summary>
		///		32-bits floating point RGBA color components.
		/// </summary>
		MRL_TEXTURE_FORMAT_RGBA32_F,

		/// <summary>
		///		16-bits depth value.
		///		This format can only be used on 2D textures.
		/// </summary>
		MRL_TEXTURE_FORMAT_D16,

		/// <summary>
		///		32-bits depth value.
		///		This format can only be used on 2D textures.
		/// </summary>
		MRL_TEXTURE_FORMAT_D32,

		/// <summary>
		///		24-bits depth value.
		///		8-bits stencil value.
		///		This format can only be used on 2D textures.
		/// </summary>
		MRL_TEXTURE_FORMAT_D24S8,

		/// <summary>
		///		These textures cannot be manually updated.
		///		32-bits depth value.
		///		8-bits stencil value.
		///		This format can only be used on 2D textures.
		/// </summary>
		MRL_TEXTURE_FORMAT_D32S8,

	};

	// ---- Texture usage modes ----

	enum
	{
		/// <summary>
		///		Default texture usage mode.
		/// </summary>
		MRL_TEXTURE_USAGE_DEFAULT,

		/// <summary>
		///		The texture can be used as a render target.
		/// </summary>
		MRL_TEXTURE_USAGE_RENDER_TARGET,
	};

	// ---- Max texture mip level count ----

#define MRL_MAX_MIP_LEVEL_COUNT 16

	// ---- Texture 1D ----

	struct mrl_texture_1d_desc_t
	{
		/// <summary>
		///		Initial texture data.
		///		To initialize a texture with NULL data, just set the pointer to NULL.
		///		Each member of the array points to a mip level, being the first member the 0th mip level.
		///		Each mip level has half of the side of the previous one.
		/// </summary>
		const void* data[MRL_MAX_MIP_LEVEL_COUNT];

		/// <summary>
		///		Texture mip level count.
		///		Valid values: 1 - MRL_MAX_MIP_LEVEL_COUNT;
		/// </summary>
		mgl_u32_t mip_level_count;

		/// <summary>
		///		Texture width.
		/// </summary>
		mgl_u64_t width;

		/// <summary>
		///		Texture usage mode.
		///		Valid values:
		///		- MRL_TEXTURE_USAGE_DEFAULT;
		///		- MRL_TEXTURE_USAGE_RENDER_TARGET;
		/// </summary>
		mgl_enum_t usage;

		/// <summary>
		///		Texture data format.
		///		Valid values:
		///			- All R, RG, RGBA formats except the depth and stencil component formats.
		/// </summary>
		mgl_enum_t format;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_TEXTURE_1D_DESC ((mrl_texture_1d_desc_t) {\
	{ NULL },\
	1,\
	256,\
	MRL_TEXTURE_USAGE_DEFAULT,\
	MRL_TEXTURE_FORMAT_RGBA32_F,\
	NULL,\
})

	struct mrl_texture_1d_update_desc_t
	{
		/// <summary>
		///		New texture data.
		/// </summary>
		const void* data;

		/// <summary>
		///		New data width.
		/// </summary>
		mgl_u64_t width;

		/// <summary>
		///		Destination X coordinate.
		/// </summary>
		mgl_u64_t dst_x;

		/// <summary>
		///		Mip level to update.
		///		Valid values: 1 - MRL_MAX_MIP_LEVEL_COUNT;
		/// </summary>
		mgl_u32_t mip_level;
	};

#define MRL_DEFAULT_TEXTURE_1D_UPDATE_DESC ((mrl_texture_1d_update_desc_t) {\
	NULL,\
	1,\
	0,\
	1,\
})

	// ---- Texture 2D ----

	struct mrl_texture_2d_desc_t
	{
		/// <summary>
		///		Initial texture data.
		///		To initialize a texture with NULL data, just set the pointer to NULL.
		///		Each member of the array points to a mip level, being the first member the 0th mip level.
		///		Each mip level has half of the side of the previous one.
		/// </summary>
		const void* data[MRL_MAX_MIP_LEVEL_COUNT];

		/// <summary>
		///		Texture mip level count.
		///		Valid values: 1 - MRL_MAX_MIP_LEVEL_COUNT;
		/// </summary>
		mgl_u32_t mip_level_count;

		/// <summary>
		///		Texture width.
		/// </summary>
		mgl_u64_t width;

		/// <summary>
		///		Texture height.
		/// </summary>
		mgl_u64_t height;

		/// <summary>
		///		Texture usage mode.
		///		Valid values:
		///		- MRL_TEXTURE_USAGE_DEFAULT;
		///		- MRL_TEXTURE_USAGE_RENDER_TARGET;
		/// </summary>
		mgl_enum_t usage;

		/// <summary>
		///		Texture data format.
		///		Valid values:
		///			- All texture formats.
		/// </summary>
		mgl_enum_t format;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_TEXTURE_2D_DESC ((mrl_texture_2d_desc_t) {\
	{ NULL },\
	1,\
	256,\
	256,\
	MRL_TEXTURE_USAGE_DEFAULT,\
	MRL_TEXTURE_FORMAT_RGBA32_F,\
	NULL,\
})

	struct mrl_texture_2d_update_desc_t
	{
		/// <summary>
		///		New texture data.
		/// </summary>
		const void* data;

		/// <summary>
		///		New data width.
		/// </summary>
		mgl_u64_t width;

		/// <summary>
		///		New data height.
		/// </summary>
		mgl_u64_t height;

		/// <summary>
		///		Destination X coordinate.
		/// </summary>
		mgl_u64_t dst_x;

		/// <summary>
		///		Destination Y coordinate.
		/// </summary>
		mgl_u64_t dst_y;

		/// <summary>
		///		Mip level to update.
		///		Valid values: 1 - MRL_MAX_MIP_LEVEL_COUNT;
		/// </summary>
		mgl_u32_t mip_level;
	};

#define MRL_DEFAULT_TEXTURE_2D_UPDATE_DESC ((mrl_texture_2d_update_desc_t) {\
	NULL,\
	1,\
	1,\
	0,\
	0,\
	1,\
})

	// ---- Texture 3D ----

	struct mrl_texture_3d_desc_t
	{
		/// <summary>
		///		Initial texture data.
		///		To initialize a texture with NULL data, just set the pointer to NULL.
		///		Each member of the array points to a mip level, being the first member the 0th mip level.
		///		Each mip level has half of the side of the previous one.
		/// </summary>
		const void* data[MRL_MAX_MIP_LEVEL_COUNT];

		/// <summary>
		///		Texture mip level count.
		///		Valid values: 1 - MRL_MAX_MIP_LEVEL_COUNT;
		/// </summary>
		mgl_u32_t mip_level_count;

		/// <summary>
		///		Texture width.
		/// </summary>
		mgl_u64_t width;

		/// <summary>
		///		Texture height.
		/// </summary>
		mgl_u64_t height;

		/// <summary>
		///		Texture depth.
		/// </summary>
		mgl_u64_t depth;

		/// <summary>
		///		Texture usage mode.
		///		Valid values:
		///		- MRL_TEXTURE_USAGE_DEFAULT;
		///		- MRL_TEXTURE_USAGE_RENDER_TARGET;
		/// </summary>
		mgl_enum_t usage;

		/// <summary>
		///		Texture data format.
		///		Valid values:
		///			- All R, RG, RGBA formats except the depth and stencil component formats.
		/// </summary>
		mgl_enum_t format;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_TEXTURE_3D_DESC ((mrl_texture_3d_desc_t) {\
	{ NULL },\
	1,\
	256,\
	256,\
	256,\
	MRL_TEXTURE_USAGE_DEFAULT,\
	MRL_TEXTURE_FORMAT_RGBA32_F,\
	NULL,\
})

	struct mrl_texture_3d_update_desc_t
	{
		/// <summary>
		///		New texture data.
		/// </summary>
		const void* data;

		/// <summary>
		///		New data width.
		/// </summary>
		mgl_u64_t width;

		/// <summary>
		///		New data height.
		/// </summary>
		mgl_u64_t height;

		/// <summary>
		///		New data depth.
		/// </summary>
		mgl_u64_t depth;

		/// <summary>
		///		Destination X coordinate.
		/// </summary>
		mgl_u64_t dst_x;

		/// <summary>
		///		Destination Y coordinate.
		/// </summary>
		mgl_u64_t dst_y;

		/// <summary>
		///		Destination Z coordinate.
		/// </summary>
		mgl_u64_t dst_z;

		/// <summary>
		///		Mip level to update.
		///		Valid values: 1 - MRL_MAX_MIP_LEVEL_COUNT;
		/// </summary>
		mgl_u32_t mip_level;
	};

#define MRL_DEFAULT_TEXTURE_3D_UPDATE_DESC ((mrl_texture_3d_update_desc_t) {\
	NULL,\
	1,\
	1,\
	1,\
	0,\
	0,\
	0,\
	1,\
})

	// ---- Cube map ----

	enum
	{
		/// <summary>
		///		Cube map positive X face.
		/// </summary>
		MRL_CUBE_MAP_FACE_POSITIVE_X = 0,

		/// <summary>
		///		Cube map negative X face.
		/// </summary>
		MRL_CUBE_MAP_FACE_NEGATIVE_X = 1,

		/// <summary>
		///		Cube map positive Y face.
		/// </summary>
		MRL_CUBE_MAP_FACE_POSITIVE_Y = 2,

		/// <summary>
		///		Cube map negative Y face.
		/// </summary>
		MRL_CUBE_MAP_FACE_NEGATIVE_Y = 3,

		/// <summary>
		///		Cube map positive Z face.
		/// </summary>
		MRL_CUBE_MAP_FACE_POSITIVE_Z = 4,

		/// <summary>
		///		Cube map negative Z face.
		/// </summary>
		MRL_CUBE_MAP_FACE_NEGATIVE_Z = 5,
	};

	struct mrl_cube_map_desc_t
	{
		/// <summary>
		///		Initial cube map data.
		///		Faces are indexed using:
		///			MRL_CUBE_MAP_FACE_POSITIVE_X;
		///			MRL_CUBE_MAP_FAEC_NEGATIVE_X;
		///			MRL_CUBE_MAP_FACE_POSITIVE_Y;
		///			MRL_CUBE_MAP_FACE_NEGATIVE_Y;
		///			MRL_CUBE_MAP_FACE_POSITIVE_Z;
		///			MRL_CUBE_MAP_FACE_NEGATIVE_Z;
		///		To initialize a face with NULL data, just set the pointer to NULL.
		///		Each member of the array of each face points to a mip level, being the first member the 0th mip level.
		///		Each mip level has half of the side of the previous one.
		/// </summary>
		const void* data[6][MRL_MAX_MIP_LEVEL_COUNT];

		/// <summary>
		///		Texture mip level count.
		///		Valid values: 1 - MRL_MAX_MIP_LEVEL_COUNT;
		/// </summary>
		mgl_u32_t mip_level_count;

		/// <summary>
		///		Cube map face width.
		/// </summary>
		mgl_u64_t width;

		/// <summary>
		///		Cube map face height.
		/// </summary>
		mgl_u64_t height;

		/// <summary>
		///		Texture usage mode.
		///		Valid values:
		///		- MRL_TEXTURE_USAGE_DEFAULT;
		///		- MRL_TEXTURE_USAGE_RENDER_TARGET;
		/// </summary>
		mgl_enum_t usage;

		/// <summary>
		///		Texture data format.
		///		Valid values:
		///			- All R, RG, RGBA formats except the depth and stencil component formats.
		/// </summary>
		mgl_enum_t format;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_CUBE_MAP_DESC ((mrl_cube_map_desc_t) {\
	{ { NULL } },\
	1,\
	256,\
	256,\
	MRL_TEXTURE_USAGE_DEFAULT,\
	MRL_TEXTURE_FORMAT_RGBA32_F,\
	NULL,\
})

	struct mrl_cube_map_update_desc_t
	{
		/// <summary>
		///		New face data.
		/// </summary>
		const void* data;

		/// <summary>
		///		The face which will be updated.
		///		Valid values:
		///			MRL_CUBE_MAP_FACE_POSITIVE_X;
		///			MRL_CUBE_MAP_FAEC_NEGATIVE_X;
		///			MRL_CUBE_MAP_FACE_POSITIVE_Y;
		///			MRL_CUBE_MAP_FACE_NEGATIVE_Y;
		///			MRL_CUBE_MAP_FACE_POSITIVE_Z;
		///			MRL_CUBE_MAP_FACE_NEGATIVE_Z;
		/// </summary>
		mgl_enum_t face;

		/// <summary>
		///		New data width.
		/// </summary>
		mgl_u64_t width;

		/// <summary>
		///		New data height.
		/// </summary>
		mgl_u64_t height;

		/// <summary>
		///		Destination X coordinate.
		/// </summary>
		mgl_u64_t dst_x;

		/// <summary>
		///		Destination Y coordinate.
		/// </summary>
		mgl_u64_t dst_y;

		/// <summary>
		///		Mip level to update.
		///		Valid values: 1 - MRL_MAX_MIP_LEVEL_COUNT;
		/// </summary>
		mgl_u32_t mip_level;
	};

#define MRL_DEFAULT_CUBE_MAP_UPDATE_DESC ((mrl_cube_map_update_desc_t) {\
	NULL,\
	MRL_CUBE_MAP_FACE_POSITIVE_X,\
	1,\
	1,\
	0,\
	0,\
	1,\
})

	// ---- Constant buffers ----

#define MRL_MAX_CONSTANT_BUFFER_ELEMENT_NAME_SIZE 64
#define MRL_MAX_CONSTANT_BUFFER_ELEMENT_COUNT 32

	enum
	{
		MRL_CONSTANT_BUFFER_MEMBER_I32,
		MRL_CONSTANT_BUFFER_MEMBER_U32,
		MRL_CONSTANT_BUFFER_MEMBER_F32,

		MRL_CONSTANT_BUFFER_MEMBER_I32V2,
		MRL_CONSTANT_BUFFER_MEMBER_I32V3,
		MRL_CONSTANT_BUFFER_MEMBER_I32V4,
		MRL_CONSTANT_BUFFER_MEMBER_U32V2,
		MRL_CONSTANT_BUFFER_MEMBER_U32V3,
		MRL_CONSTANT_BUFFER_MEMBER_U32V4,
		MRL_CONSTANT_BUFFER_MEMBER_F32V2,
		MRL_CONSTANT_BUFFER_MEMBER_F32V3,
		MRL_CONSTANT_BUFFER_MEMBER_F32V4,

		MRL_CONSTANT_BUFFER_MEMBER_F32M2X2,
		MRL_CONSTANT_BUFFER_MEMBER_F32M2X3,
		MRL_CONSTANT_BUFFER_MEMBER_F32M2X4,
		MRL_CONSTANT_BUFFER_MEMBER_F32M3X2,
		MRL_CONSTANT_BUFFER_MEMBER_F32M3X3,
		MRL_CONSTANT_BUFFER_MEMBER_F32M3X4,
		MRL_CONSTANT_BUFFER_MEMBER_F32M4X2,
		MRL_CONSTANT_BUFFER_MEMBER_F32M4X3,
		MRL_CONSTANT_BUFFER_MEMBER_F32M4X4,
	};

	struct mrl_constant_buffer_structure_element_t
	{
		/// <summary>
		///		Name of the element.
		/// </summary>
		mgl_chr8_t name[MRL_MAX_CONSTANT_BUFFER_ELEMENT_NAME_SIZE];

		/// <summary>
		///		The byte offset into the beginning of the buffer for this element.
		/// </summary>
		mgl_u64_t offset;

		/// <summary>
		///		Size of the element.
		///		For non-arrays, this value is set to 1.
		///		For arrays, this is the length of the array.
		/// </summary>
		mgl_u64_t size;

		/// <summary>
		///		Stride between each array element.
		/// </summary>
		mgl_u64_t array_stride;
	};

	struct mrl_constant_buffer_structure_t
	{
		/// <summary>
		///		Size of the buffer.
		/// </summary>
		mgl_u64_t size;

		/// <summary>
		///		Number of elements.
		/// </summary>
		mgl_u64_t element_count;

		/// <summary>
		///		Buffer elements.
		/// </summary>
		mrl_constant_buffer_structure_element_t elements[MRL_MAX_CONSTANT_BUFFER_ELEMENT_COUNT];
	};

	enum
	{
		/// <summary>
		///		The buffer should be read a lot and writen to rarely.
		/// </summary>
		MRL_CONSTANT_BUFFER_USAGE_DEFAULT,

		/// <summary>
		///		Static buffer, data is read-only and cannot be changed. 
		/// </summary>
		MRL_CONSTANT_BUFFER_USAGE_STATIC,

		/// <summary>
		///		The buffer can be written to frequently and read a lot of times.
		///		Allows mrl_map_constant_buffer to be used to update the buffer.
		/// </summary>
		MRL_CONSTANT_BUFFER_USAGE_DYNAMIC,

		/// <summary>
		///		Used for buffers that are updated every frame.
		/// </summary>
		MRL_CONSTANT_BUFFER_USAGE_STREAM,
	};

	struct mrl_constant_buffer_desc_t
	{
		/// <summary>
		///		Initial constant buffer data.
		///		When the usage mode is not set to MRL_CONSTANT_BUFFER_USAGE_STATIC, this pointer can be set to NULL to create an empty buffer.
		/// </summary>
		const void* data;

		/// <summary>
		///		Constant buffer size.
		/// </summary>
		mgl_u64_t size;

		/// <summary>
		///		Constant buffer usage mode.
		///		Valid values:
		///		- MRL_CONSTANT_BUFFER_USAGE_DEFAULT;
		///		- MRL_CONSTANT_BUFFER_USAGE_STATIC;
		///		- MRL_CONSTANT_BUFFER_USAGE_DYNAMIC;
		///		- MRL_CONSTANT_BUFFER_USAGE_STREAM;
		/// </summary>
		mgl_enum_t usage;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_CONSTANT_BUFFER_DESC ((mrl_constant_buffer_desc_t) {\
	NULL,\
	0,\
	MRL_CONSTANT_BUFFER_USAGE_DEFAULT,\
	NULL,\
})

	// ---- Index buffers ----

	enum
	{
		/// <summary>
		///		The buffer should be read a lot and writen to rarely.
		/// </summary>
		MRL_INDEX_BUFFER_USAGE_DEFAULT,

		/// <summary>
		///		Static buffer, data is read-only and cannot be changed. 
		/// </summary>
		MRL_INDEX_BUFFER_USAGE_STATIC,

		/// <summary>
		///		The buffer can be written to frequently and read a lot of times.
		///		Allows mrl_map_index_buffer to be used to update the buffer.
		/// </summary>
		MRL_INDEX_BUFFER_USAGE_DYNAMIC,

		/// <summary>
		///		Used for buffers that are updated every frame.
		/// </summary>
		MRL_INDEX_BUFFER_USAGE_STREAM,
	};

	enum
	{
		MRL_INDEX_BUFFER_FORMAT_U16,
		MRL_INDEX_BUFFER_FORMAT_U32,
	};

	struct mrl_index_buffer_desc_t
	{
		/// <summary>
		///		Initial index buffer data.
		///		When the usage mode is not set to MRL_INDEX_BUFFER_USAGE_STATIC, this pointer can be set to NULL to create an empty buffer.
		/// </summary>
		const void* data;

		/// <summary>
		///		Index buffer size.
		/// </summary>
		mgl_u64_t size;

		/// <summary>
		///		Index buffer usage mode.
		///		Valid values:
		///		- MRL_INDEX_BUFFER_USAGE_DEFAULT;
		///		- MRL_INDEX_BUFFER_USAGE_STATIC;
		///		- MRL_INDEX_BUFFER_USAGE_DYNAMIC;
		///		- MRL_INDEX_BUFFER_USAGE_STREAM;
		/// </summary>
		mgl_enum_t usage;

		/// <summary>
		///		Index data format.
		///		Valid values:
		///		- MRL_INDEX_BUFFER_FORMAT_U16;
		///		- MRL_INDEX_BUFFER_FORMAT_U32;
		/// </summary>
		mgl_enum_t format;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_INDEX_BUFFER_DESC ((mrl_index_buffer_desc_t) {\
	NULL,\
	0,\
	MRL_INDEX_BUFFER_USAGE_DEFAULT,\
	MRL_INDEX_BUFFER_FORMAT_U32,\
	NULL,\
})

	// ---- Vertex buffers ----

	enum
	{
		/// <summary>
		///		The buffer should be read a lot and writen to rarely.
		/// </summary>
		MRL_VERTEX_BUFFER_USAGE_DEFAULT,

		/// <summary>
		///		Static buffer, data is read-only and cannot be changed. 
		/// </summary>
		MRL_VERTEX_BUFFER_USAGE_STATIC,

		/// <summary>
		///		The buffer can be written to frequently and read a lot of times.
		///		Allows mrl_map_index_buffer to be used to update the buffer.
		/// </summary>
		MRL_VERTEX_BUFFER_USAGE_DYNAMIC,

		/// <summary>
		///		Used for buffers that are updated every frame.
		/// </summary>
		MRL_VERTEX_BUFFER_USAGE_STREAM,
	};

	struct mrl_vertex_buffer_desc_t
	{
		/// <summary>
		///		Initial vertex buffer data.
		///		When the usage mode is not set to MRL_VERTEX_BUFFER_USAGE_STATIC, this pointer can be set to NULL to create an empty buffer.
		/// </summary>
		const void* data;

		/// <summary>
		///		Vertex buffer size.
		/// </summary>
		mgl_u64_t size;

		/// <summary>
		///		Vertex buffer usage mode.
		///		Valid values:
		///		- MRL_VERTEX_BUFFER_USAGE_DEFAULT;
		///		- MRL_VERTEX_BUFFER_USAGE_STATIC;
		///		- MRL_VERTEX_BUFFER_USAGE_DYNAMIC;
		///		- MRL_VERTEX_BUFFER_USAGE_STREAM;
		/// </summary>
		mgl_enum_t usage;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_VERTEX_BUFFER_DESC ((mrl_vertex_buffer_desc_t) {\
	NULL,\
	0,\
	MRL_VERTEX_BUFFER_USAGE_DEFAULT,\
	NULL,\
})

	// ---- Vertex arrays ----

#define MRL_MAX_VERTEX_ELEMENT_NAME_SIZE 32
#define MRL_MAX_VERTEX_ARRAY_ELEMENT_COUNT 8
#define MRL_MAX_VERTEX_ARRAY_BUFFER_COUNT 8

	enum
	{
		MRL_VERTEX_ELEMENT_TYPE_I8,
		MRL_VERTEX_ELEMENT_TYPE_I16,
		MRL_VERTEX_ELEMENT_TYPE_I32,
		MRL_VERTEX_ELEMENT_TYPE_U8,
		MRL_VERTEX_ELEMENT_TYPE_U16,
		MRL_VERTEX_ELEMENT_TYPE_U32,
		MRL_VERTEX_ELEMENT_TYPE_N8,
		MRL_VERTEX_ELEMENT_TYPE_N16,
		MRL_VERTEX_ELEMENT_TYPE_NU8,
		MRL_VERTEX_ELEMENT_TYPE_NU16,
		MRL_VERTEX_ELEMENT_TYPE_F32,
	};

	struct mrl_vertex_element_t
	{
		/// <summary>
		///		Type of the vertex element.
		///		Valid values:
		///		- 
		/// </summary>
		mgl_enum_u32_t type;

		/// <summary>
		///		Vertex element attribute name.
		///		This specifies the element input name on the vertex shader.
		/// </summary>
		mgl_chr8_t name[MRL_MAX_VERTEX_ELEMENT_NAME_SIZE];

		/// <summary>
		///		Size of the vertex element.
		///		Valid values:
		///		- 1;
		///		- 2;
		///		- 3;
		///		- 4;
		/// </summary>
		mgl_u8_t size;

		struct
		{
			/// <summary>
			///		Stride between each vertex element in the buffer.
			/// </summary>
			mgl_u64_t stride;

			/// <summary>
			///		Offset of the first vertex element in the buffer.
			/// </summary>
			mgl_u64_t offset;

			/// <summary>
			///		Index of the buffer where the vertex element data is stored.
			/// </summary>
			mgl_u32_t index;
		} buffer;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};
		
#define MRL_DEFAULT_VERTEX_ELEMENT ((mrl_vertex_element_t) {\
	-1,\
	{ 0 },\
	1,\
	{\
		0,\
		0,\
		0,\
	},\
	NULL,\
})

	struct mrl_vertex_array_desc_t
	{
		/// <summary>
		///		Number of vertex elements used on this vertex array.
		/// </summary>
		mgl_u32_t element_count;

		/// <summary>
		///		Number of vertex buffers used on this vertex array.
		/// </summary>
		mgl_u32_t buffer_count;

		/// <summary>
		///		Vertex elements.
		/// </summary>
		mrl_vertex_element_t elements[MRL_MAX_VERTEX_ARRAY_ELEMENT_COUNT];

		/// <summary>
		///		Vertex buffers where the vertex data is stored.
		/// </summary>
		mrl_vertex_buffer_t* buffers[MRL_MAX_VERTEX_ARRAY_BUFFER_COUNT];

		/// <summary>
		///		Shader pipeline used with this vertex array.
		/// </summary>
		mrl_shader_pipeline_t* shader_pipeline;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_VERTEX_ARRAY_DESC ((mrl_vertex_array_desc_t) {\
	0,\
	0,\
	{ MRL_DEFAULT_VERTEX_ELEMENT },\
	{ NULL },\
	NULL,\
	NULL,\
})

	// ---- Shader stages ----

	enum
	{
		/// <summary>
		///		Vertex shader stage.
		///		Processes each vertex and outputs data to the pixel stage.
		/// </summary>
		MRL_SHADER_STAGE_VERTEX,

		/// <summary>
		///		Pixel shader stage.
		///		Processes each pixel rendered and outputs data to one or more render targets.
		/// </summary>
		MRL_SHADER_STAGE_PIXEL,
	};

	enum
	{
		/// <summary>
		///		SPIR-V shaders (Standard, Portable Intermediate Representation - V).
		/// </summary>
		MRL_SHADER_SOURCE_SPIRV,

		/// <summary>
		///		GLSL shaders (OpenGL Shading Language).
		/// </summary>
		MRL_SHADER_SOURCE_GLSL,

		/// <summary>
		///		HLSL shaders (High Level Shading Language).
		/// </summary>
		MRL_SHADER_SOURCE_HLSL,

		/// <summary>
		///		MSL shaders (Metal Shading Language).
		/// </summary>
		MRL_SHADER_SOURCE_MSL,

		/// <summary>
		///		MRSL shaders (Magma Rendering Shading Language).
		/// </summary>
		MRL_SHADER_SOURCE_MRSL,
	};

	struct mrl_shader_stage_desc_t
	{
		/// <summary>
		///		Shader stage.
		///		Valid values:
		///		- MRL_SHADER_STAGE_VERTEX;
		///		- MRL_SHADER_STAGE_PIXEL;
		/// </summary>
		mgl_enum_t stage;

		/// <summary>
		///		Shader source type.
		///		Valid values:
		///		- MRL_SHADER_SOURCE_SPIRV;
		///		- MRL_SHADER_SOURCE_GLSL;
		///		- MRL_SHADER_SOURCE_HLSL;
		///		- MRL_SHADER_SOURCE_MSL;
		///		- MRL_SHADER_SOURCE_MRLS;
		/// 
		///		Some source types may not be supported by a render device.
		///		If a shader source type is not supported, the error MRL_ERROR_UNSUPPORTED_SHADER_SOURCE is returned.
		/// </summary>
		mgl_enum_t src_type;

		/// <summary>
		///		Pointer to shader source data.
		/// </summary>
		const void* src;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_SHADER_STAGE_DESC ((mrl_shader_stage_desc_t) {\
	-1,\
	-1,\
	NULL,\
	NULL,\
})

	// ------- Shader pipeline -------

	struct mrl_shader_pipeline_desc_t
	{
		/// <summary>
		///		Vertex shader stage.
		/// </summary>
		mrl_shader_stage_t* vertex;

		/// <summary>
		///		Pixel shader stage.
		/// </summary>
		mrl_shader_stage_t* pixel;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_SHADER_PIPELINE_DESC ((mrl_shader_pipeline_desc_t) {\
	NULL,\
	NULL,\
	NULL,\
})

	// ------- Render device -------

	enum
	{
		MRL_VSYNC_OFF,
		MRL_VSYNC_ON,
		MRL_VSYNC_ADAPTIVE,
	};

	struct mrl_render_device_desc_t
	{
		/// <summary>
		///		Allocater used by the render device.
		/// </summary>
		void* allocator;

		/// <summary>
		///		Window where the render context will be created on.
		///		If the window type is unsupported, MRL_ERROR_UNSUPPORTED_WINDOW is returned on device creation.
		/// </summary>
		void* window;

		/// <summary>
		///		Vertical synchronization mode.
		///		Valid values:
		///		- MRL_VSYNC_OFF - Disables VSync.
		///		- MRL_VSYNC_ON - Enables VSync.
		///		- MRL_VSYNC_ADAPTIVE - Enables adaptive VSync.
		/// 
		///		If a VSync mode is not supported, it falls back to the previous mode.
		/// </summary>
		mgl_enum_t vsync_mode;

		/// <summary>
		///		Maximum number of framebuffers.
		/// </summary>
		mgl_u64_t max_framebuffer_count;

		/// <summary>
		///		Maximum number of raster states.
		/// </summary>
		mgl_u64_t max_raster_state_count;

		/// <summary>
		///		Maximum number of depth stencil states.
		/// </summary>
		mgl_u64_t max_depth_stencil_state_count;

		/// <summary>
		///		Maximum number of blend states.
		/// </summary>
		mgl_u64_t max_blend_state_count;

		/// <summary>
		///		Maximum number of samplers.
		/// </summary>
		mgl_u64_t max_sampler_count;

		/// <summary>
		///		Maximum number of 1D textures.
		/// </summary>
		mgl_u64_t max_texture_1d_count;

		/// <summary>
		///		Maximum number of 2D textures.
		/// </summary>
		mgl_u64_t max_texture_2d_count;

		/// <summary>
		///		Maximum number of 3D textures.
		/// </summary>
		mgl_u64_t max_texture_3d_count;

		/// <summary>
		///		Maximum number of cube maps.
		/// </summary>
		mgl_u64_t max_cube_map_count;

		/// <summary>
		///		Maximum number of constant buffers.
		/// </summary>
		mgl_u64_t max_constant_buffer_count;

		/// <summary>
		///		Maximum number of index buffers.
		/// </summary>
		mgl_u64_t max_index_buffer_count;

		/// <summary>
		///		Maximum number of vertex buffers.
		/// </summary>
		mgl_u64_t max_vertex_buffer_count;

		/// <summary>
		///		Maximum number of vertex arrays.
		/// </summary>
		mgl_u64_t max_vertex_array_count;

		/// <summary>
		///		Maximum number of shader stages.
		/// </summary>
		mgl_u64_t max_shader_stage_count;

		/// <summary>
		///		Maximum number of shader pipelines.
		/// </summary>
		mgl_u64_t max_shader_pipeline_count;

		/// <summary>
		///		Enable debug mode?
		/// </summary>	
		mgl_bool_t debug_mode;

		/// <summary>
		///		Hint list.
		///		Hints may be ignored by some render devices.
		///		Optional (can be NULL).
		/// </summary>
		const mrl_hint_t* hints;
	};

#define MRL_DEFAULT_RENDER_DEVICE_DESC ((mrl_render_device_desc_t) {\
	NULL,\
	NULL,\
	MRL_VSYNC_ADAPTIVE,\
	64,\
	256,\
	256,\
	256,\
	512,\
	512,\
	512,\
	512,\
	512,\
	512,\
	512,\
	256,\
	256,\
	1024,\
	512,\
	MGL_TRUE,\
	NULL,\
})

	typedef struct mrl_render_device_t mrl_render_device_t;
	struct mrl_render_device_t
	{
		// ------- Framebuffer functions -------
		mrl_error_t(*create_framebuffer)(mrl_render_device_t* rd, mrl_framebuffer_t** fb, const mrl_framebuffer_desc_t* desc);
		void(*destroy_framebuffer)(mrl_render_device_t* rd, mrl_framebuffer_t* fb);
		void(*set_framebuffer)(mrl_render_device_t* rd, mrl_framebuffer_t* fb);

		// ------- Raster state functions -------
		mrl_error_t(*create_raster_state)(mrl_render_device_t* rd, mrl_raster_state_t** s, const mrl_raster_state_desc_t* desc);
		void(*destroy_raster_state)(mrl_render_device_t* rd, mrl_raster_state_t* s);
		void(*set_raster_state)(mrl_render_device_t* rd, mrl_raster_state_t* s);

		// ------- Depth stencil state functions -------
		mrl_error_t(*create_depth_stencil_state)(mrl_render_device_t* rd, mrl_depth_stencil_state_t** s, const mrl_depth_stencil_state_desc_t* desc);
		void(*destroy_depth_stencil_state)(mrl_render_device_t* rd, mrl_depth_stencil_state_t* s);
		void(*set_depth_stencil_state)(mrl_render_device_t* rd, mrl_depth_stencil_state_t* s);

		// ------- Blend state functions -------
		mrl_error_t(*create_blend_state)(mrl_render_device_t* rd, mrl_blend_state_t** s, const mrl_blend_state_desc_t* desc);
		void(*destroy_blend_state)(mrl_render_device_t* rd, mrl_blend_state_t* s);
		void(*set_blend_state)(mrl_render_device_t* rd, mrl_blend_state_t* s);

		// ------- Sampler functions -------
		mrl_error_t(*create_sampler)(mrl_render_device_t* rd, mrl_sampler_t** s, const mrl_sampler_desc_t* desc);
		void(*destroy_sampler)(mrl_render_device_t* rd, mrl_sampler_t* s);
		void(*bind_sampler)(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_sampler_t* s);

		// ------- Texture 1D functions -------
		mrl_error_t(*create_texture_1d)(mrl_render_device_t* rd, mrl_texture_1d_t** tex, const mrl_texture_1d_desc_t* desc);
		void(*destroy_texture_1d)(mrl_render_device_t* rd, mrl_texture_1d_t* tex);
		void(*generate_texture_1d_mipmaps)(mrl_render_device_t* rd, mrl_texture_1d_t* tex);
		void(*bind_texture_1d)(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_texture_1d_t* tex);
		mrl_error_t(*update_texture_1d)(mrl_render_device_t* rd, mrl_texture_1d_t* tex, const mrl_texture_1d_update_desc_t* desc);

		// ------- Texture 2D functions -------
		mrl_error_t(*create_texture_2d)(mrl_render_device_t* rd, mrl_texture_2d_t** tex, const mrl_texture_2d_desc_t* desc);
		void(*destroy_texture_2d)(mrl_render_device_t* rd, mrl_texture_2d_t* tex);
		void(*generate_texture_2d_mipmaps)(mrl_render_device_t* rd, mrl_texture_2d_t* tex);
		void(*bind_texture_2d)(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_texture_2d_t* tex);
		mrl_error_t(*update_texture_2d)(mrl_render_device_t* rd, mrl_texture_2d_t* tex, const mrl_texture_2d_update_desc_t* desc);

		// ------- Texture 3D functions -------
		mrl_error_t(*create_texture_3d)(mrl_render_device_t* rd, mrl_texture_3d_t** tex, const mrl_texture_3d_desc_t* desc);
		void(*destroy_texture_3d)(mrl_render_device_t* rd, mrl_texture_3d_t* tex);
		void(*generate_texture_3d_mipmaps)(mrl_render_device_t* rd, mrl_texture_3d_t* tex);
		void(*bind_texture_3d)(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_texture_3d_t* tex);
		mrl_error_t(*update_texture_3d)(mrl_render_device_t* rd, mrl_texture_3d_t* tex, const mrl_texture_3d_update_desc_t* desc);

		// ------- Cube map functions -------
		mrl_error_t(*create_cube_map)(mrl_render_device_t* rd, mrl_cube_map_t** cb, const mrl_cube_map_desc_t* desc);
		void(*destroy_cube_map)(mrl_render_device_t* rd, mrl_cube_map_t* cb);
		void(*generate_cube_map_mipmaps)(mrl_render_device_t* rd, mrl_cube_map_t* cb);
		void(*bind_cube_map)(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_cube_map_t* cb);
		mrl_error_t(*update_cube_map)(mrl_render_device_t* rd, mrl_cube_map_t* cb, const mrl_cube_map_update_desc_t* desc);

		// ------- Constant buffer functions -------
		mrl_error_t(*create_constant_buffer)(mrl_render_device_t* rd, mrl_constant_buffer_t** cb, const mrl_constant_buffer_desc_t* desc);
		void(*destroy_constant_buffer)(mrl_render_device_t* rd, mrl_constant_buffer_t* cb);
		void(*bind_constant_buffer)(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_constant_buffer_t* cb);
		void*(*map_constant_buffer)(mrl_render_device_t* rd, mrl_constant_buffer_t* cb);
		void(*unmap_constant_buffer)(mrl_render_device_t* rd, mrl_constant_buffer_t* cb);
		void(*update_constant_buffer)(mrl_render_device_t* rd, mrl_constant_buffer_t* cb, mgl_u64_t offset, mgl_u64_t size, const void* data);
		void(*query_constant_buffer_structure)(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_constant_buffer_structure_t* cbs);

		// ------- Index buffer functions -------
		mrl_error_t(*create_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t** ib, const mrl_index_buffer_desc_t* desc);
		void(*destroy_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t* ib);
		void(*set_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t* ib);
		void*(*map_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t* ib);
		void(*unmap_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t* ib);
		void(*update_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t* ib, mgl_u64_t offset, mgl_u64_t size, const void* data);

		// ------- Vertex buffer functions -------
		mrl_error_t(*create_vertex_buffer)(mrl_render_device_t* rd, mrl_vertex_buffer_t** vb, const mrl_vertex_buffer_desc_t* desc);
		void(*destroy_vertex_buffer)(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb);
		void*(*map_vertex_buffer)(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb);
		void(*unmap_vertex_buffer)(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb);
		void(*update_vertex_buffer)(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb, mgl_u64_t offset, mgl_u64_t size, const void* data);

		// ------- Vertex array functions -------
		mrl_error_t(*create_vertex_array)(mrl_render_device_t* rd, mrl_vertex_array_t** va, const mrl_vertex_array_desc_t* desc);
		void(*destroy_vertex_array)(mrl_render_device_t* rd, mrl_vertex_array_t* va);
		void(*set_vertex_array)(mrl_render_device_t* rd, mrl_vertex_array_t* va);

		// ------- Shader functions -------
		mrl_error_t(*create_shader_stage)(mrl_render_device_t* rd, mrl_shader_stage_t** stage, const mrl_shader_stage_desc_t* desc);
		void(*destroy_shader_stage)(mrl_render_device_t* rd, mrl_shader_stage_t* stage);
		mrl_error_t(*create_shader_pipeline)(mrl_render_device_t* rd, mrl_shader_pipeline_t** pipeline, const mrl_shader_pipeline_desc_t* desc);
		void(*destroy_shader_pipeline)(mrl_render_device_t* rd, mrl_shader_pipeline_t* pipeline);
		void(*set_shader_pipeline)(mrl_render_device_t* rd, mrl_shader_pipeline_t* pipeline);
		mrl_shader_binding_point_t*(*get_shader_binding_point)(mrl_render_device_t* rd, mrl_shader_pipeline_t* pipeline, const mgl_chr8_t* name);

		// -------- Draw functions --------
		void(*clear_color)(mrl_render_device_t* rd, mgl_f32_t r, mgl_f32_t g, mgl_f32_t b, mgl_f32_t a);
		void(*clear_depth)(mrl_render_device_t* rd, mgl_f32_t depth);
		void(*clear_stencil)(mrl_render_device_t* rd, mgl_i32_t stencil);
		void(*swap_buffers)(mrl_render_device_t* rd);
		void(*draw_triangles)(mrl_render_device_t* rd, mgl_u64_t offset, mgl_u64_t count);
		void(*draw_triangles_indexed)(mrl_render_device_t* rd, mgl_u64_t offset, mgl_u64_t count);
		void(*draw_triangles_instanced)(mrl_render_device_t* rd, mgl_u64_t offset, mgl_u64_t count, mgl_u64_t instance_count);
		void(*draw_triangles_indexed_instanced)(mrl_render_device_t* rd, mgl_u64_t offset, mgl_u64_t count, mgl_u64_t instance_count);
		void(*set_viewport)(mrl_render_device_t* rd, mgl_i32_t x, mgl_i32_t y, mgl_i32_t w, mgl_i32_t h);

		// ----------- Getters -----------
		const mgl_chr8_t*(*get_type_name)(mrl_render_device_t* rd);
		mgl_i64_t(*get_property_i)(mrl_render_device_t* rd, mgl_enum_t name);
		mgl_f64_t(*get_property_f)(mrl_render_device_t* rd, mgl_enum_t name);
	};

	// ------- Framebuffer functions -------

	/// <summary>
	///		Creates a new framebuffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="fb">Out framebuffer handle</param>
	/// <param name="desc">Description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_framebuffer(mrl_render_device_t* rd, mrl_framebuffer_t** fb, const mrl_framebuffer_desc_t* desc);

	/// <summary>
	///		Destroys a framebuffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="fb">Framebuffer handle</param>
	MRL_API void mrl_destroy_framebuffer(mrl_render_device_t* rd, mrl_framebuffer_t* fb);

	/// <summary>
	///		Sets a framebuffer as active.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="fb">Framebuffer handle</param>
	MRL_API void mrl_set_framebuffer(mrl_render_device_t* rd, mrl_framebuffer_t* fb);

	// ------- Raster state functions -------

	/// <summary>
	///		Creates a new raster state.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="s">Out raster state handle</param>
	/// <param name="desc">Description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_raster_state(mrl_render_device_t* rd, mrl_raster_state_t** s, const mrl_raster_state_desc_t* desc);

	/// <summary>
	///		Destroys a raster state.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="s">Raster state handle</param>
	MRL_API void mrl_destroy_raster_state(mrl_render_device_t* rd, mrl_raster_state_t* s);

	/// <summary>
	///		Sets a raster state as active.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="s">Raster state handle</param>
	MRL_API void mrl_set_raster_state(mrl_render_device_t* rd, mrl_raster_state_t* s);

	// ------- Depth stencil state functions -------

	/// <summary>
	///		Creates a new depth stencil state.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="s">Out depth stencil state handle</param>
	/// <param name="desc">Description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_depth_stencil_state(mrl_render_device_t* rd, mrl_depth_stencil_state_t** s, const mrl_depth_stencil_state_desc_t* desc);

	/// <summary>
	///		Destroys a depth stencil state.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="s">Depth stencil state handle</param>
	MRL_API void mrl_destroy_depth_stencil_state(mrl_render_device_t* rd, mrl_depth_stencil_state_t* s);

	/// <summary>
	///		Sets a depth stencil state as active.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="s">Depth stencil state handle</param>
	MRL_API void mrl_set_depth_stencil_state(mrl_render_device_t* rd, mrl_depth_stencil_state_t* s);

	// ------- Blend state functions -------

	/// <summary>
	///		Creates a new blend state.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="s">Out blend state handle</param>
	/// <param name="desc">Description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_blend_state(mrl_render_device_t* rd, mrl_blend_state_t** s, const mrl_blend_state_desc_t* desc);

	/// <summary>
	///		Destroys a blend state.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="s">Blend state handle</param>
	MRL_API void mrl_destroy_blend_state(mrl_render_device_t* rd, mrl_blend_state_t* s);

	/// <summary>
	///		Sets a blend state as active.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="s">Blend state handle</param>
	MRL_API void mrl_set_blend_state(mrl_render_device_t* rd, mrl_blend_state_t* s);

	// ------- Sampler functions -------

	/// <summary>
	///		Creates a sampler.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="s">Out sampler handle</param>
	/// <param name="desc">Description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_sampler(mrl_render_device_t* rd, mrl_sampler_t** s, const mrl_sampler_desc_t* desc);

	/// <summary>
	///		Destroys a sampler.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="s">Sampler handle</param>
	MRL_API void mrl_destroy_sampler(mrl_render_device_t* rd, mrl_sampler_t* s);

	/// <summary>
	///		Binds a texture 1D to a shader binding point.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="bp">Binding point</param>
	/// <param name="s">Sampler handle</param>
	MRL_API void mrl_bind_sampler(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_sampler_t* s);

	// ------- Texture 1D functions -------

	/// <summary>
	///		Creates a new texture 1D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Out texture 1d handle</param>
	/// <param name="desc">Description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_texture_1d(mrl_render_device_t* rd, mrl_texture_1d_t** tex, const mrl_texture_1d_desc_t* desc);

	/// <summary>
	///		Destroys a texture 1D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Texture 1D handle</param>
	MRL_API void mrl_destroy_texture_1d(mrl_render_device_t* rd, mrl_texture_1d_t* tex);

	/// <summary>
	///		Generates mipmaps for a texture 1D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Texture 1D handle</param>
	MRL_API void mrl_generate_texture_1d_mipmaps(mrl_render_device_t* rd, mrl_texture_1d_t* tex);

	/// <summary>
	///		Binds a texture 1D to a shader binding point.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="bp">Binding point</param>
	/// <param name="tex">Texture 1D handle</param>
	MRL_API void mrl_bind_texture_1d(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_texture_1d_t* tex);

	/// <summary>
	///		Updates a texture 1D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Texture 1D handle</param>
	/// <param name="desc">Update description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_update_texture_1d(mrl_render_device_t* rd, mrl_texture_1d_t* tex, const mrl_texture_1d_update_desc_t* desc);

	// ------- Texture 2D functions -------

	/// <summary>
	///		Creates a new texture 2D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Out texture 2d handle</param>
	/// <param name="desc">Description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_texture_2d(mrl_render_device_t* rd, mrl_texture_2d_t** tex, const mrl_texture_2d_desc_t* desc);

	/// <summary>
	///		Destroys a texture 2D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Texture 2D handle</param>
	MRL_API void mrl_destroy_texture_2d(mrl_render_device_t* rd, mrl_texture_2d_t* tex);

	/// <summary>
	///		Generates mipmaps for a texture 2D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Texture 2D handle</param>
	MRL_API void mrl_generate_texture_2d_mipmaps(mrl_render_device_t* rd, mrl_texture_2d_t* tex);

	/// <summary>
	///		Binds a texture 2D to a shader binding point.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="bp">Binding point</param>
	/// <param name="tex">Texture 2D handle</param>
	MRL_API void mrl_bind_texture_2d(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_texture_2d_t* tex);

	/// <summary>
	///		Updates a texture 2D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Texture 2D handle</param>
	/// <param name="desc">Update description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_update_texture_2d(mrl_render_device_t* rd, mrl_texture_2d_t* tex, const mrl_texture_2d_update_desc_t* desc);

	// ------- Texture 3D functions -------

	/// <summary>
	///		Creates a new texture 3D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Out texture 3d handle</param>
	/// <param name="desc">Description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_texture_3d(mrl_render_device_t* rd, mrl_texture_3d_t** tex, const mrl_texture_3d_desc_t* desc);

	/// <summary>
	///		Destroys a texture 3D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Texture 3D handle</param>
	MRL_API void mrl_destroy_texture_3d(mrl_render_device_t* rd, mrl_texture_3d_t* tex);

	/// <summary>
	///		Generates mipmaps for a texture 3D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Texture 3D handle</param>
	MRL_API void mrl_generate_texture_3d_mipmaps(mrl_render_device_t* rd, mrl_texture_3d_t* tex);

	/// <summary>
	///		Binds a texture 3D to a shader binding point.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="bp">Binding point</param>
	/// <param name="tex">Texture 3D handle</param>
	MRL_API void mrl_bind_texture_3d(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_texture_3d_t* tex);

	/// <summary>
	///		Updates a texture 3D.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="tex">Texture 3D handle</param>
	/// <param name="desc">Update description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_update_texture_3d(mrl_render_device_t* rd, mrl_texture_3d_t* tex, const mrl_texture_3d_update_desc_t* desc);

	// ------- Cube map functions -------

	/// <summary>
	///		Creates a new cube map.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="cb">Out cube map handle</param>
	/// <param name="desc">Description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_cube_map(mrl_render_device_t* rd, mrl_cube_map_t** cb, const mrl_cube_map_desc_t* desc);

	/// <summary>
	///		Destroys a cube map.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="cb">Cube map handle</param>
	MRL_API void mrl_destroy_cube_map(mrl_render_device_t* rd, mrl_cube_map_t* cb);

	/// <summary>
	///		Generates mipmaps for a cube map.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="cb">Cube map handle</param>
	MRL_API void mrl_generate_cube_map_mipmaps(mrl_render_device_t* rd, mrl_cube_map_t* cb);

	/// <summary>
	///		Binds a cube map to a shader binding point.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="bp">Binding point</param>
	/// <param name="cb">Cube map handle</param>
	MRL_API void mrl_bind_cube_map(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_cube_map_t* cb);

	/// <summary>
	///		Updates a cube map.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="cb">Cube map handle</param>
	/// <param name="desc">Update description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_update_cube_map(mrl_render_device_t* rd, mrl_cube_map_t* cb, const mrl_cube_map_update_desc_t* desc);

	// ------- Constant buffer functions -------

	/// <summary>
	///		Creates an constant buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="cb">Out constant buffer handle</param>
	/// <param name="desc">Constant buffer description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_constant_buffer(mrl_render_device_t* rd, mrl_constant_buffer_t** cb, const mrl_constant_buffer_desc_t* desc);

	/// <summary>
	///		Destroys an constant buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="cb">Constant buffer handle</param>
	MRL_API void mrl_destroy_constant_buffer(mrl_render_device_t* rd, mrl_constant_buffer_t* cb);

	/// <summary>
	///		Binds a constant buffer to a shader binding point.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="bp">Shader binding point handle (Set to NULL to unbind previuously bound buffer)</param>
	/// <param name="cb">Constant buffer handle</param>
	MRL_API void mrl_bind_constant_buffer(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_constant_buffer_t* cb);

	/// <summary>
	///		Maps an constant buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="cb">Constant buffer handle</param>
	/// <returns>Pointer to constant buffer data</returns>
	MRL_API void* mrl_map_constant_buffer(mrl_render_device_t* rd, mrl_constant_buffer_t* cb);

	/// <summary>
	///		Unmaps an constant buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="cb">Constant buffer handle</param>
	MRL_API void mrl_unmap_constant_buffer(mrl_render_device_t* rd, mrl_constant_buffer_t* cb);

	/// <summary>
	///		Updates an constant buffer data.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="cb">Constant buffer handle</param>
	/// <param name="offset">Data offset</param>
	/// <param name="size">Data size</param>
	/// <param name="data">Pointer to data</param>
	MRL_API void mrl_update_constant_buffer(mrl_render_device_t* rd, mrl_constant_buffer_t* cb, mgl_u64_t offset, mgl_u64_t size, const void* data);

	/// <summary>
	///		Queries a constant buffer's binding point structure and data members.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="bp">Constant buffer binding point</param>
	/// <param name="cbs">Out constant buffer structure data</param>
	MRL_API void mrl_query_constant_buffer_structure(mrl_render_device_t* rd, mrl_shader_binding_point_t* bp, mrl_constant_buffer_structure_t* cbs);

	// ------- Index buffer functions -------

	/// <summary>
	///		Creates an index buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="ib">Out index buffer handle</param>
	/// <param name="desc">Index buffer description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_index_buffer(mrl_render_device_t* rd, mrl_index_buffer_t** ib, const mrl_index_buffer_desc_t* desc);

	/// <summary>
	///		Destroys an index buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="ib">Index buffer handle</param>
	MRL_API void mrl_destroy_index_buffer(mrl_render_device_t* rd, mrl_index_buffer_t* ib);

	/// <summary>
	///		Sets an index buffer as active.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="ib">Index buffer handle</param>
	MRL_API void mrl_set_index_buffer(mrl_render_device_t* rd, mrl_index_buffer_t* ib);

	/// <summary>
	///		Maps an index buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="ib">Index buffer handle</param>
	/// <returns>Pointer to index buffer data</returns>
	MRL_API void* mrl_map_index_buffer(mrl_render_device_t* rd, mrl_index_buffer_t* ib);

	/// <summary>
	///		Unmaps an index buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="ib">Index buffer handle</param>
	MRL_API void mrl_unmap_index_buffer(mrl_render_device_t* rd, mrl_index_buffer_t* ib);

	/// <summary>
	///		Updates an index buffer data.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="ib">Index buffer handle</param>
	/// <param name="offset">Data offset</param>
	/// <param name="size">Data size</param>
	/// <param name="data">Pointer to data</param>
	MRL_API void mrl_update_index_buffer(mrl_render_device_t* rd, mrl_index_buffer_t* ib, mgl_u64_t offset, mgl_u64_t size, const void* data);

	// ------- Vertex buffer functions -------

	/// <summary>
	///		Creates a vertex buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="vb">Out vertex buffer handle</param>
	/// <param name="desc">Vertex buffer description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_vertex_buffer(mrl_render_device_t* rd, mrl_vertex_buffer_t** vb, const mrl_vertex_buffer_desc_t* desc);

	/// <summary>
	///		Destroys a vertex buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="vb">Vertex buffer handle</param>
	MRL_API void mrl_destroy_vertex_buffer(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb);

	/// <summary>
	///		Maps a vertex buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="vb">vertex buffer handle</param>
	/// <returns>Pointer to index buffer data</returns>
	MRL_API void* mrl_map_vertex_buffer(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb);

	/// <summary>
	///		Unmaps a vertex buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="vb">Index buffer handle</param>
	MRL_API void mrl_unmap_vertex_buffer(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb);

	/// <summary>
	///		Updates a vertex buffer data.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="vb">vertex buffer handle</param>
	/// <param name="offset">Data offset</param>
	/// <param name="size">Data size</param>
	/// <param name="data">Pointer to data</param>
	MRL_API void mrl_update_vertex_buffer(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb, mgl_u64_t offset, mgl_u64_t size, const void* data);

	// ------- Vertex array functions -------

	/// <summary>
	///		Creates a vertex array.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="va">Out vertex array handle</param>
	/// <param name="desc">Vertex array description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_vertex_array(mrl_render_device_t* rd, mrl_vertex_array_t** va, const mrl_vertex_array_desc_t* desc);
	
	/// <summary>
	///		Destroys a vertex array.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="va">Vertex array handle</param>
	MRL_API void mrl_destroy_vertex_array(mrl_render_device_t* rd, mrl_vertex_array_t* va);

	/// <summary>
	///		Sets a vertex array as active.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="va">Vertex array handle</param>
	MRL_API void mrl_set_vertex_array(mrl_render_device_t* rd, mrl_vertex_array_t* va);

	// ------- Shader functions -------

	/// <summary>
	///		Creates a new shader stage.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="stage">Out shader stage handle</param>
	/// <param name="desc">Shader stage description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_shader_stage(mrl_render_device_t* rd, mrl_shader_stage_t** stage, const mrl_shader_stage_desc_t* desc);

	/// <summary>
	///		Destroys a shader stage.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="stage">Shader stage</param>
	MRL_API void mrl_destroy_shader_stage(mrl_render_device_t* rd, mrl_shader_stage_t* stage);

	/// <summary>
	///		Creates a new shader pipeline.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="pipeline">Out shader pipeline handle</param>
	/// <param name="desc">Shader pipeline description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_shader_pipeline(mrl_render_device_t* rd, mrl_shader_pipeline_t** pipeline, const mrl_shader_pipeline_desc_t* desc);

	/// <summary>
	///		Destroys a shader pipeline.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="pipeline">Shader pipeline</param>
	MRL_API void mrl_destroy_shader_pipeline(mrl_render_device_t* rd, mrl_shader_pipeline_t* pipeline);

	/// <summary>
	///		Sets the active shader pipeline.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="pipeline">Shader pipeline</param>
	MRL_API void mrl_set_shader_pipeline(mrl_render_device_t* rd, mrl_shader_pipeline_t* pipeline);

	/// <summary>
	///		Gets a shader binding point.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="pipeline">Pipeline handle</param>
	/// <param name="name">Binding point name</param>
	/// <returns>Binding point handle</returns>
	MRL_API mrl_shader_binding_point_t* mrl_get_shader_binding_point(mrl_render_device_t* rd, mrl_shader_pipeline_t* pipeline, const mgl_chr8_t* name);

	// -------- Draw functions --------

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
	///		Draws triangles.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="offset">First vertex offset</param>
	/// <param name="count">Number of vertexes to render</param>
	MRL_API void mrl_draw_triangles(mrl_render_device_t* rd, mgl_u64_t offset, mgl_u64_t count);

	/// <summary>
	///		Draws triangles using an index buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="offset">First index offset</param>
	/// <param name="count">Number of indexes to render</param>
	MRL_API void mrl_draw_triangles_indexed(mrl_render_device_t* rd, mgl_u64_t offset, mgl_u64_t count);

	/// <summary>
	///		Draws triangles multiple times.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="offset">First vertex offset</param>
	/// <param name="count">Number of vertexes to render</param>
	/// <param name="instance_count">Number of instances to render</param>
	MRL_API void mrl_draw_triangles_instanced(mrl_render_device_t* rd, mgl_u64_t offset, mgl_u64_t count, mgl_u64_t instance_count);

	/// <summary>
	///		Draws triangles using an index buffer multiple times.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="offset">First index offset</param>
	/// <param name="count">Number of indexes to render</param>
	/// <param name="instance_count">Number of instances to render</param>
	MRL_API void mrl_draw_triangles_indexed_instanced(mrl_render_device_t* rd, mgl_u64_t offset, mgl_u64_t count, mgl_u64_t instance_count);

	/// <summary>
	///		Sets the current viewport.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="x">Bottom left viewport corner X coordinate</param>
	/// <param name="y">Bottom left viewport corner Y coordinate</param>
	/// <param name="w">Viewport width</param>
	/// <param name="h">Viewport height</param>
	MRL_API void mrl_set_viewport(mrl_render_device_t* rd, mgl_i32_t x, mgl_i32_t y, mgl_i32_t w, mgl_i32_t h);

	// -------- Getters --------

	/// <summary>
	///		Gets the type name of a render device.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <returns>Render device type name</returns>
	MRL_API const mgl_chr8_t* mrl_get_type_name(mrl_render_device_t* rd);

	/// <summary>
	///		Gets an integer property from a render device.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="name">Property name</param>
	/// <returns>Property value</returns>
	MRL_API mgl_i64_t mrl_get_property_i(mrl_render_device_t* rd, mgl_enum_t name);

	/// <summary>
	///		Gets a floating point property from a render device.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="name">Property name</param>
	/// <returns>Property value</returns>
	MRL_API mgl_f64_t mrl_get_property_f(mrl_render_device_t* rd, mgl_enum_t name);

#ifdef __cplusplus
}
#endif
#endif