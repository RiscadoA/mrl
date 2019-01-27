#include <mrl/ogl_330_render_device.h>
#include <mgl/input/windows_window.h>
#include <mgl/stream/stream.h>
#include <mgl/memory/allocator.h>
#include <mgl/entry.h>

struct
{
	mgl_windows_window_t window;
	mgl_bool_t running;
	mrl_render_device_t* rd;
} app;

void handle_error(mrl_error_t error, const mgl_chr8_t* msg);

void on_window_close(mgl_action_t* action, mgl_enum_t status);

void load(void)
{
	// Init render device
	{
		mrl_render_device_desc_t desc = MRL_DEFAULT_RENDER_DEVICE_DESC;
		desc.allocator = mgl_standard_allocator;
		desc.window = &app.window;
		desc.vsync_mode = MRL_VSYNC_ADAPTIVE;
		handle_error(mrl_init_ogl_330_render_device(&desc, &app.rd), u8"mrl_init_ogl_330_render_device() failed");
	}
}

void unload(void)
{
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
		handle_error(mrl_make_mgl_error(mgl_add_action_callback(&input_manager, mgl_get_window_action(&app.window, MGL_WINDOW_CLOSE), &on_window_close)), u8"mgl_add_action_callback() failed");
	}

	load();

	// Main loop
	while (app.running)
	{
		mgl_poll_window_events(&app.window);

		mrl_clear_color(app.rd, 0.0f, 0.4f, 0.8f, 1.0f);
		mrl_swap_buffers(app.rd);
	}

	unload();

	mgl_close_windows_window(&app.window);
	mgl_terminate();
	return 0;
}

void handle_error(mrl_error_t error, const mgl_chr8_t* msg)
{
	if (error == MRL_ERROR_NONE || mrl_get_mgl_error(error) == MGL_ERROR_NONE)
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
