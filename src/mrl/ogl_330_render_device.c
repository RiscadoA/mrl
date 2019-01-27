#include <mrl/ogl_330_render_device.h>

#include <mgl/memory/allocator.h>
#include <mgl/memory/manipulation.h>
#include <mgl/string/manipulation.h>
#include <mgl/input/window.h>

#ifdef MRL_BUILD_OGL_330
#	ifdef MGL_SYSTEM_WINDOWS
#		include <mgl/input/windows_window.h>
#		define GLEW_STATIC
#		include <GL/glew.h>
#		include <GL/wglew.h>
#	endif

typedef struct
{
	mrl_render_device_t base;
	void* allocator;
	void* window;
#	ifdef MGL_SYSTEM_WINDOWS
	struct
	{
		HDC hdc;
		HGLRC hrc;
	} win32;
#	endif
} mrl_ogl_330_render_device_t;

static void clear_color(mrl_render_device_t* brd, mgl_f32_t r, mgl_f32_t g, mgl_f32_t b, mgl_f32_t a)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

static void clear_depth(mrl_render_device_t* brd, mgl_f32_t depth)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	glClearDepth(depth);
	glClear(GL_DEPTH_BUFFER_BIT);
}

static void clear_stencil(mrl_render_device_t* brd, mgl_i32_t stencil)
{
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	glClearStencil(stencil);
	glClear(GL_STENCIL_BUFFER_BIT);
}

static void win32_swap_buffers(mrl_render_device_t* brd)
{
#	ifdef MGL_SYSTEM_WINDOWS
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;
	SwapBuffers(rd->win32.hdc);
#	endif
}

static const mgl_chr8_t* get_type_name(mrl_render_device_t* rd)
{
	return u8"ogl_330";
}

static void set_rd_functions(mrl_ogl_330_render_device_t* rd)
{
	// Draw functions
	rd->base.clear_color = &clear_color;
	rd->base.clear_depth = &clear_depth;
	rd->base.clear_stencil = &clear_stencil;

	// Swap buffers
	if (mgl_str_equal(u8"win32", mgl_get_window_type(rd->window)))
	{
#	ifndef MGL_SYSTEM_WINDOWS
		mgl_abort();
#	else
		rd->base.swap_buffers = &win32_swap_buffers;
#	endif
	}
	else mgl_abort();

	rd->base.get_type_name = &get_type_name;
}

static mrl_error_t create_gl_context(mrl_ogl_330_render_device_t* rd, const mrl_render_device_desc_t* desc)
{
	if (mgl_str_equal(u8"win32", mgl_get_window_type(rd->window)))
	{
#	ifndef MGL_SYSTEM_WINDOWS
		return MRL_ERROR_UNSUPPORTED_WINDOW;
#	else
		HWND hwnd = (HWND)((mgl_windows_window_t*)rd->window)->hwnd;
		rd->win32.hdc = GetDC(hwnd);

		// Init context
		PIXELFORMATDESCRIPTOR pfd;
		mgl_mem_set(&pfd, sizeof(pfd), 0);
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int n_pixel_format = ChoosePixelFormat(rd->win32.hdc, &pfd);
		if (n_pixel_format == 0)
			return MRL_ERROR_EXTERNAL;
		if (!SetPixelFormat(rd->win32.hdc, n_pixel_format, &pfd))
			return MRL_ERROR_EXTERNAL;

		HGLRC temp_context = wglCreateContext(rd->win32.hdc);
		wglMakeCurrent(rd->win32.hdc, temp_context);

		if (glewInit() != GLEW_OK)
			return MGL_ERROR_EXTERNAL;

		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0,
		};

		if (!WGL_ARB_create_context)
			return MRL_ERROR_UNSUPPORTED_DEVICE;

		rd->win32.hrc = wglCreateContextAttribsARB(rd->win32.hdc, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(temp_context);
		wglMakeCurrent(rd->win32.hdc, rd->win32.hrc);
		
		if (WGL_EXT_swap_control_tear && desc->vsync_mode == MRL_VSYNC_ADAPTIVE)
			wglSwapIntervalEXT(-1);
		else if (WGL_EXT_swap_control)
		{
			if (desc->vsync_mode == MRL_VSYNC_ADAPTIVE || desc->vsync_mode == MRL_VSYNC_ON)
				wglSwapIntervalEXT(1);
			else
				wglSwapIntervalEXT(0);
		}

		if (!rd->win32.hrc)
			return MRL_ERROR_EXTERNAL;
		return MRL_ERROR_NONE;
#	endif
	}
	else
		return MRL_ERROR_UNSUPPORTED_WINDOW;
}

static void destroy_gl_context(mrl_ogl_330_render_device_t* rd)
{
	if (mgl_str_equal(u8"win32", mgl_get_window_type(rd->window)))
	{
#	ifndef MGL_SYSTEM_WINDOWS
		mgl_abort();
#	else
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(rd->win32.hrc);
#	endif
	}
	else
		mgl_abort();
}
#endif

MRL_API mrl_error_t mrl_init_ogl_330_render_device(const mrl_render_device_desc_t* desc, mrl_render_device_t ** out_rd)
{
	MGL_DEBUG_ASSERT(desc != NULL && out_rd != NULL);
	MGL_DEBUG_ASSERT(desc->allocator != NULL && desc->window != NULL);

#ifndef MRL_BUILD_OGL_330
	return MRL_ERROR_UNSUPPORTED_DEVICE;
#else
	// Allocate render device
	mrl_ogl_330_render_device_t* rd;
	mgl_error_t mglerr = mgl_allocate(desc->allocator, sizeof(mrl_ogl_330_render_device_t), (void**)&rd);
	if (mglerr != MGL_ERROR_NONE)
		return mrl_make_mgl_error(mglerr);

	rd->allocator = desc->allocator;
	rd->window = desc->window;

	// Init context
	mrl_error_t err = create_gl_context(rd, desc);
	if (err != MRL_ERROR_NONE)
	{
		mgl_deallocate(rd->allocator, rd);
		return err;
	}

	// Set render device funcs
	set_rd_functions(rd);

	*out_rd = (mrl_render_device_t*)rd;

	return MRL_ERROR_NONE;
#endif
}

MRL_API void mrl_terminate_ogl_330_render_device(mrl_render_device_t * brd)
{
	MGL_DEBUG_ASSERT(brd != NULL);

#ifndef MRL_BUILD_OGL_330
	mgl_abort();
#else
	mrl_ogl_330_render_device_t* rd = (mrl_ogl_330_render_device_t*)brd;

	// Terminate context
	destroy_gl_context(rd);

	// Deallocate
	MGL_DEBUG_ASSERT(mgl_deallocate(rd->allocator, rd) == MRL_ERROR_NONE);
#endif
}
