#include <mrl/shader/lexer.h>
#include <mgl/stream/stream.h>
#include <mgl/memory/allocator.h>
#include <mgl/entry.h>

#define MULTI_LINE_STRING(...) #__VA_ARGS__

int main(int argc, char** argv)
{
	if (mgl_init() != MGL_ERROR_NONE)
		return -1;

	const mgl_chr8_t* src = MULTI_LINE_STRING(
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
	);


	mrl_shader_token_t token_buffer[4096];
	mgl_u64_t token_count;
	mgl_chr8_t error_buf[2048];

	mrl_shader_lexer_params_t params;
	params.allocator = mgl_standard_allocator;
	params.error_buffer = &error_buf;
	params.error_buffer_size = sizeof(error_buf);
	params.max_token_count = sizeof(token_buffer) / sizeof(*token_buffer);
	params.out_tokens = token_buffer;
	params.out_token_count = &token_count;
	params.src = src;

	// Run lexer
	if (!mrl_run_shader_lexer(&params))
	{
		mgl_print(mgl_stdout_stream, u8"Lexer failed:\n");
		mgl_print(mgl_stdout_stream, error_buf);
		mgl_read_chars_until(mgl_stdin_stream, NULL, 0, NULL, u8"\n");
		return -2;
	}

	for (mgl_u64_t i = 0; i < token_count; ++i)
	{
		mgl_print(mgl_stdout_stream, token_buffer[i].tinfo->name);

		if (token_buffer[i].attribute != NULL)
		{
			mgl_print(mgl_stdout_stream, u8" (\"");
			mgl_print(mgl_stdout_stream, token_buffer[i].attribute);
			mgl_print(mgl_stdout_stream, u8"\")");
		}

		mgl_put_char(mgl_stdout_stream, '\n');
	}

	mrl_deallocate_shader_lexer_tokens(token_buffer, token_count, params.allocator);

	mgl_read_chars_until(mgl_stdin_stream, NULL, 0, NULL, u8"\n");

	mgl_terminate();
	return 0;
}
