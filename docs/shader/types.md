# Shader Data Types

``void`- Represents an empty type (nothing).

## Scalars

- `bool` - Boolean.
- `i32` - 32-bit integer.
- `u32` - 32-bit unsigned integer.
- `f32` - 32-bit floating point.

## Vectors

- `boolv2` - two component boolean vector.
- `boolv3` - three component boolean vector.
- `boolv4` - four component boolean vector.
- `i32v2` - two component 32-bit integer vector.
- `i32v3` - three component 32-bit integer vector.
- `i32v4` - four component 32-bit integer vector.
- `u32v2` - two component 32-bit unsigned integer vector.
- `u32v3` - three component 32-bit unsigned integer vector.
- `u32v4` - four component 32-bit unsigned integer vector.
- `f32v2` - two component 32-bit floating point vector.
- `f32v3` - three component 32-bit floating point vector.
- `f32v4` - four component 32-bit floating point vector.

## Matrices

- `f32m2x2` - 2x2 component 32-bit floating point matrix.
- `f32m2x3` - 2x3 component 32-bit floating point matrix.
- `f32m2x4` - 2x4 component 32-bit floating point matrix.
- `f32m3x2` - 3x2 component 32-bit floating point matrix.
- `f32m3x3` - 3x3 component 32-bit floating point matrix.
- `f32m3x4` - 3x4 component 32-bit floating point matrix.
- `f32m4x2` - 4x2 component 32-bit floating point matrix.
- `f32m4x3` - 4x3 component 32-bit floating point matrix.
- `f32m4x4` - 4x4 component 32-bit floating point matrix.

## Opaque types

Opaque types represent an external object which the shader references in some fashion. Opaque types don't have actual values, and as such, they can only be used as paremeters to functions. These functions return/modify the actual data.

### Samplers

Samplers are used to sample data from textures.

- `i32sampler1d` - 1D 32-bit integer component texture.
- `u32sampler1d` - 1D 32-bit unsigned integer component texture.
- `f32sampler1d` - 1D 32-bit floating point component texture.
- `i32sampler2d` - 2D 32-bit integer component texture.
- `u32sampler2d` - 2D 32-bit unsigned integer component texture.
- `f32sampler2d` - 2D 32-bit floating point component texture.
- `i32sampler3d` - 3D 32-bit integer component texture.
- `u32sampler3d` - 3D 32-bit unsigned integer component texture.
- `f32sampler3d` - 3D 32-bit floating point component texture.
- `i32sampler_cube` - Cube 32-bit integer component map.
- `u32sampler_cube` - Cube 32-bit unsigned integer component map.
- `f32sampler_cube` - Cube 32-bit floating point component map.

#### Depth samplers

Depth samplers are used to compare a value from a depth-stencil texture and another value.

- `dsampler1d` - 1D depth-stencil texture.
- `dsampler2d` - 2D depth-stencil texture.
- `dsampler3d` - 3D depth-stencil texture.
- `dsampler_cube` - Cube depth-stencil map.

## Uniform Buffers

Uniform buffers are used to store scalars, vectors and matrices in a single bindable buffer.
Example:

```
[desc = "Uniform buffer that stores the material properties"]
[name = "Material"]
uniform my_material : 0
{
	(name = "Specular color")
    f32v4 specular : 0;
    (name = "Albedo")
    f32v4 albedo : 1;
}
```