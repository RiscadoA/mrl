# Vertex Arrays

Vertex arrays join multiple vertex buffers and multiple vertex elements to specify how should the data be passed to a shader.

## Functions

- `mrl_vertex_array_t* mrl_create_vertex_array(mrl_render_device_t* device, mrl_vertex_array_desc_t* desc);` - Creates a new vertex array.
- `void mrl_destroy_vertex_array(mrl_render_device_t* device, mrl_vertex_array_t* va);` - Destroys a vertex array.
- `void mrl_set_vertex_array(mrl_render_device_t* device, mrl_vertex_array_t* va);` - Sets a vertex array as the active vertex array for drawing operations. `va` can be set to NULL to unset the vertex array.

### `mrl_vertex_array_desc_t`

Contains the description used to create a vertex array.

Members:

 - `mgl_u32_t vertex_element_count;`- Number of vertex elements used on this vertex array.
 - `mgl_u32_t vertex_buffer_count;`- Number of vertex buffers used on this vertex array.
 - `mrl_vertex_element_t vertex_elements[MRL_MAX_VERTEX_ARRAY_ELEMENT_COUNT];`- Vertex elements used on this vertex array.
 - `mrl_vertex_buffer_t* vertex_buffers[MRL_MAX_VERTEX_ARRAY_BUFFER_COUNT]`; - Vertex buffers used on this vertex array.
### `mrl_vertex_element_t`

Contains data about a single vertex element.

Members:

- `mgl_u64_t stride;`- Stride between each element in memory.
- `mgl_u64_t offset;`- Offset of the first element in the buffer.
- `mgl_enum_u32_t type;`- Vertex element type.
- `mgl_u32_t buffer_index;`- Vertex buffer index.
- `mgl_u32_t location;`- Vertex element attribute location.
- `mgl_u8_t size;`- Vertex element size (valid values: 1; 2; 3; 4).

#### Vertex element types

- `MRL_VERTEX_ELEMENT_I8` - Each component of the vertex element is a 8-bit integer.
- `MRL_VERTEX_ELEMENT_I16` - Each component of the vertex element is a 16-bit integer.
- `MRL_VERTEX_ELEMENT_I32` - Each component of the vertex element is a 32-bit integer.
- `MRL_VERTEX_ELEMENT_U8` - Each component of the vertex element is a 8-bit integer.
- `MRL_VERTEX_ELEMENT_U16` - Each component of the vertex element is an unsigned 16-bit integer.
- `MRL_VERTEX_ELEMENT_U32` - Each component of the vertex element is an unsigned 32-bit integer.
- `MRL_VERTEX_ELEMENT_N8` - Each component of the vertex element is a normalized 8-bit integer.
- `MRL_VERTEX_ELEMENT_N16` - Each component of the vertex element is a normalized 16-bit integer.
- `MRL_VERTEX_ELEMENT_NU8` - Each component of the vertex element is a normalized unsigned 8-bit integer.
- `MRL_VERTEX_ELEMENT_NU16` - Each component of the vertex element is a normalized unsigned 16-bit integer.
- `MRL_VERTEX_ELEMENT_F32` - Each component of the vertex element is a 32-bit float.