
add_executable(mld80
	do_out.c
	fixup.c
	main.c
	readobj.c
	section.c
	symbol.c
)

install(TARGETS mld80 RUNTIME DESTINATION bin)
