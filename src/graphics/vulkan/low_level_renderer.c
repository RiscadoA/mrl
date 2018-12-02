#include "low_level_renderer.h"

#include <mgl/memory/allocator.h>
#include <mgl/memory/manipulation.h>

struct mgl_vulkan_low_level_renderer_t
{
	mgl_vulkan_low_level_renderer_info_t info;
};

mgl_vulkan_low_level_renderer_t * mgl_create_vulkan_low_level_renderer(const mgl_vulkan_low_level_renderer_info_t * info)
{
	MGL_DEBUG_ASSERT(info != NULL);

	mgl_vulkan_low_level_renderer_t* renderer = NULL;
	mgl_error_t e = mgl_allocate(info->allocator, sizeof(mgl_vulkan_low_level_renderer_t), &renderer);
	if (e != MGL_ERROR_NONE)
		return NULL;
	mgl_mem_copy(&renderer->info, info, sizeof(mgl_vulkan_low_level_renderer_info_t));



	return renderer;
}

void mgl_destroy_vulkan_low_level_renderer(mgl_vulkan_low_level_renderer_t * renderer)
{
	MGL_DEBUG_ASSERT(renderer != NULL);
	MGL_ASSERT(mgl_deallocate(renderer->info.allocator, renderer) == MGL_ERROR_NONE);
}

void mgl_vulkan_low_level_renderer_render(mgl_vulkan_low_level_renderer_t * renderer, const void * frame)
{

}
