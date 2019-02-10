#include <mrl/ogl_330_render_device.h>
#include <mgl/input/windows_window.h>
#include <mgl/stream/stream.h>
#include <mgl/string/manipulation.h>
#include <mgl/memory/allocator.h>
#include <mgl/entry.h>
#include <mgl/time/sleep.h>

#include <stddef.h>

// Set texture 2
// Draw into framebuffer 1 
// Set texture 1
// Draw into framebuffer 2
// Draw into screen framebuffer
// Set texture 2
// Draw into framebuffer 1 
// (...)

struct
{
	mgl_windows_window_t window;
	mgl_bool_t running;
	mrl_render_device_t* rd;

	struct
	{
		mrl_shader_stage_t* vertex;
		mrl_shader_stage_t* pixel;
		mrl_shader_pipeline_t* pipeline;
		mrl_shader_binding_point_t* bp;
	} shader;

	mrl_framebuffer_t* fb1;
	mrl_framebuffer_t* fb2;

	mrl_sampler_t* sampler;
	mrl_texture_2d_t* start_tex;
	mrl_texture_2d_t* tex1;
	mrl_texture_2d_t* tex2;
	mrl_index_buffer_t* ibo;
	mrl_vertex_buffer_t* vbo;
	mrl_vertex_array_t* vao;
} app;

typedef struct
{
	mgl_f32_t x, y;
	mgl_f32_t u, v;
} my_vertex_t;

void handle_error(mrl_error_t error, const mgl_chr8_t* msg);

void on_window_close(mgl_action_t* action, mgl_enum_t status);

void render_device_error_callback(mrl_error_t error, const mgl_chr8_t* msg)
{
	handle_error(error, msg);
}

