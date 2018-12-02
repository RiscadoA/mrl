# ./CMakeLists.txt
cmake_minimum_required(VERSION 3.11)
project(libmrl VERSION 0.1.0 LANGUAGES C)
include(GenerateExportHeader)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#####################################################
# MRL source and include files

set(MRL_SOURCE
	
)

set(MRL_INCLUDE
	
)

#####################################################
# Set options
option(MRL_BUILD_SHARED OFF)
option(MRL_BUILD_VULKAN ON)

#####################################################
# Create MRL target and set its properties

if(MRL_BUILD_SHARED)
	add_library(mrl SHARED ${MRL_SOURCE} ${MRL_INCLUDE})
	target_compile_definitions(mrl PUBLIC MRL_BUILD_SHARED)
else()
	add_library(mrl STATIC ${MRL_SOURCE} ${MRL_INCLUDE})
endif()
add_library(MRL::mrl ALIAS mrl)

# Link MRL
find_package(MRL REQUIRED)
target_link_libraries(mrl PUBLIC MRL::MRL)

# Link Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(mrl PRIVATE Vulkan::Vulkan)

generate_export_header(mrl)
target_include_directories(mrl
	PUBLIC
		$<INSTALL_INTERFACE:include/>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(mrl PRIVATE -WX -W3)
target_compile_definitions(mrl
	PUBLIC
		MRL_VERSION="${PROJECT_VERSION}"
		MRL_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
		MRL_VERSION_MINOR=${PROJECT_VERSION_MINOR}
		MRL_VERSION_PATCH=${PROJECT_VERSION_PATCH}
	PRIVATE
		MRL_API_EXPORT
)

if (MRL_BUILD_VULKAN)
	target_compile_definitions(mrl PUBLIC MRL_BUILD_VULKAN)
endif()

# Add file filters
foreach(_source IN ITEMS ${MRL_SOURCE} ${MRL_INCLUDE})
	if (IS_ABSOLUTE "${_source}")
		file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
	else()
		set(_source_rel "${_source}")
	endif()
	get_filename_component(_source_path "${_source_rel}" PATH)
	string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
	source_group("${_source_path_msvc}" FILES "${_source}")
endforeach()

#####################################################
# Installation instructions

include (GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/MRL)

install(TARGETS mrl
    EXPORT mrl-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_target_properties(mrl PROPERTIES EXPORT_NAME MRL)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export targets
install(EXPORT mrl-targets
	FILE
		MRLTargets.cmake
	NAMESPACE
		MRL::
	DESTINATION
		${INSTALL_CONFIGDIR}
)

# Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/MRLConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/MRLConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MRLConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

# Install the Config, ConfigVersion and custom find modules
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MRLConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MRLConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

##############################################
# Exporting from the build tree

export(EXPORT mrl-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/MRLTargets.cmake NAMESPACE MRL::)

# Register package in user's package registry
export(PACKAGE MRL)