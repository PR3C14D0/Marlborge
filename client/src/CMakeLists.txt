add_executable ("MarlborgeClient" "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MarlborgeClient PROPERTY CXX_STANDARD 20)
endif()
