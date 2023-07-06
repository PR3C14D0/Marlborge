add_executable ("MarlborgeServer" "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MarlborgeServer PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(MarlborgeServer "ws2_32.lib")
