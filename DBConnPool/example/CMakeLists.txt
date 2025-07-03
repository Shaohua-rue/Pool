add_executable(main
    main.cpp
)
target_link_libraries(main PRIVATE
    mysqlclient
)

add_executable(connpooltest
    connpooltest.cpp
)
target_link_libraries(connpooltest PRIVATE
    mysqlpool
)