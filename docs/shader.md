# Shaders

## Pipelines

Shader pipelines are composed of multiple shader stages.

Pipeline parameters can be inspected and set to new values.

## Stages

### Creation

Shader stages are created through the render device by multiple source types. The only supported source type right now is `MRL_SHADER_SRC_GLSL `, but it is planned to add a custom portable shader language.

The function used to create a shader is `mrl_create_shader_stage(mrl_render_device_t* device, mgl_enum_t stage, mgl_enum_t src_type, const void* src)`.

- `device` - Specifies the render device used to create the shader.  
- `stage` - Specifies the shader stage.
- `src_type` - Specifies the shader source type (`MRL_SHADER_SRC_GLSL`).
- `src` - Stores the source code used to create the shader.

### Types

The following shader stage types are supported:

- `MRL_SHADER_STAGE_VERTEX` - Vertex shader stage.
- `MRL_SHADER_STAGE_PIXEL` - Pixel shader stage.