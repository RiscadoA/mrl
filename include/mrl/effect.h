#ifndef MRL_EFFECT_H
#define MRL_EFFECT_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <mrl/render_device.h>

#define MRL_MAX_LAYER_NAME_SIZE 32
#define MRL_MAX_EFFECT_NAME_SIZE 64
#define MRL_MAX_EFFECT_PASS_COUNT 4
#define MRL_EFFECT_VERTEX_ARRAY_GROUP_SIZE 32

	typedef struct mrl_effect_pass_t mrl_effect_pass_t;
	typedef struct mrl_effect_pass_desc_t mrl_effect_pass_desc_t;
	typedef struct mrl_effect_t mrl_effect_t;
	typedef struct mrl_effect_desc_t mrl_effect_desc_t;
	typedef struct mrl_effect_vertex_array_group_t mrl_effect_vertex_array_group_t;

	struct mrl_effect_vertex_array_group_t
	{
		/// <summary>
		///		Vertex arrays.
		/// </summary>
		mrl_vertex_array_t* vertex_arrays[MRL_EFFECT_VERTEX_ARRAY_GROUP_SIZE];

		/// <summary>
		///		Next vertex array group.
		/// </summary>
		mrl_effect_vertex_array_group_t* next;
	};

	struct mrl_effect_pass_t
	{
		/// <summary>
		///		Index of the layer where the pass is done.
		/// </summary>
		mgl_u32_t layer;

		/// <summary>
		///		Shader pipeline used on this pass.
		/// </summary>
		mrl_shader_pipeline_t* shader_pipeline;

		/// <summary>
		///		First pass vertex array group.
		/// </summary>
		mrl_effect_vertex_array_group_t va_group;
	};

	struct mrl_effect_pass_desc_t
	{
		/// <summary>
		///		Name of the layer where the pass is done.
		/// </summary>
		const mgl_chr8_t* layer;

		/// <summary>
		///		Shader pipeline used on this pass.
		/// </summary>
		mrl_shader_pipeline_t* shader_pipeline;
	};

#define MRL_DEFAULT_EFFECT_PASS_DESC ((mrl_effect_pass_desc_t) {\
	NULL,\
	NULL,\
	})

	struct mrl_effect_t
	{
		/// <summary>
		///		Effect name.
		/// </summary>
		mgl_chr8_t name[MRL_MAX_EFFECT_NAME_SIZE];

		/// <summary>
		///		Pipeline pass count. 
		/// </summary>
		mrl_effect_pass_t passes[MRL_MAX_EFFECT_PASS_COUNT];

		/// <summary>
		///		Number of passes.
		/// </summary>
		mgl_u32_t pass_count;
	};

	struct mrl_effect_desc_t
	{
		/// <summary>
		///		Effect name.
		/// </summary>
		mgl_chr8_t name[MRL_MAX_EFFECT_NAME_SIZE];

		/// <summary>
		///		Pipeline pass descriptions. 
		/// </summary>
		mrl_effect_pass_desc_t passes[MRL_MAX_EFFECT_PASS_COUNT];

		/// <summary>
		///		Number of passes.
		/// </summary>
		mgl_u32_t pass_count;
	};

#define MRL_DEFAULT_EFFECT_DESC ((mrl_effect_desc_t) {\
	{ 0 },\
	{ MRL_DEFAULT_EFFECT_PASS_DESC },\
	0,\
	})

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
