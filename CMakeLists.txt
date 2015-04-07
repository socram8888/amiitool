cmake_minimum_required(VERSION 2.8.4)
project(nfc3d)

include_directories("include")

set(HEADER_FILES
	include/drbg.h
	include/keygen.h
	include/ninkeys.h
)

set(LIB_FILES
	${HEADER_FILES}
	src/drbg.c
	src/keygen.c
	src/ninkeys.c
)

set(ZEROTEST_FILES
	${LIB_FILES}
	src/zerotest.c
)

add_executable(zerotest ${ZEROTEST_FILES})
target_link_libraries(zerotest crypto)
