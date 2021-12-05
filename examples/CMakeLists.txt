add_executable(server server.cc) 

target_link_libraries(server my_thread_pool) 

install(TARGETS server DESTINATION examples)
