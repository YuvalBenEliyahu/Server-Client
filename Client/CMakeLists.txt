cmake_minimum_required(VERSION 3.27)
project(Client)

set(CMAKE_CXX_STANDARD 14)

# Include directories for Crypto++
include_directories(C:/Users/BEYOVAL/Desktop/Server-Client/cryptopp890)

# Include directories for Boost
include_directories(C:/Users/BEYOVAL/Desktop/Server-Client/boost_1_82_0)

add_executable(Client
        Client/AESWrapper.cpp
        Client/AESWrapper.h
        Client/Base64Wrapper.cpp
        Client/Base64Wrapper.h
        Client/Client.cpp
        Client/main.cpp
        Client/RequestProtocol.h
        Client/RSAWrapper.cpp
        Client/RSAWrapper.h
        Client/SocketHandler.cpp
        Client/SocketHandler.h
        Client/RequestProtocol.cpp)

# Link against Crypto++ library
target_link_libraries(Client C:/Users/BEYOVAL/Desktop/Server-Client/cryptopp890/x64/Output/Debug/cryptlib.lib)

# Link against Boost libraries as needed
target_link_libraries(Client C:/Users/BEYOVAL/Desktop/Server-Client/boost_1_82_0/libboost_whatever.lib)
