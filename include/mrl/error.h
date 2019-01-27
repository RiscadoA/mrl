#ifndef MRL_ERROR_H
#define MRL_ERROR_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <mgl/error.h>
#include <mrl/api_utils.h>

	typedef mgl_enum_t mrl_error_t;

	enum
	{
		MRL_ERROR_NONE						= 0x00,
		MRL_ERROR_UNSUPPORTED_WINDOW		= 0x01,
		MRL_ERROR_UNSUPPORTED_DEVICE		= 0x02,
		MRL_ERROR_EXTERNAL					= 0x03,
	};

	/// <summary>
	///		Turns a MGL error code into a MRL error code.
	/// </summary>
	/// <param name="err">MGL error code</param>
	/// <returns>MRL error code</returns>
	MRL_API mrl_error_t mrl_make_mgl_error(mgl_error_t err);

	/// <summary>
	///		Gets an MGL error code from an MRL error code.
	/// </summary>
	/// <param name="err">MRL error code</param>
	/// <returns>MGL error code</returns>
	MRL_API mgl_error_t mrl_get_mgl_error(mrl_error_t err);

	/// <summary>
	///		Gets the error string of a MRL error code.
	/// </summary>
	/// <param name="err">MRL error code</param>
	/// <returns>Error string</returns>
	MRL_API const mgl_chr8_t* mrl_get_error_string(mrl_error_t err);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
