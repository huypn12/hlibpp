add_library(my_thread_pool my_thread_pool.cc)

set_target_properties(my_thread_pool PROPERTIES PREFIX "")

install(
    TARGETS my_thread_pool
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
