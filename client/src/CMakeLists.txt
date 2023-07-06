add_executable ("MarlborgeInjector" "main.cpp" "dllmain.cpp")
add_library ("MarlborgeDLL" SHARED "dllmain.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MarlborgeInjector PROPERTY CXX_STANDARD 20)
  set_property(TARGET MarlborgeDLL PROPERTY CXX_STANDARD 20)
endif()
