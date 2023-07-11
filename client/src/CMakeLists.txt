add_executable ("MarlborgeInjector" "main.cpp")
add_library ("MarlborgeDLL" SHARED "dllmain.cpp" "public/HTTPClient.h" "private/HTTPClient.cpp" "public/Util.h" "public/Methods/HTTP.h" "private/Methods/HTTP.cpp" "public/UserAgents.h" "public/Referers.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MarlborgeInjector PROPERTY CXX_STANDARD 20)
  set_property(TARGET MarlborgeDLL PROPERTY CXX_STANDARD 20)
endif()

set("PRIVATE_INCLUDES" "${CMAKE_CURRENT_SOURCE_DIR}/public")
set("GLOBAL_INCLUDES" "${CMAKE_CURRENT_SOURCE_DIR}/../../includes")

set("LIB" "${CMAKE_CURRENT_SOURCE_DIR}/../../lib")
set("OPENSSL_LIB" "${LIB}/openssl/libssl.lib;${LIB}/openssl/libcrypto.lib")

target_link_libraries(MarlborgeDLL "ws2_32.lib")
target_link_libraries(MarlborgeDLL "${OPENSSL_LIB}")
target_include_directories(MarlborgeDLL PRIVATE "${PRIVATE_INCLUDES}")
target_include_directories(MarlborgeDLL PUBLIC "${GLOBAL_INCLUDES}")
