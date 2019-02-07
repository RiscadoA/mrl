# Drawing

## Functions

- `void mrl_clear_color(mrl_render_device_t* device, mgl_f32_t r, mgl_f32_t g, mgl_f32_t b, mgl_f32_t a);` - Clears the current framebuffer color buffer.
- `void mrl_clear_depth((mrl_render_device_t* device, mgl_f32_t depth);`- Clears the current framebuffer depth buffer.
- `void mrl_clear_stencil((mrl_render_device_t* device, mgl_i32_t stencil);`- Clears the current framebuffer stencil buffer.
- `void mrl_draw_triangles((mrl_render_device_t* device, mgl_u64_t offset, mgl_u64_t count);`- Draws the triangles on the current vertex array.
- `void mrl_draw_triangles_indexed((mrl_render_device_t* device, mgl_u64_t offset, mgl_u64_t count);`- Draws the triangles using indices on the current vertex array and index buffer.
- `void mrl_swap_buffers(mrl_render_device_t* device);`- Swaps the front and back buffers.

