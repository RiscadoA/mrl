#ifndef MRL_API_UTILS_H
#define MRL_API_UTILS_H
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef MRL_API
#	ifdef MRL_BUILD_SHARED
#		ifdef _WIN32
#			ifdef MRL_API_EXPORT
#				define MRL_API __declspec(dllexport)
#			else
#				define MRL_API __declspec(dllimport)
#			endif
#		else
#			define MRL_API
#		endif
#	else
#		define MRL_API
#	endif
#endif

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MRL_API_UTILS_H
