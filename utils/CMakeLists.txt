set(HEADERS
  file_helper.h
  root_utils.h
)

add_library(MaCh3Utils INTERFACE)

set_target_properties(MaCh3Utils PROPERTIES
  PUBLIC_NAME "${HEADERS}"
  EXPORT_NAME MaCh3Utils
)

#target_link_libraries(MaCh3Utils ROOT::ROOT spdlog MaCh3::Manager MaCh3CompilerOptions)

#target_include_directories(MaCh3Utils INTERFACE
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../>
#  $<INSTALL_INTERFACE:include>)

install(TARGETS MaCh3Utils
        EXPORT MaCh3-targets
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/utils)

add_library(MaCh3::MaCh3Utils ALIAS MaCh3Utils)
