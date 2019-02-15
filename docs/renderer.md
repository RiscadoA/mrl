# Renderer

A renderer is in charge of:

- Rendering a frame to the screen.
- Managing effects and materials.

## Meshes

Meshes shall be created through a mesh description which should be sent to the effects system.



## Effects System

The effects system is in charge of creating and destroying effects and materials.

An effect contains the information necessary to render a mesh.

There are two essential two data types in the effect system:

- `mrl_effect_t` - contains the multiple shader pipelines, one per pass, and other rendering info.
- `mrl_material_t` - contains the data that shall be sent to the effect shaders (textures, etc).

### Effects

Effects are composed of one or more passes. Each pass is assigned to a layer (which is specific to a type of renderer). A pass contains a shader pipeline, a rasterizer state, a depth stencil state and a blend state.

Effect structure layout:

```
struct mrl_effect_pass_t
{
	mgl_chr8_t layer[MRL_MAX_LAYER_NAME_SIZE]

    mrl_raster_state_t* raster_state;
	mrl_depth_stencil_state_t* depth_stencil_state;
	mrl_blend_state_t* blend_state;
	mrl_shader_pipeline_t* pipeline;
}

struct mrl_effect_t
{
	mgl_chr8_t name[MRL_MAX_EFFECT_NAME_SIZE];

    mrl_material_pass_t passes[MRL_MAX_EFFECT_PASS_COUNT];
    mgl_u32_t pass_count;
}
```

