# FetchContent
include(FetchContent)
set(FETCHCONTENT_QUIET    off)
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/external")

function(GitHelper name url tag add_sub inc_dir link_lib)
    FetchContent_Declare(
        ${name}
        GIT_REPOSITORY ${url}
        GIT_TAG        ${tag}
        GIT_PROGRESS   ON
    )
    FetchContent_GetProperties(${name})
    string(TOLOWER ${name} lcName)
    if (NOT ${lcName}_POPULATED)
        FetchContent_Populate(${name})
        set(${lcName}_SOURCE_DIR ${${lcName}_SOURCE_DIR} PARENT_SCOPE)
        set(${lcName}_BINARY_DIR ${${lcName}_BINARY_DIR} PARENT_SCOPE)
        if (add_sub)
            add_subdirectory(${${lcName}_SOURCE_DIR} ${${lcName}_BINARY_DIR} EXCLUDE_FROM_ALL)
        endif()
    endif()

    set(snow_include_directories ${snow_include_directories} ${${lcName}_SOURCE_DIR}/${inc_dir} PARENT_SCOPE)
    set(snow_link_libraries      ${snow_link_libraries} ${link_lib}                             PARENT_SCOPE)
endfunction(GitHelper)


# macro to get source codes
function(FetchSnowModule module)
    string(TOLOWER ${module} _TMP_PATH)
    string(TOUPPER ${module} _TMP_NAME)

    set(SNOW_ROOT_${_TMP_NAME} ${PROJECT_SOURCE_DIR}/snow/${_TMP_PATH})
    file(GLOB_RECURSE SNOW_HEADERS_${_TMP_NAME}
        ${SNOW_ROOT_${_TMP_NAME}}/*.h
        ${SNOW_ROOT_${_TMP_NAME}}/*.hpp
    )
    file(GLOB_RECURSE SNOW_SOURCES_${_TMP_NAME}
        ${SNOW_ROOT_${_TMP_NAME}}/*.cxx
        ${SNOW_ROOT_${_TMP_NAME}}/*.cpp
    )

    set(snow_headers ${snow_headers} ${SNOW_HEADERS_${_TMP_NAME}} PARENT_SCOPE)
    set(snow_sources ${snow_sources} ${SNOW_SOURCES_${_TMP_NAME}} PARENT_SCOPE)
endfunction(FetchSnowModule)