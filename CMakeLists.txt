# cmake_minimum_required(VERSION <specify CMake version here>)
project(MyEngineServer)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(src)

add_executable(MyEngineServer
        src/MyEngineServer.cpp src/tcp/MyTcpSocket.h src/tcp/MyTcpSocket.cpp src/MessageManager/MessageManager.h src/MessageManager/MessageManager.cpp src/MessageManager/HandleMessage.cpp src/MyMysql/MyMysql.h src/MyMysql/MyMysql.cpp src/protobuf/MyEngineMessage.pb.cc)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} libmysqlclient.so.21 libprotobuf.so -lpthread)


