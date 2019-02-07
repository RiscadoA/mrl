# Vertex Buffers

Vertex buffers are used to store data of one or more vertex elements.

## Functions

- `mrl_vertex_buffer_t* mrl_create_vertex_buffer(mrl_render_device_t* device, mrl_vertex_buffer_desc_t* desc);` - Creates a new vertex buffer.
- `void mrl_destroy_vertex_buffer(mrl_render_device_t* device, mrl_vertex_buffer_t* vb);` - Destroys a vertex buffer.
- `void* mrl_map_vertex_buffer(mrl_render_device_t* device, mrl_vertex_buffer_t* vb);` - Maps a vertex buffer to a writable memory region.
- `void mrl_unmap_vertex_buffer(mrl_render_device_t* device, mrl_vertex_buffer_t* vb);` - Unmaps a vertex buffer.

### `mrl_vertex_buffer_desc_t`

Contains the description used to create a vertex buffer.

Members:

 - `const void* data;`- Vertex buffer initial data (optional, set to NULL to create empty buffer).
 - `mgl_u64_t size;`- Vertex buffer size.
 - `mgl_enum_u32_t usage_mode;`- Vertex buffer usage mode.

#### Usage modes

Vertex buffer valid usage modes:

- `MRL_VERTEX_BUFFER_USAGE_DEFAULT`- Data can be both written to and read from.
- `MRL_VERTEX_BUFFER_USAGE_STATIC`- Data can only be written to on creation (read-only).
- `MRL_VERTEX_BUFFER_USAGE_STREAM`- Data can be both written to and read from (used for vertex buffers which need to be updated very frequently, e.g, every frame).

