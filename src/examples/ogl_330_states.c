#include <mrl/ogl_330_render_device.h>
#include <mgl/input/windows_window.h>
#include <mgl/stream/stream.h>
#include <mgl/string/manipulation.h>
#include <mgl/memory/allocator.h>
#include <mgl/entry.h>

#include <stddef.h>

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
	
	struct
	{
		mgl_u64_t pos_offsets[2];
		mgl_u64_t color_offsets[2];
	} cb_struct;

	mrl_raster_state_t* rs;
	mrl_depth_stencil_state_t* dss;
	mrl_blend_state_t* bs;

	mrl_constant_buffer_t* cbo;
	mrl_index_buffer_t* ibo;
	mrl_vertex_buffer_t* vbo;
	mrl_vertex_array_t* vao;
} app;

typedef struct
{
	mgl_f32_t x, y;
	mgl_f32_t r, g, b, a;
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
		hint.type = MRL_HINT_ERROR_CALLBACK;
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
			"in vec4 color;"

			"out vec4 frag_color;"

			"uniform data_cb { vec4 color[2]; vec2 position[2]; } data;"

			"void main() {"
			"	frag_color = color * data.color[gl_InstanceID];"
			"	gl_Position = vec4(position + data.position[gl_InstanceID], 0.0, 1.0);"
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

			"in vec4 frag_color;"

			"out vec4 color;"

			"void main() {"
			"	color = frag_color;"
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

	// Get CB BP
	app.shader.bp = mrl_get_shader_binding_point(app.rd, app.shader.pipeline, u8"data_cb");

	// Init CBO
	{
		mrl_constant_buffer_structure_t cbs;
		mrl_query_constant_buffer_structure(app.rd, app.shader.bp, &cbs);

		for (mgl_u32_t i = 0; i < cbs.element_count; ++i)
		{
			if (mgl_str_equal(u8"position", cbs.elements[i].name))
			{
				MGL_DEBUG_ASSERT(cbs.elements[i].size == 2);
				app.cb_struct.pos_offsets[0] = cbs.elements[i].offset;
				app.cb_struct.pos_offsets[1] = cbs.elements[i].offset + cbs.elements[i].array_stride;
			}
			else if (mgl_str_equal(u8"color", cbs.elements[i].name))
			{
				MGL_DEBUG_ASSERT(cbs.elements[i].size == 2);
				app.cb_struct.color_offsets[0] = cbs.elements[i].offset;
				app.cb_struct.color_offsets[1] = cbs.elements[i].offset + cbs.elements[i].array_stride;
			}
		}

		mrl_constant_buffer_desc_t desc = MRL_DEFAULT_CONSTANT_BUFFER_DESC;
		desc.data = NULL;
		desc.size = cbs.size;
		desc.usage = MRL_CONSTANT_BUFFER_USAGE_DYNAMIC;

		handle_error(mrl_create_constant_buffer(app.rd, &app.cbo, &desc), u8"Failed to create constant buffer");
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
		desc.usage = MRL_INDEX_BUFFER_USAGE_STATIC;

		handle_error(mrl_create_index_buffer(app.rd, &app.ibo, &desc), u8"Failed to create index buffer");
	}

	// Init VBO
	{
		my_vertex_t data[4] =
		{
			{ -0.5f, -0.5f,	1.0f, 0.0f, 0.0f, 1.0f },
			{ +0.5f, -0.5f,	0.0f, 1.0f, 0.0f, 1.0f },
			{ +0.5f, +0.5f,	0.0f, 0.0f, 1.0f, 1.0f },
			{ -0.5f, +0.5f,	1.0f, 0.0f, 1.0f, 1.0f },
		};

		mrl_vertex_buffer_desc_t desc = MRL_DEFAULT_VERTEX_BUFFER_DESC;
		desc.data = data;
		desc.size = sizeof(data);
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
		mgl_str_copy(u8"color", desc.elements[1].name, MRL_MAX_VERTEX_ELEMENT_NAME_SIZE);
		desc.elements[1].size = 4;
		desc.elements[1].type = MRL_VERTEX_ELEMENT_TYPE_F32;
		desc.elements[1].buffer.index = 0;
		desc.elements[1].buffer.offset = offsetof(my_vertex_t, r);
		desc.elements[1].buffer.stride = sizeof(my_vertex_t);
		
		// Set VBOs
		desc.buffer_count = 1;
		desc.buffers[0] = app.vbo;

		// Set shader pipeline
		desc.shader_pipeline = app.shader.pipeline;

		handle_error(mrl_create_vertex_array(app.rd, &app.vao, &desc), u8"Failed to create vertex array");
	}

	// Init raster state
	{
		mrl_raster_state_desc_t desc = MRL_DEFAULT_RASTER_STATE_DESC;

		handle_error(mrl_create_raster_state(app.rd, &app.rs, &desc), u8"Failed to create raster state");

		mrl_set_raster_state(app.rd, app.rs);
	}

	// Init depth stencil state
	{
		mrl_depth_stencil_state_desc_t desc = MRL_DEFAULT_DEPTH_STENCIL_STATE_DESC;

		handle_error(mrl_create_depth_stencil_state(app.rd, &app.dss, &desc), u8"Failed to create depth stencil state");

		mrl_set_depth_stencil_state(app.rd, app.dss);
	}

	// Init blend state
	{
		mrl_blend_state_desc_t desc = MRL_DEFAULT_BLEND_STATE_DESC;

		desc.blend_enabled = MGL_TRUE;

		handle_error(mrl_create_blend_state(app.rd, &app.bs, &desc), u8"Failed to create blend state");

		mrl_set_blend_state(app.rd, app.bs);
	}
}

