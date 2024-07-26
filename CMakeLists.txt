cmake_minimum_required(VERSION 3.12)
project(cdc)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS filesystem program_options system)

find_library(SQLITE3_LIBRARY sqlite3)

set(SOURCES
    Broker/broker.cpp
    Network/network.cpp
    Drivers/SQLiteDriver/connector.cpp
    Drivers/PostgreSQLDriver/connector.cpp
    # Drivers/MySQLDriver/connector.cpp
    Drivers/ClickHouseDriver/connector.cpp
    main.cpp
    parser.cpp
)

set(HEADERS
    Logger/Logger.h
    Broker/broker.h
    Network/network.h
    Drivers/baseConnector.h
    Drivers/SQLiteDriver/connector.h
    Drivers/PostgreSQLDriver/connector.h
    # Drivers/MySQLDriver/connector.h
    Drivers/ClickHouseDriver/connector.h
    Sub/tripl.h
    consumer.h
    parser.h
)

set(mysql-concpp_DIR "/home/bararide/mysql-connector-c++-9.0.0-linux-glibc2.28-x86-64bit")
find_package(mysql-concpp)

add_subdirectory(contribs/clickhouse-cpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_options(${PROJECT_NAME} PRIVATE -I/usr/pgsql-11/include -I/usr/include)

target_link_directories(${PROJECT_NAME} PRIVATE
    /home/bararide/cppkafka/build/src/lib64
    /usr/pgsql-11/lib
    /usr/local/lib
    /usr/lib
    ${mysql-concpp_DIR}
)

target_include_directories(${PROJECT_NAME} PRIVATE contribs/clickhouse-cpp/ contribs/clickhouse-cpp/contrib/absl)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pq
    cppkafka
    sqlparser
    ${Boost_LIBRARIES}
    Boost::system
    ${SQLITE3_LIBRARY}
    clickhouse-cpp-lib
)

include_directories(
    /home/bararide/.pgx/11.22/pgx-install/include
    /home/bararide/mysql-connector-c++-9.0.0-linux-glibc2.28-x86-64bit/include
)

link_directories(
    /usr/lib64
    /usr/lib
    ${mysql-concpp_DIR}
)