#ifndef MRL_MESH_H
#define MRL_MESH_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <mrl/render_device.h>

	typedef struct mrl_mesh_desc_t mrl_mesh_desc_t;

	struct mrl_mesh_desc_t
	{
		/// <summary>
		///		Number of vertex elements used on this mesh.
		/// </summary>
		mgl_u32_t element_count;

		/// <summary>
		///		Number of vertex buffers used on this mesh.
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
	};

	/*
		Usage:

		void func(mrl_manager_t* manager)
		{
			mrl_mesh_desc_t desc;
			desc.element_count = 2; 
			(...)
			
			mgl_u32_t mesh = mrl_create_mesh(manager, &desc);
			mgl_u32_t effect1 = (...);
			mgl_u32_t effect2 = (...);
			mgl_u32_t effect3 = (...);

			mrl_mesh_to_effect(manager, mesh, effect1);
			mrl_mesh_to_effect(manager, mesh, effect2);
			mrl_mesh_to_effect(manager, mesh, effect3);
		}
	*/

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