void unload(void)
{
	// Terminate states
	mrl_set_raster_state(app.rd, NULL);
	mrl_destroy_raster_state(app.rd, app.rs);
	mrl_set_depth_stencil_state(app.rd, NULL);
	mrl_destroy_depth_stencil_state(app.rd, app.dss);
	mrl_set_blend_state(app.rd, NULL);
	mrl_destroy_blend_state(app.rd, app.bs);

	// Terminate vertex array
	mrl_destroy_vertex_array(app.rd, app.vao);

	// Terminate vertex buffer
	mrl_destroy_vertex_buffer(app.rd, app.vbo);

	// Terminate index buffer
	mrl_destroy_index_buffer(app.rd, app.ibo);

	// Terminate constant buffer
	mrl_destroy_constant_buffer(app.rd, app.cbo);

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

	// Bind objects
	mrl_set_shader_pipeline(app.rd, app.shader.pipeline);
	mrl_set_index_buffer(app.rd, app.ibo);
	mrl_set_vertex_array(app.rd, app.vao);
	mrl_bind_constant_buffer(app.rd, app.shader.bp, app.cbo);

	// Main loop
	while (app.running)
	{
		mgl_poll_window_events(&app.window);

		mrl_clear_color(app.rd, 0.0f, 0.4f, 0.8f, 1.0f);

		// Update constant buffer
		{
			mgl_u8_t* data = (mgl_u8_t*)mrl_map_constant_buffer(app.rd, app.cbo);

			mgl_f32_t* pos_1 = (mgl_f32_t*)(data + app.cb_struct.pos_offsets[0]);
			pos_1[0] = -0.5f;
			pos_1[1] = 0.0f;
			
			mgl_f32_t* color_1 = (mgl_f32_t*)(data + app.cb_struct.color_offsets[0]);
			color_1[0] = 1.0f;
			color_1[1] = 0.0f;
			color_1[2] = 0.0f;
			color_1[3] = 1.0f;

			mgl_f32_t* pos_2 = (mgl_f32_t*)(data + app.cb_struct.pos_offsets[1]);
			pos_2[0] = 0.0f;
			pos_2[1] = 0.0f;

			mgl_f32_t* color_2 = (mgl_f32_t*)(data + app.cb_struct.color_offsets[1]);
			color_2[0] = 1.0f;
			color_2[1] = 1.0f;
			color_2[2] = 1.0f;
			color_2[3] = 0.5f;

			mrl_unmap_constant_buffer(app.rd, app.cbo);
		}	
		mrl_draw_triangles_indexed_instanced(app.rd, 0, 6, 2);

		mrl_swap_buffers(app.rd);
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
