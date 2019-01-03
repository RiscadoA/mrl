#ifdef MRL_BUILD_OGL330
#include <mrl/ogl330/ll_renderer.h>
#include <mgl/error.h>
#include <mgl/memory/allocator.h>
#include <mgl/stream/buffer_stream.h>
#include <mgl/input/window.h>
#include <mgl/string/manipulation.h>

#define GLEW_STATIC
#include <GL/glew.h>

#ifdef _WIN32
#include <Windows.h>
#include <mgl/input/windows_window.h>
#include <GL/wglew.h>
#endif // _WIN32

struct mrl_ogl330_ll_renderer_t
{
	void* allocator;
	void* window;
	void(*error_callback)(const mgl_chr8_t* error);
	void(*warning_callback)(const mgl_chr8_t* msg);
	void(*verbose_callback)(const mgl_chr8_t* msg);
#ifdef _WIN32
	HDC hdc;
	HGLRC hrc;
#endif
};

static mgl_bool_t init_ogl(mrl_ogl330_ll_renderer_t* renderer)
{
	MGL_DEBUG_ASSERT(renderer != NULL);

#ifdef _WIN32
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int pixel_format = ChoosePixelFormat(renderer->hdc, &pfd);
	if (pixel_format == 0)
	{
		if (renderer->error_callback != NULL)
			renderer->error_callback(u8"ChoosePixelFormat() returned 0");
		return MGL_FALSE;
	}

	if (!SetPixelFormat(renderer->hdc, pixel_format, &pfd))
	{
		if (renderer->error_callback != NULL)
			renderer->error_callback(u8"SetPixelFormat() failed");
		return MGL_FALSE;
	}

	HGLRC temp_context = wglCreateContext(renderer->hdc);
	wglMakeCurrent(renderer->hdc, temp_context);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		if (renderer->error_callback != NULL)
		{
			mgl_chr8_t error_buf[4096] = { 0 };
			mgl_buffer_stream_t error_stream;
			mgl_init_buffer_stream(&error_stream, error_buf, sizeof(error_buf));
			mgl_print(&error_stream, u8"glewInit() failed with error ");
			mgl_print_u32(&error_stream, err, 10);
			renderer->error_callback(error_buf);
		}
		return MGL_FALSE;
	}

	int attributes[] = 
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0,
	};

	if (!wglewIsSupported(u8"WGL_ARB_create_context"))
	{
		if (renderer->error_callback != NULL)
			renderer->error_callback(u8"OpenGL 3.3 is not supported");
		return MGL_FALSE;
	}

	renderer->hrc = wglCreateContextAttribsARB(renderer->hdc, 0, attributes);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(temp_context);
	wglMakeCurrent(renderer->hdc, renderer->hrc);

	GLint gl_version[2];
	glGetIntegerv(GL_MAJOR_VERSION, &gl_version[0]);
	glGetIntegerv(GL_MINOR_VERSION, &gl_version[1]);

	if (!renderer->hrc)
	{
		if (renderer->error_callback != NULL)
			renderer->error_callback(u8"Failed to create OpenGL context");
		return MGL_FALSE;
	}

	if (gl_version[0] < 3 || (gl_version[0] == 3 && gl_version[1] < 3))
	{
		if (renderer->error_callback != NULL)
			renderer->error_callback(u8"Unsupported OpenGL version");
		return MGL_FALSE;
	}

	if (renderer->verbose_callback != NULL)
	{
		mgl_chr8_t buf[4096] = { 0 };
		mgl_buffer_stream_t stream;
		mgl_init_buffer_stream(&stream, buf, sizeof(buf));
		mgl_print(&stream, u8"OpenGL version: ");
		mgl_print(&stream, glGetString(GL_VERSION));
		renderer->verbose_callback(buf);
	}

#endif

	return MGL_TRUE;
}

mrl_ogl330_ll_renderer_t* MRL_API mrl_create_ogl330_ll_renderer(const mrl_ogl330_ll_renderer_creation_info_t * info)
{
	MGL_DEBUG_ASSERT(info != NULL && info->window != NULL);

#ifdef _WIN32
	if (!mgl_str_equal(mgl_get_window_type(info->window), u8"win32"))
#endif
	{
		if (info->error_callback != NULL)
		{
			mgl_chr8_t error_buf[4096] = { 0 };
			mgl_buffer_stream_t error_stream;
			mgl_init_buffer_stream(&error_stream, error_buf, sizeof(error_buf));
			mgl_print(&error_stream, u8"Unsupported window type \"");
			mgl_print(&error_stream, mgl_get_window_type(info->window));
			mgl_print(&error_stream, u8"\"");
			info->error_callback(error_buf);
		}
		return NULL;
	}

	mrl_ogl330_ll_renderer_t* renderer;
	mgl_error_t e = mgl_allocate(info->allocator, sizeof(mrl_ogl330_ll_renderer_t), &renderer);
	if (e != MGL_ERROR_NONE)
	{
		if (info->error_callback != NULL)
		{
			mgl_chr8_t error_buf[4096] = { 0 };
			mgl_buffer_stream_t error_stream;
			mgl_init_buffer_stream(&error_stream, error_buf, sizeof(error_buf));
			mgl_print(&error_stream, u8"Failed to allocate mrl_ogl330_ll_renderer_t, mgl_allocate returned ");
			mgl_print(&error_stream, mgl_get_error_string(e));
			info->error_callback(error_buf);
		}
		return NULL;
	}

	renderer->allocator = info->allocator;
	renderer->window = info->window;
	renderer->error_callback = info->error_callback;
	renderer->warning_callback = info->warning_callback;
	renderer->verbose_callback = info->verbose_callback;

#ifdef _WIN32
	renderer->hdc = GetDC((HWND)((mgl_windows_window_t*)renderer->window)->hwnd);
#endif

	if (!init_ogl(renderer))
	{
		MGL_ASSERT(mgl_deallocate(renderer->allocator, renderer) == MGL_ERROR_NONE);
		return NULL;
	}

	return renderer;
}

void MRL_API mrl_destroy_ogl330_ll_renderer(mrl_ogl330_ll_renderer_t * renderer)
{
	MGL_DEBUG_ASSERT(renderer != NULL);

#ifdef _WIN32
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(renderer->hrc);
#endif

	MGL_DEBUG_ASSERT(mgl_deallocate(renderer->allocator, renderer) == MGL_ERROR_NONE);
}

void MRL_API mrl_ogl330_ll_render(mrl_ogl330_ll_renderer_t * renderer, const mgl_u8_t * frame_package)
{
	MGL_DEBUG_ASSERT(renderer != NULL && frame_package != NULL);

	
	glViewport(0, 0, mgl_get_window_width(renderer->window), mgl_get_window_height(renderer->window));
	glClearColor(0.0f, 0.4f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

#ifdef _WIN32
	SwapBuffers(renderer->hdc);
#endif
}

#endif // MRL_BUILD_VULKAN