#
# IDE support for headers
#

set(ASL_HEADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/../include")

file(GLOB ASL_HEADERS "${ASL_HEADERS_DIR}/jhoyt/asl/*.hpp")

set(ASL_ALL_HEADERS ${ASL_HEADERS})

source_group("Header Files\\jhoyt\\asl" FILES ${ASL_HEADERS})
