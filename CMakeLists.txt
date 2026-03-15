cmake_minimum_required(VERSION 3.10)
project(Lab3_Sockets LANGUAGES CXX)

# Set a modern C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find Boost ---
# Boost.Asio is mostly header-only.
# Boost 1.69.0 made the 'system' component header-only. Boost 1.90.0 removed the compiled library entirely.
find_package(Boost 1.65.0 REQUIRED)
if(Boost_VERSION LESS 106900)
    find_package(Boost 1.65.0 COMPONENTS system REQUIRED)
endif()

# --- Find Threads ---
# Required for std::thread on Linux and macOS
find_package(Threads REQUIRED)

# --- Platform-specific setup ---
set(PLATFORM_LIBS "")
if(WIN32)
    # Specific settings for Windows
    # This ensures that symbols are exported correctly when building DLLs
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    
    # Define _WIN32_WINNT (Windows 7 and up) to avoid Boost.Asio warnings and define Windows leaner compilation
    add_compile_definitions(_WIN32_WINNT=0x0601 WIN32_LEAN_AND_MEAN)
    
    # Windows native socket libraries required by Boost.Asio
    set(PLATFORM_LIBS ws2_32 wsock32)
elseif(APPLE)
    # Specific settings for macOS
    # For example, you might want to create application bundles
    # set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist")
else()
    # Specific settings for Linux and other Unix-like systems
endif()

# --- Include directories ---
# Add Boost include directory
include_directories(${Boost_INCLUDE_DIRS})

# --- Define executables ---
add_executable(server server.cpp)
add_executable(client client.cpp)

# --- Link libraries ---
# Link the server and client executables to the Boost libraries and platform-specific dependencies
target_link_libraries(server PRIVATE ${Boost_LIBRARIES} Threads::Threads ${PLATFORM_LIBS})
target_link_libraries(client PRIVATE ${Boost_LIBRARIES} Threads::Threads ${PLATFORM_LIBS})

# --- Installation ---
# Define where the executables should be installed when the 'install' target is built.
# On Unix-like systems (Linux, macOS), this is often /usr/local/bin
# On Windows, this is often C:\Program Files\<Project Name>\bin
install(TARGETS server client
    RUNTIME DESTINATION bin
)

# --- Build instructions ---
message(STATUS "Project configured for ${CMAKE_SYSTEM_NAME}")
message(STATUS "Boost version: ${Boost_VERSION_STRING}")
message(STATUS "To build the project, you will need to have CMake and the Boost libraries installed.")
message(STATUS "Then, from the project root directory, run the following commands:")
message(STATUS "  mkdir build")
message(STATUS "  cd build")
message(STATUS "  cmake ..")
message(STATUS "  cmake --build .")
message(STATUS "The 'server' and 'client' executables will be in the 'build' directory.")
message(STATUS "To install the executables to a system-wide location, run (may require administrator/root privileges):")
message(STATUS "  cmake --build . --target install")
