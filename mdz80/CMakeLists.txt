project(mdz80)

cmake_minimum_required(VERSION 2.8)

find_package(BISON)

BISON_TARGET(zmac zmac.y ${CMAKE_CURRENT_BINARY_DIR}/zmac.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(mdz80
	mdz80.c
	analyze.c
	mio.c
	opcodes_8080.c
	opcodes_z80.c
	pass1.c
	pass2.c
	${BISON_zmac_OUTPUTS}
)

install(TARGETS mdz80 RUNTIME DESTINATION bin)
