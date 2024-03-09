add_executable("MarlborgeServer" "main.cpp" "Colors.h" "Util.h")

target_link_libraries(MarlborgeServer "ws2_32.lib")