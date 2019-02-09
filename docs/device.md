# Device

The graphics backend device is responsible for creating and managing all GPU objects.

The following object types are supported:

- [x] Vertex arrays.
- [x] Vertex buffers.
- [x] Index buffers.
- [x] Constant buffers.
- [ ] Samplers.
- [ ] 1D Textures.
- [ ] 2D Textures.
- [ ] 3D Textures.
- [ ] Cubemaps.
- [ ] Framebuffers.
- [ ] Blend states.
- [ ] Depth stencil states.
- [ ] Rasterizer states.
- [x] Shader stages.
- [x] Shader pipelines.

## Underlying APIs

A graphics backend can be swapped by another one which is implemented by another API.

The planned support APIs are (in order of priority):

- [x] OpenGL 3.3.
- [ ] DirectX 11.
- [ ] Vulkan 1.0.
- [ ] Metal.

