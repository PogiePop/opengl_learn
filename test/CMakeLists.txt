

add_executable(basic opengl_basic.cpp)
add_test(basic basic)
target_link_libraries(basic PUBLIC ${COMMON_LIBS})