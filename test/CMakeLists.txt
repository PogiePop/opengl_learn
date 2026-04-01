

add_executable(basic opengl_basic.cpp)
add_test(basic basic)
target_link_libraries(basic PUBLIC ${COMMON_LIBS})

add_executable(sun opengl_sun.cpp)
add_test(sun sun)
target_link_libraries(sun PUBLIC ${COMMON_LIBS})


add_executable(model opengl_model.cpp)
add_test(model model)
target_link_libraries(model PUBLIC ${COMMON_LIBS})


add_executable(depth opengl_depthtest.cpp)
add_test(depth depth)
target_link_libraries(depth PUBLIC ${COMMON_LIBS})

add_executable(stencil opengl_stenciltest.cpp)
add_test(stencil stencil)
target_link_libraries(stencil PUBLIC ${COMMON_LIBS})

add_executable(blend opengl_blend.cpp)
add_test(blend blend)
target_link_libraries(blend PUBLIC ${COMMON_LIBS})

add_executable(cullface opengl_cullface.cpp)
add_test(cullface cullface)
target_link_libraries(cullface PUBLIC ${COMMON_LIBS})

add_executable(framebuffer opengl_framebuffer.cpp)
add_test(framebuffer framebuffer)
target_link_libraries(framebuffer PUBLIC ${COMMON_LIBS})

add_executable(skybox opengl_skybox.cpp)
add_test(skybox skybox)
target_link_libraries(skybox PUBLIC ${COMMON_LIBS})

add_executable(highdata opengl_highdata.cpp)
add_test(highdata highdata)
target_link_libraries(highdata PUBLIC ${COMMON_LIBS})


add_executable(highglsl opengl_highglsl.cpp)
add_test(highglsl highglsl)
target_link_libraries(highglsl PUBLIC ${COMMON_LIBS})

add_executable(geometry opengl_geometry.cpp)
add_test(geometry geometry)
target_link_libraries(geometry PUBLIC ${COMMON_LIBS})

add_executable(instance opengl_instance.cpp)
add_test(instance instance)
target_link_libraries(instance PUBLIC ${COMMON_LIBS})


add_executable(mutisample opengl_mutisample.cpp)
add_test(mutisample mutisample)
target_link_libraries(mutisample PUBLIC ${COMMON_LIBS})

add_executable(blinn opengl_blinn.cpp)
add_test(blinn blinn)
target_link_libraries(blinn PUBLIC ${COMMON_LIBS})

add_executable(gamma opengl_gamma.cpp)
add_test(gamma gamma)
target_link_libraries(gamma PUBLIC ${COMMON_LIBS})

add_executable(shadow opengl_shadow.cpp)
add_test(shadow shadow)
target_link_libraries(shadow PUBLIC ${COMMON_LIBS})

add_executable(shadow1 opengl_shadow1.cpp)
add_test(shadow1 shadow1)
target_link_libraries(shadow1 PUBLIC ${COMMON_LIBS})

add_executable(normalpt opengl_normalpt.cpp)
add_test(normalpt normalpt)
target_link_libraries(normalpt PUBLIC ${COMMON_LIBS})

add_executable(disp opengl_disp.cpp)
add_test(disp disp)
target_link_libraries(disp PUBLIC ${COMMON_LIBS})


add_executable(hdr opengl_hdr.cpp)
add_test(hdr hdr)
target_link_libraries(hdr PUBLIC ${COMMON_LIBS})


add_executable(bloom opengl_bloom.cpp)
add_test(bloom bloom)
target_link_libraries(bloom PUBLIC ${COMMON_LIBS})

add_executable(deferred opengl_deferred.cpp)
add_test(deferred deferred)
target_link_libraries(deferred PUBLIC ${COMMON_LIBS})

add_executable(ssao opengl_ssao.cpp)
add_test(ssao ssao)
target_link_libraries(ssao PUBLIC ${COMMON_LIBS})

#将assimp dll复制到执行文件下的目录
add_custom_command(TARGET model POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${ASSIMP_DLL}"
    $<TARGET_FILE_DIR:model>
)