set(HEADERS
        ${HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/RecordManager.h
        ${CMAKE_CURRENT_SOURCE_DIR}/RID.h
        ${CMAKE_CURRENT_SOURCE_DIR}/RM_FileHandle.h
        PARENT_SCOPE
        )

set(SOURCE
        ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/RecordManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RM_FileHandle.cpp
        PARENT_SCOPE
        )