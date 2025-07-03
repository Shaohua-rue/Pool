add_library( mysqlpool SHARED
    Connection.cpp
    ConnPool.cpp
)

target_link_libraries(mysqlpool PUBLIC
    mysqlclient
    pthread
)