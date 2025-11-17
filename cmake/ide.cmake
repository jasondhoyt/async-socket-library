#
# IDE support for headers
#

set(ASL_HEADERS_DIR "${CMAKE_CURRENT_LIST_DIR}/../include")
set(ASL_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../src")

file(GLOB ASL_HEADERS "${ASL_HEADERS_DIR}/jhoyt/asl/*.hpp")
file(GLOB ASL_DETAIL_HEADERS "${ASL_SOURCE_DIR}/detail/*.hpp")

set(ASL_ALL_HEADERS ${ASL_HEADERS} ${ASL_DETAIL_HEADERS})

source_group("Header Files\\asl" FILES ${ASL_HEADERS})
source_group("Header Files\\asl\\details" FILES ${ASL_DETAIL_HEADERS})
