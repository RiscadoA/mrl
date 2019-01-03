#include <mrl/ogl330/ll_renderer.h>
#include <mgl/input/windows_window.h>
#include <mgl/stream/stream.h>
#include <mgl/memory/allocator.h>
#include <mgl/entry.h>

mgl_bool_t running = MGL_TRUE;

void handle_error(mgl_error_t error, const mgl_chr8_t* msg);

void on_window_close(mgl_action_t* action, mgl_enum_t status);

void error_callback(const mgl_chr8_t* msg);

void warning_callback(const mgl_chr8_t* msg);

void verbose_callback(const mgl_chr8_t* msg);

int main(int argc, char** argv)
{
	handle_error(mgl_init(), u8"mgl_init() failed");

	// Init input manager
	mgl_input_manager_t input_manager;
	mgl_init_input_manager(&input_manager);

	// Open window
	mgl_windows_window_t window;
	{
		mgl_windows_window_settings_t settings;
		settings.width = 800;
		settings.height = 600;
		settings.mode = MGL_WINDOW_MODE_WINDOWED;
		settings.title = u8"OpenGL 3.3 LL Renderer Direct Usage Example";
		settings.input_manager = &input_manager;
		handle_error(mgl_open_windows_window(&window, &settings), u8"mgl_open_windows_window() failed");
	}

	// Add on window close callback
	handle_error(mgl_add_action_callback(&input_manager, mgl_get_window_action(&window, MGL_WINDOW_CLOSE), &on_window_close), u8"mgl_add_action_callback() failed");

	// Create renderer
	mrl_ogl330_ll_renderer_t* renderer;

	{
		mrl_ogl330_ll_renderer_creation_info_t info;

		info.allocator = mgl_standard_allocator;
		info.window = &window;
		info.error_callback = &error_callback;
		info.warning_callback = &warning_callback;
		info.verbose_callback = &verbose_callback;

		renderer = mrl_create_ogl330_ll_renderer(&info);
	}

	// Main loop
	while (running)
	{
		mgl_poll_window_events(&window);

		mgl_u8_t frame_package[] = { 0 };


		mrl_ogl330_ll_render(renderer, frame_package);
	}

	mrl_destroy_ogl330_ll_renderer(renderer);
	mgl_close_windows_window(&window);
	mgl_terminate();
	return 0;
}

void handle_error(mgl_error_t error, const mgl_chr8_t* msg)
{
	if (error == MGL_ERROR_NONE)
		return;
	if (msg != NULL)
		mgl_print(mgl_stderr_stream, msg);
	mgl_put_char(mgl_stderr_stream, '\n');
	mgl_print(mgl_stderr_stream, mgl_get_error_string(error));
	mgl_read_chars_until(mgl_stdin_stream, NULL, 0, NULL, u8"\n");
	mgl_abort();
}

void on_window_close(mgl_action_t* action, mgl_enum_t status)
{
	running = MGL_FALSE;
}

void error_callback(const mgl_chr8_t* msg)
{
	mgl_print(mgl_stderr_stream, u8"[Renderer Error] ");
	mgl_print(mgl_stderr_stream, msg);
	mgl_read_chars_until(mgl_stdin_stream, NULL, 0, NULL, u8"\n");
	mgl_abort();
}

void warning_callback(const mgl_chr8_t* msg)
{
	mgl_print(mgl_stdout_stream, u8"[Renderer Warning] ");
	mgl_print(mgl_stdout_stream, msg);
	mgl_put_char(mgl_stdout_stream, '\n');
}

void verbose_callback(const mgl_chr8_t* msg)
{
	mgl_print(mgl_stdout_stream, u8"[Renderer Verbose] ");
	mgl_print(mgl_stdout_stream, msg);
	mgl_put_char(mgl_stdout_stream, '\n');
}