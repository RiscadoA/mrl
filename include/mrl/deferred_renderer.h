#ifndef MRL_DEFERRED_RENDERER_H
#define MRL_DEFERRED_RENDERER_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <mrl/renderer.h>

	typedef struct mrl_deferred_renderer_desc_t mrl_deferred_renderer_desc_t;
	struct mrl_deferred_renderer_desc_t
	{
		void;
	};

	/// <summary>
	///		Initializes a new deferred renderer.
	/// </summary>
	/// <param name="desc">Renderer description</param>
	/// <param name="out_r">Out renderer handle</param>
	/// <returns>Error code</returns>
	MRL_API mrl_error_t mrl_init_deferred_renderer(const mrl_deferred_renderer_desc_t* desc, mrl_renderer_t** out_r);

	/// <summary>
	///		Terminates a deferred renderer.
	/// </summary>
	/// <param name="r">Renderer handle</param>
	MRL_API void mrl_terminate_deferred_renderer(mrl_renderer_t* r);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif
