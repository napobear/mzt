
find_package(BISON)

BISON_TARGET(zmac zmac.y ${CMAKE_CURRENT_BINARY_DIR}/zmac.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


add_executable(mzmac
	${BISON_zmac_OUTPUTS}
	help.c
	zi80dis.cpp
)

install(TARGETS mzmac RUNTIME DESTINATION bin)
