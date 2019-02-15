#ifndef MRL_FRAME_H
#define MRL_FRAME_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <mgl/type.h>


#define MRL_MAX_FRAME_MESH_TYPE_COUNT 512

	typedef struct mrl_frame_t mrl_frame_t;
	struct mrl_frame_t
	{
		/// <summary>
		///		Mesh instance data.
		/// </summary>
		struct
		{
			/// <summary>
			///		Effect vertex array group index.
			/// </summary>
			mgl_u32_t index;
			
			/// <summary>
			///		Effect ID.
			/// </summary>
			mgl_u32_t effect;

			/// <summary>
			///		Uniform buffer data that should be sent to the effect shaders.
			/// </summary>
			const void* data;
		} mesh_instances[MRL_MAX_FRAME_MESH_TYPE_COUNT];
	};

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
