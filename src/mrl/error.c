#include <mrl/error.h>

MRL_API mrl_error_t mrl_make_mgl_error(mgl_error_t err)
{
	return err | 0xF000;
}

MRL_API mgl_error_t mrl_get_mgl_error(mrl_error_t err)
{
	if (err & 0xF000)
		return err & 0x0FFF;
	else
		return MGL_ERROR_NONE;
}

MRL_API const mgl_chr8_t * mrl_get_error_string(mrl_error_t err)
{
	if (err & 0xF000)
	{
		mgl_error_t e = err & (0x0FFF);
		return mgl_get_error_string(e);
	}

	switch (err)
	{
		case MRL_ERROR_NONE: return u8"MRL_ERROR_NONE: No error";
		case MRL_ERROR_UNSUPPORTED_WINDOW: return u8"MRL_ERROR_UNSUPPORTED_PLATFORM: Unsupported render device window type";
		case MRL_ERROR_UNSUPPORTED_DEVICE: return u8"MRL_ERROR_UNSUPPORTED_DEVICE: Unsupported render device type";
		case MRL_ERROR_EXTERNAL: return u8"MRL_ERROR_EXTERNAL: External library error";
		default: return u8"???: Unknown error";
	}
	return NULL;
}
