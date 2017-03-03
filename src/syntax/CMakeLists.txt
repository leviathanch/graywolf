
add_library(GraywolfSyntax STATIC
	main.c
	output.c
	readcells.c
	${CMAKE_SOURCE_DIR}/src/date/date.c
	)

target_link_libraries(GraywolfSyntax ${CMAKE_BINARY_DIR}/src/Ylib/libycadgraywolf.a)
target_link_libraries(GraywolfSyntax X11)
target_link_libraries(GraywolfSyntax m)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include .)

install(
	TARGETS GraywolfSyntax
	DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)

