
#add_executable(mc_compact
add_library(graywolf_mc_compact STATIC
	cdraw.c
	compactor.c
	io.c
	multi.c
	stdmacro.c
	changraph.c
	debug2.c
	movestrat2.c
	readcgraph.c
	xcompact.c
	cmain.c
	grid.c
	movestrat.c
	readtiles.c
	ycompact.c
	${CMAKE_SOURCE_DIR}/src/date/date.c
	)

#target_link_libraries(mc_compact ${CMAKE_BINARY_DIR}/src/Ylib/libycadgraywolf.a)
target_link_libraries(graywolf_mc_compact ${CMAKE_BINARY_DIR}/src/Ylib/libycadgraywolf.a)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include .)

install(
	TARGETS graywolf_mc_compact 
	DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)

