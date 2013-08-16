# List the source files
set(HEADERS Application.h
)

set(SRCS main.cpp
         Application.cpp
)


# Declaration of the executable
xmake_create_executable(WINGCHUN_TRAININGASSISTANT wingchun-trainingassistant ${HEADERS} ${SRCS})

if (APPLE)
    target_link_libraries(wingchun-trainingassistant "-framework OpenGL -framework GLUT")
else()
    target_link_libraries(wingchun-trainingassistant "-lgl -lglut")
endif()
