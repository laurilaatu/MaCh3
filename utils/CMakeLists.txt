set(HEADERS
  file_helper.h
  root_utils.h
)

add_library(Utils INTERFACE)

set_target_properties(Utils PROPERTIES
  PUBLIC_NAME "${HEADERS}"
  EXPORT_NAME Utils
)

#target_link_libraries(Utils ROOT::ROOT spdlog MaCh3::Manager MaCh3CompilerOptions)

#target_include_directories(Utils INTERFACE
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
#  $<INSTALL_INTERFACE:include>)

install(TARGETS Utils
        EXPORT MaCh3-targets
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/utils)

add_library(MaCh3::Utils ALIAS Utils)