void load(void)
{
	// Init render device
	{
		mrl_render_device_hint_error_callback_t error_callback = &render_device_error_callback;

		mrl_hint_t hint = MRL_DEFAULT_HINT;
		hint.type = MRL_HINT_RENDER_DEVICE_ERROR_CALLBACK;
		hint.data = &error_callback;

		mrl_render_device_desc_t desc = MRL_DEFAULT_RENDER_DEVICE_DESC;
		desc.allocator = mgl_standard_allocator;
		desc.window = &app.window;
		desc.vsync_mode = MRL_VSYNC_ADAPTIVE;
		desc.hints = &hint;

		handle_error(mrl_init_ogl_330_render_device(&desc, &app.rd), u8"mrl_init_ogl_330_render_device() failed");
	}

	// Init vertex shader stage
	{
		mrl_shader_stage_desc_t desc = MRL_DEFAULT_SHADER_STAGE_DESC;
		desc.stage = MRL_SHADER_STAGE_VERTEX;
		desc.src_type = MRL_SHADER_SOURCE_GLSL;
		desc.src =
			"#version 330 core\n"

			"in vec2 position;"
			"in vec2 uvs;"

			"out vec2 frag_uvs;"

			"void main() {"
			"	frag_uvs = uvs;"
			"	gl_Position = vec4(position, 0.0, 1.0);"
			"}";

		handle_error(mrl_create_shader_stage(app.rd, &app.shader.vertex, &desc), u8"Failed to create vertex shader stage");
	}

	// Init pixel shader stage
	{
		mrl_shader_stage_desc_t desc = MRL_DEFAULT_SHADER_STAGE_DESC;
		desc.stage = MRL_SHADER_STAGE_PIXEL;
		desc.src_type = MRL_SHADER_SOURCE_GLSL;
		desc.src =
			"#version 330 core\n"
			"in vec2 frag_uvs;"
			"out vec4 color;"
			"uniform sampler2D tex;"
			"void main() {"
			"	color = texture(tex, frag_uvs);"
			"}";

		handle_error(mrl_create_shader_stage(app.rd, &app.shader.pixel, &desc), u8"Failed to create pixel shader stage");
	}

	// Init shader pipeline
	{
		mrl_shader_pipeline_desc_t desc = MRL_DEFAULT_SHADER_PIPELINE_DESC;
		desc.vertex = app.shader.vertex;
		desc.pixel = app.shader.pixel;

		handle_error(mrl_create_shader_pipeline(app.rd, &app.shader.pipeline, &desc), u8"Failed to create shader pipeline");
	}

	// Get BP
	app.shader.bp = mrl_get_shader_binding_point(app.rd, app.shader.pipeline, u8"tex");

	// Create sampler
	{
		mrl_sampler_desc_t desc = MRL_DEFAULT_SAMPLER_DESC;

		desc.min_filter = MRL_SAMPLER_FILTER_LINEAR;
		desc.mag_filter = MRL_SAMPLER_FILTER_LINEAR;
		desc.max_anisotropy = (mgl_u32_t)mrl_get_property_i(app.rd, MRL_PROPERTY_MAX_ANISTROPY);

		desc.address_u = MRL_SAMPLER_ADDRESS_MIRROR;
		desc.address_v = MRL_SAMPLER_ADDRESS_MIRROR;

		handle_error(mrl_create_sampler(app.rd, &app.sampler, &desc), u8"Failed to create sampler");
	}

	// Create textures
	{
		mrl_texture_2d_desc_t desc = MRL_DEFAULT_TEXTURE_2D_DESC;

		desc.usage = MRL_TEXTURE_USAGE_RENDER_TARGET;
		desc.format = MRL_TEXTURE_FORMAT_RGBA32_F;
		desc.width = 800;
		desc.height = 600;

		handle_error(mrl_create_texture_2d(app.rd, &app.tex1, &desc), u8"Failed to create texture 1");
		handle_error(mrl_create_texture_2d(app.rd, &app.tex2, &desc), u8"Failed to create texture 2");

		desc = MRL_DEFAULT_TEXTURE_2D_DESC;

		mgl_u8_t data[] =
		{
			255, 0, 0, 255,
			0, 255, 0, 255,
			0, 0, 255, 255,
			255, 0, 255, 255,
		};

		desc.format = MRL_TEXTURE_FORMAT_RGBA8_UN;
		desc.width = 2;
		desc.height = 2;
		desc.data[0] = data;

		handle_error(mrl_create_texture_2d(app.rd, &app.start_tex, &desc), u8"Failed to create starting texture");
	}

	// Create framebuffers
	{
		mrl_framebuffer_desc_t desc = MRL_DEFAULT_FRAMEBUFFER_DESC;
		desc.target_count = 1;

		desc.targets[0].type = MRL_RENDER_TARGET_TYPE_TEXTURE_2D;
		desc.targets[0].tex_2d.handle = app.tex1;
		handle_error(mrl_create_framebuffer(app.rd, &app.fb1, &desc), u8"Failed to create framebuffer 1");

		desc.targets[0].type = MRL_RENDER_TARGET_TYPE_TEXTURE_2D;
		desc.targets[0].tex_2d.handle = app.tex2;
		handle_error(mrl_create_framebuffer(app.rd, &app.fb2, &desc), u8"Failed to create framebuffer 2");
	}

	// Init IBO
	{
		mrl_index_buffer_desc_t desc = MRL_DEFAULT_INDEX_BUFFER_DESC;

		mgl_u16_t data[6] =
		{
			0, 1, 2,
			0, 3, 2,
		};

		desc.data = data;
		desc.size = sizeof(data);
		desc.format = MRL_INDEX_BUFFER_FORMAT_U16;

		handle_error(mrl_create_index_buffer(app.rd, &app.ibo, &desc), u8"Failed to create index buffer");
	}

	// Init VBO
	{
		my_vertex_t data[] =
		{
			{ -0.9f, -0.9f, 0.0f, 0.0f },
			{ +0.9f, -0.9f, 1.0f, 0.0f },
			{ +0.9f, +0.9f, 1.0f, 1.0f },
			{ -0.9f, +0.9f, 0.0f, 1.0f },
		};

		mrl_vertex_buffer_desc_t desc = MRL_DEFAULT_VERTEX_BUFFER_DESC;
		desc.data = data;
		desc.size = sizeof(my_vertex_t) * 4;
		desc.usage = MRL_VERTEX_BUFFER_USAGE_STATIC;

		handle_error(mrl_create_vertex_buffer(app.rd, &app.vbo, &desc), u8"Failed to create vertex buffer");
	}

	// Init VAO
	{
		mrl_vertex_array_desc_t desc = MRL_DEFAULT_VERTEX_ARRAY_DESC;

		// Set elements
		desc.element_count = 2;

		desc.elements[0] = MRL_DEFAULT_VERTEX_ELEMENT;
		mgl_str_copy(u8"position", desc.elements[0].name, MRL_MAX_VERTEX_ELEMENT_NAME_SIZE);
		desc.elements[0].size = 2;
		desc.elements[0].type = MRL_VERTEX_ELEMENT_TYPE_F32;
		desc.elements[0].buffer.index = 0;
		desc.elements[0].buffer.offset = offsetof(my_vertex_t, x);
		desc.elements[0].buffer.stride = sizeof(my_vertex_t);

		desc.elements[1] = MRL_DEFAULT_VERTEX_ELEMENT;
		mgl_str_copy(u8"uvs", desc.elements[1].name, MRL_MAX_VERTEX_ELEMENT_NAME_SIZE);
		desc.elements[1].size = 2;
		desc.elements[1].type = MRL_VERTEX_ELEMENT_TYPE_F32;
		desc.elements[1].buffer.index = 0;
		desc.elements[1].buffer.offset = offsetof(my_vertex_t, u);
		desc.elements[1].buffer.stride = sizeof(my_vertex_t);

		// Set VBOs
		desc.buffer_count = 1;
		desc.buffers[0] = app.vbo;

		// Set shader pipeline
		desc.shader_pipeline = app.shader.pipeline;

		handle_error(mrl_create_vertex_array(app.rd, &app.vao, &desc), u8"Failed to create vertex array");
	}
}

