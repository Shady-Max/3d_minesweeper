string(REPLACE "|" ";" SHADER_FILES_LIST "${SHADER_FILES}")

file(WRITE "${GENERATED_HEADER}" "// Auto-generated from GLSL files\n\n")

foreach(SHADER_FILE IN LISTS SHADER_FILES_LIST)
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    string(REPLACE "." "_" SHADER_NAME ${SHADER_NAME})

    file(READ ${SHADER_FILE} SHADER_SRC)

    string(REPLACE "\\" "\\\\" SHADER_SRC "${SHADER_SRC}")
    string(REPLACE "\"" "\\\"" SHADER_SRC "${SHADER_SRC}")

    file(APPEND "${GENERATED_HEADER}"
            "static const char* ${SHADER_NAME}_glsl = R\"(${SHADER_SRC})\";\n\n"
    )
endforeach()