add_executable(spef_check spef_check.cpp spef_actions.cpp)
target_include_directories(spef_check SYSTEM PRIVATE ${PEGTL_INCLUDE_DIRS})
target_link_libraries(spef_check PRIVATE compilation_options sanitizer_options taocpp::pegtl CLI11::CLI11)
install(TARGETS spef_check)

