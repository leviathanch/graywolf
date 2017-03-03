
add_library(GraywolfGenrows STATIC
	draw.c
	genrows.c
	main.c
	merge.c
	readpar.c
	${CMAKE_SOURCE_DIR}/src/date/date.c
	)

target_link_libraries(GraywolfGenrows X11)
target_link_libraries(GraywolfGenrows m)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include .)

install(
	TARGETS GraywolfGenrows
	DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)

