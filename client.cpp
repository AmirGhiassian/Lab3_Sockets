//
// client.cpp
// ~~~~~~~~~~
//
// Author: Amir Ghiassian
// Date: 03/13/2026
// Course: CSC414
//
// A TCP client that sends strings to a server to check if they are palindromes.
//

#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

using boost::asio::ip::tcp;

int main(int argc, char *argv[]) {
  try {
    // Default server and port
    std::string server_address = "localhost";
    std::string port = "50504";

    // Parse command line arguments
    if (argc > 1) {
      server_address = argv[1];
    }
    if (argc > 2) {
      port = argv[2];
    }

    // The port 50501 is used if specified
    if (argc > 2 && std::string(argv[2]) == "50501") {
      port = "50501";
    }

    boost::asio::io_context io_context;
    tcp::socket socket(io_context);

    // Loop to handle connection attempts
    for (;;) {
      try {
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(server_address, port);
        boost::asio::connect(socket, endpoints);
        break; // If connection succeeds, exit the loop
      } catch (const boost::system::system_error &e) {
        std::cerr << "Error: Could not connect to the server at "
                  << server_address << ":" << port << "." << std::endl;
        std::cerr
            << "Please ensure the server is running and the address is correct."
            << std::endl;
        std::cout << "Would you like to (r)etry or (e)xit? ";
        std::string choice;
        std::getline(std::cin, choice);
        if (choice == "e" || choice == "E") {
          return 1; // Exit the program
        }
        std::cout << "Retrying connection..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
      }
    }

    std::cout << "Connected to server at " << server_address << ":" << port
              << std::endl;
    std::cout << "Enter a string to check (or press Enter to quit):"
              << std::endl;

    for (;;) {
      // Read a line from the user
      std::string input_line;
      std::cout << "> ";
      std::getline(std::cin, input_line);

      // Send the line to the server
      boost::asio::write(socket, boost::asio::buffer(input_line + "\n"));

      // If the user entered an empty line, terminate the client
      if (input_line.empty()) {
        std::cout << "Client terminating." << std::endl;
        break;
      }

      // Read the response from the server
      boost::asio::streambuf response_buf;
      boost::asio::read_until(socket, response_buf, "\n");

      std::istream response_stream(&response_buf);
      std::string response_line;
      std::getline(response_stream, response_line);

      // Print the server's response
      std::cout << "Server: " << response_line << std::endl;
    }

  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