void unload(void)
{
	// Terminate vertex array
	mrl_destroy_vertex_array(app.rd, app.vao);

	// Terminate vertex buffer
	mrl_destroy_vertex_buffer(app.rd, app.vbo);

	// Terminate index buffer
	mrl_destroy_index_buffer(app.rd, app.ibo);

	// Terminate framebuffers
	mrl_destroy_framebuffer(app.rd, app.fb2);
	mrl_destroy_framebuffer(app.rd, app.fb1);

	// Terminate textures
	mrl_destroy_texture_2d(app.rd, app.start_tex);
	mrl_destroy_texture_2d(app.rd, app.tex2);
	mrl_destroy_texture_2d(app.rd, app.tex1);

	// Terminate sampler
	mrl_destroy_sampler(app.rd, app.sampler);

	// Terminate shader pipeline
	mrl_destroy_shader_stage(app.rd, app.shader.pipeline);

	// Terminate vertex shader stage
	mrl_destroy_shader_stage(app.rd, app.shader.vertex);

	// Terminate pixel shader stage
	mrl_destroy_shader_stage(app.rd, app.shader.pixel);

	// Terminate device
	mrl_terminate_ogl_330_render_device(app.rd);
}

int main(int argc, char** argv)
{
	handle_error(mrl_make_mgl_error(mgl_init()), u8"mgl_init() failed");

	app.running = MGL_TRUE;

	// Init input manager
	mgl_input_manager_t input_manager;
	mgl_init_input_manager(&input_manager);

	// Open window
	{
		mgl_windows_window_settings_t settings;
		settings.width = 800;
		settings.height = 600;
		settings.mode = MGL_WINDOW_MODE_WINDOWED;
		settings.title = u8"OpenGL 3.3 Render Device Usage Example";
		settings.input_manager = &input_manager;
		handle_error(mgl_open_windows_window(&app.window, &settings), u8"mgl_open_windows_window() failed");

		// Add on window close callback
		handle_error(
			mrl_make_mgl_error(
				mgl_add_action_callback(
					&input_manager,
					mgl_get_window_action(&app.window, MGL_WINDOW_CLOSE),
					&on_window_close
				)
			),
			u8"mgl_add_action_callback() failed"
		);
	}

	load();

	// Set pipeline, sampler, vertex array, index buffer and viewport
	mrl_set_shader_pipeline(app.rd, app.shader.pipeline);
	mrl_bind_sampler(app.rd, app.shader.bp, app.sampler);
	mrl_set_index_buffer(app.rd, app.ibo);
	mrl_set_vertex_array(app.rd, app.vao);
	mrl_set_viewport(app.rd, 0, 0, 800, 600);

	// Initial draw to texture 2
	mrl_set_framebuffer(app.rd, app.fb2);
	mrl_bind_texture_2d(app.rd, app.shader.bp, app.start_tex);
	mrl_draw_triangles_indexed(app.rd, 0, 6);

	// Main loop
	while (app.running)
	{
		mgl_poll_window_events(&app.window);
		
		// Draw to texture 1
		mrl_set_framebuffer(app.rd, app.fb1);
		mrl_bind_texture_2d(app.rd, app.shader.bp, app.tex2);
		mrl_draw_triangles_indexed(app.rd, 0, 6);

		// Draw to texture 2
		mrl_set_framebuffer(app.rd, app.fb2);
		mrl_bind_texture_2d(app.rd, app.shader.bp, app.tex1);
		mrl_draw_triangles_indexed(app.rd, 0, 6);

		// Draw to screen
		mrl_set_framebuffer(app.rd, NULL);
		mrl_bind_texture_2d(app.rd, app.shader.bp, app.tex2);
		mrl_draw_triangles_indexed(app.rd, 0, 6);

		// Swap buffers
		mrl_swap_buffers(app.rd);

		mgl_sleep(mgl_from_milliseconds(100));
	}

	unload();

	mgl_close_windows_window(&app.window);
	mgl_terminate();
	return 0;
}

void handle_error(mrl_error_t error, const mgl_chr8_t* msg)
{
	if (error == MRL_ERROR_NONE)
		return;
	if ((error & 0xF000) && mrl_get_mgl_error(error) == MGL_ERROR_NONE)
		return;
	if (msg != NULL)
		mgl_print(mgl_stderr_stream, msg);
	mgl_put_char(mgl_stderr_stream, '\n');
	mgl_print(mgl_stderr_stream, mrl_get_error_string(error));
	mgl_read_chars_until(mgl_stdin_stream, NULL, 0, NULL, u8"\n");
	mgl_abort();
}

void on_window_close(mgl_action_t* action, mgl_enum_t status)
{
	app.running = MGL_FALSE;
}
