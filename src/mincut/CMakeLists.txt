
add_library(GraywolfMincut STATIC 
	main.c
	output.c
	readcells.c
	${CMAKE_SOURCE_DIR}/src/date/date.c
	)

target_link_libraries(GraywolfMincut X11)
target_link_libraries(GraywolfMincut m)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include .)

#install(
#	TARGETS GraywolfMincut
#	DESTINATION ${CMAKE_INSTALL_LIBDIR}
#	)

