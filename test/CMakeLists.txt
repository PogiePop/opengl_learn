

add_executable(basic opengl_basic.cpp)
add_test(basic basic)
target_link_libraries(basic PUBLIC ${COMMON_LIBS})

add_executable(sun opengl_sun.cpp)
add_test(sun sun)
target_link_libraries(sun PUBLIC ${COMMON_LIBS})


add_executable(model opengl_model.cpp)
add_test(model model)
target_link_libraries(model PUBLIC ${COMMON_LIBS})