# List the source files
set(HEADERS Application.h
            FrontView.h
            TextureView.h
            TopView.h
)

set(SRCS main.cpp
         Application.cpp
         FrontView.cpp
         TextureView.cpp
         TopView.cpp
)


# List the include paths
xmake_import_search_paths(OPENNI2)
xmake_import_search_paths(NITE2)

include_directories("${WINGCHUN_TRAININGASSISTANT_SOURCE_DIR}/dependencies")
include_directories("${FREENECT_SDK_SOURCE_DIR}")


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEVELOPMENT_FEATURES)
endif()


# Declaration of the executable
xmake_create_executable(WINGCHUN_TRAININGASSISTANT wingchun-trainingassistant ${HEADERS} ${SRCS})

xmake_project_link(WINGCHUN_TRAININGASSISTANT OPENNI2 NITE2)

if (APPLE)
    target_link_libraries(wingchun-trainingassistant "-framework OpenGL -framework GLUT")
else()
    target_link_libraries(wingchun-trainingassistant "-lgl -lglut")
endif()
