add_library(threadpool SHARED
    threadpool.cpp
)
target_link_libraries(threadpool PUBLIC
pthread)