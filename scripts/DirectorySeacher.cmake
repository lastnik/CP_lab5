function(add_dir_to_executable TARGET DIR)
    file(GLOB CPP_FILES ${DIR}/*.cpp)
    message("add to executable ${TARGET} files: ${CPP_FILES}")
    add_executable(${TARGET} ${CPP_FILES})
endfunction()

function(dir_to_target_sources TARGET DIR)
    file(GLOB CPP_FILES ${DIR}/*.cpp)
    message("add to executable ${TARGET} files: ${CPP_FILES}")
    target_sources(${TARGET} PUBLIC ${CPP_FILES})
endfunction()

