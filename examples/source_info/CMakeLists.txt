set(NAME source_info)
set(TYPE application)
set(INCLUDE_DIR "include/source_info")
set(SRC_DIR "src")

set(HEADERS
)
 
set(SOURCES
    ${SRC_DIR}/main.cpp
)

set(DEPS_PUBLIC
    logandload
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    EXAMPLE
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
)