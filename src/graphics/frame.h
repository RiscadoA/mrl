#ifndef MGE_GRAPHICS_FRAME_H
#define MGE_GRAPHICS_FRAME_H

#include <mgl/type.h>
#include <mgl/math/matrix4x4.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	// Frame static data layout (aligned 16-bytes):
	// [type] (4 bytes)
	// [mesh index] (4 bytes)
	// [material index] (4 bytes)
	// [element count] (4 bytes)
	// [transform matrix 1] (16 bytes)
	// [transform matrix 2] (16 bytes)
	// [transform matrix 3] (16 bytes)
	// [transform matrix n] (16 bytes)
	// [mesh index] (4 bytes)
	// [material index] (4 bytes)
	// [element count] (4 bytes)
	// [padding] (4 bytes)
	// [transform matrix 1] (16 bytes)
	// [transform matrix 2] (16 bytes)
	// [transform matrix 3] (16 bytes)
	// [transform matrix n] (16 bytes)
	// (...)

	typedef struct
	{
		mgl_u32_t type; // Type info
		mgl_u32_t mesh_index;
		mgl_u32_t material_index;
		mgl_u32_t element_count;
	} mge_graphics_frame_static_element_header_t;

	enum
	{
		MGE_GRAPHICS_FRAME_ELEMENT_TYPE_END		= 0x00,
		MGE_GRAPHICS_FRAME_ELEMENT_TYPE_DEFAULT	= 0x01,
	}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MGE_GRAPHICS_FRAME_H
