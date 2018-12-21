set(HEADERS
        ${HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/ix.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ix_internal.h
        PARENT_SCOPE
        )

set(SOURCE
        ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/ix_internal.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/ix_error.cc
        PARENT_SCOPE
        )