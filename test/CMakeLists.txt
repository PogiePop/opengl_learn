

add_executable(basic opengl_basic.cpp)
add_test(basic basic)
target_link_libraries(basic PUBLIC ${COMMON_LIBS})

add_executable(sun opengl_sun.cpp)
add_test(sun sun)
target_link_libraries(sun PUBLIC ${COMMON_LIBS})


add_executable(model opengl_model.cpp)
add_test(model model)
target_link_libraries(model PUBLIC ${COMMON_LIBS})



#将assimp dll复制到执行文件下的目录
add_custom_command(TARGET model POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${ASSIMP_DLL}"
    $<TARGET_FILE_DIR:model>
)