PROJECT("cb2util" C)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(ZLIB REQUIRED)

SET(BIGINT "libbig_int")

SET(CMAKE_C_FLAGS "-Wall -Werror -O2 -s")

INCLUDE_DIRECTORIES(
	${BIGINT}/include
	${ZLIB_INCLUDE_DIR}
)

ADD_LIBRARY(bigint
	${BIGINT}/src/basic_funcs.c
	${BIGINT}/src/bitset_funcs.c
	${BIGINT}/src/low_level_funcs/add.c
	${BIGINT}/src/low_level_funcs/and.c
	${BIGINT}/src/low_level_funcs/andnot.c
	${BIGINT}/src/low_level_funcs/cmp.c
	${BIGINT}/src/low_level_funcs/div.c
	${BIGINT}/src/low_level_funcs/mul.c
	${BIGINT}/src/low_level_funcs/or.c
	${BIGINT}/src/low_level_funcs/sqr.c
	${BIGINT}/src/low_level_funcs/sub.c
	${BIGINT}/src/low_level_funcs/xor.c
	${BIGINT}/src/memory_manager.c
	${BIGINT}/src/modular_arithmetic.c
	${BIGINT}/src/number_theory.c
	${BIGINT}/src/service_funcs.c
	${BIGINT}/src/str_funcs.c
)

ADD_EXECUTABLE(cb2util
	arcfour.c
	cb2_crypto.c
	cb2util.c
	cbc.c
	cheats.c
	compress.c
	fileio.c
	pcb.c
	shs.c
)

TARGET_LINK_LIBRARIES(cb2util bigint ${ZLIB_LIBRARIES})

INSTALL(TARGETS cb2util DESTINATION bin)
