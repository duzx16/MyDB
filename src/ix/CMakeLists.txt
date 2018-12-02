set(HEADERS
        ${HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/ix.h
		${CMAKE_CURRENT_SOURCE_DIR}/ix_internal.h
        PARENT_SCOPE
        )

set(SOURCE
        ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/ix_manager.cc
		${CMAKE_CURRENT_SOURCE_DIR}/ix_error.cc
		${CMAKE_CURRENT_SOURCE_DIR}/ix_indexhandle.cc
		${CMAKE_CURRENT_SOURCE_DIR}/ix_indexscan.cc
		${CMAKE_CURRENT_SOURCE_DIR}/ix_internal.cc
        PARENT_SCOPE
        )