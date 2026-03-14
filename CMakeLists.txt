cmake_minimum_required(VERSION 3.10)
project(Lab3_Sockets LANGUAGES CXX)

# Set a modern C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Find Boost ---
# Boost.Asio is a header-only library for the most part, but other parts of Boost
# might require compiled libraries. We look for the 'system' component as a good
# practice.
find_package(Boost 1.65.0 COMPONENTS system REQUIRED)

# --- Platform-specific setup ---
if(WIN32)
    # Specific settings for Windows
    # This ensures that symbols are exported correctly when building DLLs,
    # though not strictly necessary for executables.
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
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
# Link the server and client executables to the Boost libraries
target_link_libraries(server PRIVATE ${Boost_LIBRARIES})
target_link_libraries(client PRIVATE ${Boost_LIBRARIES})

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