# Shader Pipelines

## Examples

```
version 1.0;

vertex
{
    input
    {
        f32v3 position : 0;
        f32v3 normal : 1;
        f32v2 uv : 2;
    }

    output
    {
    	f32v4 position : SCREEN_POSITION;
        f32v4 world_normal : 0;
        f32v3 world_position : 1;
        f32v2 uv : 2;
    }
	
	[name = "Model matrix"]
	uniform f32m4x4 model : 0;
	[name = "View matrix"]
	uniform f32m4x4 view : 1;
	[name = "Projection matrix"]
	uniform f32m4x4 projection : 2;

    void main()
    {
        output.world_position = (model * f32v4(input.position, 1.0f)).xyz;
        output.world_normal = (model * f32v4(input.normal, 1.0f)).xyz;
        output.position = projection * view * output.world_position;
        output.uv = input.uv;
    }
}

pixel
{
    input
    {
        f32v3 world_normal : 0;
        f32v3 world_position : 1;
        f32v2 uv : 2;
    }

    output
    {
        f32v4 pos_spec : 0;
        f32v4 normals : 1;
        f32v4 albedo : 2;
    }

	[desc = "Material uniform buffer"]
	[name = "Material"]
	uniform material : 0; 
	{
        f32v4 color : 0;
		f32 specular : 1;
	}
	
    sampler2d diffuse : DIFFUSE;
	
    void main()
    {
        output.pos_spec.xyz = input.world_position;
        output.pos_spec.w = material.specular;
        output.normals.xyz = input.world_normal;
        output.albedo = material.color * diffuse.sample(input.uv);
    }
}
```