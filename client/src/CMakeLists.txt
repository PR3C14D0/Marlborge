add_executable ("MarlborgeInjector" "main.cpp")
add_library ("MarlborgeDLL" SHARED "dllmain.cpp" "public/HTTPClient.h" "private/HTTPClient.cpp" "public/Util.h" "public/Methods/HTTP.h" "private/Methods/HTTP.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MarlborgeInjector PROPERTY CXX_STANDARD 20)
  set_property(TARGET MarlborgeDLL PROPERTY CXX_STANDARD 20)
endif()

set("PRIVATE_INCLUDES" "${CMAKE_CURRENT_SOURCE_DIR}/public")

target_link_libraries(MarlborgeDLL "ws2_32.lib")
target_include_directories(MarlborgeDLL PRIVATE "${PRIVATE_INCLUDES}")
