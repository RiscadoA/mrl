#ifndef MRL_RENDER_DEVICE_H
#define MRL_RENDER_DEVICE_H
#ifdef __cplusplus
extern "C" {
#endif 

#include <mrl/error.h>

	typedef void(*mrl_render_device_hint_warning_callback_t)(mrl_error_t error, const mgl_chr8_t* msg);
	typedef void(*mrl_render_device_hint_error_callback_t)(mrl_error_t error, const mgl_chr8_t* msg);

	typedef struct mrl_hint_t mrl_hint_t;
	typedef struct mrl_index_buffer_desc_t mrl_index_buffer_desc_t;
	typedef struct mrl_vertex_buffer_desc_t mrl_vertex_buffer_desc_t;
	typedef struct mrl_vertex_element_t mrl_vertex_element_t;
	typedef struct mrl_vertex_array_desc_t mrl_vertex_array_desc_t;
	typedef struct mrl_shader_stage_desc_t mrl_shader_stage_desc_t;
	typedef struct mrl_shader_pipeline_desc_t mrl_shader_pipeline_desc_t;
	typedef struct mrl_render_device_desc_t mrl_render_device_desc_t;

	typedef void mrl_index_buffer_t;
	typedef void mrl_vertex_buffer_t;
	typedef void mrl_vertex_array_t;
	typedef void mrl_shader_stage_t;
	typedef void mrl_shader_pipeline_t;

	// ----- Hints -----

	enum
	{
		/// <summary>
		///		Hints that the render device should call a function when a warning is issued.
		///		The pointer to the warning callback function pointer is stored on the 'data' member of the hint.
		///		The function pointer is of the type mrl_render_device_hint_warning_callback_t.
		/// </summary>
		MRL_HINT_WARNING_CALLBACK,

		/// <summary>
		///		Hints that the render device should call a function when an error occurs.
		///		The pointer to the error callback function pointer is stored on the 'data' member of the hint.
		/// The function pointer is of the type mrl_render_device_hint_error_callback_t.
		/// </summary>
		MRL_HINT_ERROR_CALLBACK,
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
	512,\
	512,\
	256,\
	256,\
	256,\
	NULL,\
})

	typedef struct mrl_render_device_t mrl_render_device_t;
	struct mrl_render_device_t
	{
		// ------- Index buffer functions -------
		mrl_error_t(*create_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t** ib, mrl_index_buffer_desc_t* desc);
		void(*destroy_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t* ib);
		void(*set_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t* ib);
		void*(*map_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t* ib);
		void(*unmap_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t* ib);
		void(*update_index_buffer)(mrl_render_device_t* rd, mrl_index_buffer_t* ib, mgl_u64_t offset, mgl_u64_t size, const void* data);

		// ------- Vertex buffer functions -------
		mrl_error_t(*create_vertex_buffer)(mrl_render_device_t* rd, mrl_vertex_buffer_t** vb, mrl_vertex_buffer_desc_t* desc);
		void(*destroy_vertex_buffer)(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb);
		void*(*map_vertex_buffer)(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb);
		void(*unmap_vertex_buffer)(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb);
		void(*update_vertex_buffer)(mrl_render_device_t* rd, mrl_vertex_buffer_t* vb, mgl_u64_t offset, mgl_u64_t size, const void* data);

		// ------- Vertex array functions -------
		mrl_error_t(*create_vertex_array)(mrl_render_device_t* rd, mrl_vertex_array_t** va, mrl_vertex_array_desc_t* desc);
		void(*destroy_vertex_array)(mrl_render_device_t* rd, mrl_vertex_array_t* va);
		void(*set_vertex_array)(mrl_render_device_t* rd, mrl_vertex_array_t* va);

		// ------- Shader functions -------
		mrl_error_t(*create_shader_stage)(mrl_render_device_t* rd, mrl_shader_stage_t** stage, mrl_shader_stage_desc_t* desc);
		void(*destroy_shader_stage)(mrl_render_device_t* rd, mrl_shader_stage_t* stage);
		mrl_error_t(*create_shader_pipeline)(mrl_render_device_t* rd, mrl_shader_pipeline_t** pipeline, mrl_shader_pipeline_desc_t* desc);
		void(*destroy_shader_pipeline)(mrl_render_device_t* rd, mrl_shader_pipeline_t* pipeline);
		void(*set_shader_pipeline)(mrl_render_device_t* rd, mrl_shader_pipeline_t* pipeline);

		// -------- Draw functions --------
		void(*clear_color)(mrl_render_device_t* rd, mgl_f32_t r, mgl_f32_t g, mgl_f32_t b, mgl_f32_t a);
		void(*clear_depth)(mrl_render_device_t* rd, mgl_f32_t depth);
		void(*clear_stencil)(mrl_render_device_t* rd, mgl_i32_t stencil);
		void(*swap_buffers)(mrl_render_device_t* rd);
		void(*draw_triangles)(mrl_render_device_t* rd, mgl_u64_t offset, mgl_u64_t count);
		void(*draw_triangles_indexed)(mrl_render_device_t* rd, mgl_u64_t offset, mgl_u64_t count);

		// ----------- Getters -----------
		const mgl_chr8_t*(*get_type_name)(mrl_render_device_t* rd);
	};

	// ------- Index buffer functions -------

	/// <summary>
	///		Creates an index buffer.
	/// </summary>
	/// <param name="rd">Render device</param>
	/// <param name="ib">Out index buffer handle</param>
	/// <param name="desc">Index buffer description</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_create_index_buffer(mrl_render_device_t* rd, mrl_index_buffer_t** ib, mrl_index_buffer_desc_t* desc);

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
	MRL_API mrl_error_t mrl_create_vertex_buffer(mrl_render_device_t* rd, mrl_vertex_buffer_t** vb, mrl_vertex_buffer_desc_t* desc);

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
	MRL_API mrl_error_t mrl_create_vertex_array(mrl_render_device_t* rd, mrl_vertex_array_t** va, mrl_vertex_array_desc_t* desc);
	
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
	MRL_API mrl_error_t mrl_create_shader_stage(mrl_render_device_t* rd, mrl_shader_stage_t** stage, mrl_shader_stage_desc_t* desc);

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
	MRL_API mrl_error_t mrl_create_shader_pipeline(mrl_render_device_t* rd, mrl_shader_pipeline_t** pipeline, mrl_shader_pipeline_desc_t* desc);

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

	// -------- Getters --------

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