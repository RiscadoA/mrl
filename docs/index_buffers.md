# Index Buffers

Index buffers are used to store vertex indices for indexed drawing.

## Functions

- `mrl_index_buffer_t* mrl_create_index_buffer(mrl_render_device_t* device, mrl_index_buffer_desc_t* desc);` - Creates a new index buffer.
- `void mrl_destroy_index_buffer(mrl_render_device_t* device, mrl_index_buffer_t* ib);` - Destroys a index buffer.
- `void mrl_set_index_buffer(mrl_render_device_t* device, mrl_index_buffer_t* ib);` - Sets an index buffer as the active index buffer for indexed drawing operations. `ib` can be set to NULL to unset the buffer.
- `void* mrl_map_index_buffer(mrl_render_device_t* device, mrl_index_buffer_t* ib);` - Maps an index buffer to a writable memory region.
- `void mrl_unmap_index_buffer(mrl_render_device_t* device, mrl_index_buffer_t* ib);` - Unmaps an index buffer.

### `mrl_index_buffer_desc_t`

Contains the description used to create an index buffer.

Members:

- `const void* data;`- Index buffer initial data (optional, set to NULL to create empty buffer).
- `mgl_u64_t size;`- Index buffer size.
- `mgl_enum_u32_t usage_mode;`- Index buffer usage mode.
- `mgl_enum_u32_t format;`- Index format.

#### Usage modes

Vertex buffer valid usage modes:

- `MRL_INDEX_BUFFER_USAGE_DEFAULT`- Data can be both written to and read from.

- `MRL_INDEX_BUFFER_USAGE_STATIC`- Data can only be written to on creation (read-only).

- `MRL_INDEX_BUFFER_USAGE_STREAM`- Data can be both written to and read from (used for index buffers which need to be updated very frequently, e.g, every frame).

#### Index formats

Valid index formats:

- `MRL_INDEX_BUFFER_U16`- Each index is an unsigned 16-bit integer.
- `MRL_INDEX_BUFFER_U32`- Each index is an unsigned 32-bit integer.


