
set(features
    set-input-output.feature
    write_text_stream.feature
    plot.feature
    read_json.feature
    operations.feature
    write_json_sparse_arrays.feature
    write_json.feature
    unitmgr.feature
    message_queue_reply.feature)

foreach(feature IN LISTS features)
   #message("feature: ${feature}")
   get_filename_component(base_name features/${feature} NAME_WE)
   add_test(NAME ${base_name}
        COMMAND
          python3 -m behave features/${base_name}.feature
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
   set_tests_properties(${base_name}
      PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_SOURCE_DIR}/python-packages")
endforeach()
