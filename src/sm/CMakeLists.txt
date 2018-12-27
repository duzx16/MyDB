set(HEADERS
        ${HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/sm.h
        ${CMAKE_CURRENT_SOURCE_DIR}/sm_internal.h
        PARENT_SCOPE
        )

set(SOURCE
        ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/sm_error.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/sm_manager.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/sm_internal.cc
        PARENT_SCOPE
        )