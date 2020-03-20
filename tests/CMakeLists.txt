set(test_base
    approx.cc approx.h
    approx_equal.cc approx_equal.h
    stringeq.cc stringeq.h
    falsestring.cc falsestring.h
    utils.h
    vectortostring.h
    main.cc
)

set(app_src
    test_angle.cc
    test_argparse.cc
    test_axisangle.cc
    test_base64.cc
    test_bitfield.cc
    test_camera.cc
    test_chatbot.cc
    test_commandlineparser.cc
    test_duk.cc
    test_drunken_bishop.cc
    test_editdistance.cc
    test_enumtostring.cc
    test_findstring.cc
    test_fnv1a.cc
    test_image.cc
    test_image.png
    test_insertionsort.cc
    test_interpolate.cc
    test_mat4.cc
    test_minsynth.cc
    test_multisort.cc
    test_numeric.cc
    test_quat.cc
    test_quicksort.cc
    test_radix.cc
    test_range.cc
    test_rect.cc
    test_rgb.cc
    test_rgb_blend.cc
    test_stringmerger.cc
    test_stringtable.cc
    test_stringutils.cc
    test_table.cc
    test_tablelayout.cc
    test_template.cc
    test_tests.cc
    test_textbox.cc
    test_textfileparser.cc
    test_tracery.cc
    test_ui_text.cc
    test_utf8.cc
    test_vec2.cc
    test_vec3.cc
    test_vec4.cc
    test_vfs.cc
    test_vfspath.cc
    test_viewport.cc
    test_wordwrap.cc
)


set(src ${app_src} ${test_base})

source_group("tests" FILES ${app_src})
source_group("base" FILES ${test_base})

include_directories(SYSTEM ../external/catch/)

add_executable(tests ${src})

TARGET_LINK_LIBRARIES(tests ${CMAKE_THREAD_LIBS_INIT} core duk minsynth)
