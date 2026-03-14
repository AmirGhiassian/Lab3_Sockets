//
// server.cpp
// ~~~~~~~~~~
//
// Author: Amir Ghiassian
// Date: 03/13/2026
// Course: CSC414
//
// A multi-threaded TCP server that checks if a string is a palindrome.

#include <boost/asio.hpp>
#include <cctype>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using boost::asio::ip::tcp;

// Function to check if a string is a palindrome, ignoring case, spaces, and
// punctuation
bool is_palindrome(const std::string &s) {
  std::string normalized;
  for (char c : s) {
    if (std::isalnum(c)) {
      normalized += std::tolower(c);
    }
  }

  if (normalized.empty()) {
    return true; // An empty string is considered a palindrome
  }

  int left = 0;
  int right = normalized.length() - 1;
  while (left < right) {
    if (normalized[left] != normalized[right]) {
      return false;
    }
    left++;
    right--;
  }
  return true;
}

// Handles a single client session
void session(tcp::socket sock) {
  std::string client_ip = sock.remote_endpoint().address().to_string();
  unsigned short client_port = sock.remote_endpoint().port();
  std::cout << "Accepted connection from: " << client_ip << ":" << client_port
            << std::endl;

  try {
    for (;;) {
      boost::asio::streambuf buf;
      boost::system::error_code error;

      // Read data from the client until a newline is encountered
      size_t len = boost::asio::read_until(sock, buf, "\n", error);

      if (error == boost::asio::error::eof) {
        // Connection closed cleanly by peer.
        std::cout << "Client " << client_ip << ":" << client_port
                  << " disconnected." << std::endl;
        break;
      } else if (error) {
        throw boost::system::system_error(error); // Some other error.
      }

      std::istream is(&buf);
      std::string line;
      std::getline(is, line);

      // Remove potential carriage return
      if (!line.empty() && line.back() == '\r') {
        line.pop_back();
      }

      // If the client sends an empty string, close the connection
      if (line.empty()) {
        std::cout << "Client " << client_ip << ":" << client_port
                  << " requested termination." << std::endl;
        break;
      }

      std::cout << "Received from " << client_ip << ":" << client_port << ": "
                << line << std::endl;

      // Check if the string is a palindrome and prepare the response
      std::string response =
          is_palindrome(line) ? "palindrome" : "not a palindrome";
      response += "\n";

      // Log the response being sent
      std::cout << "Sending to " << client_ip << ":" << client_port << ": "
                << response;

      // Send the response back to the client
      boost::asio::write(sock, boost::asio::buffer(response));
    }
  } catch (std::exception &e) {
    std::cerr << "Exception in thread for " << client_ip << ":" << client_port
              << ": " << e.what() << std::endl;
  }
}

// Main server function
void server(boost::asio::io_context &io_context, unsigned short port) {
  tcp::acceptor a(io_context, tcp::endpoint(boost::asio::ip::tcp::v4(), port));
  std::cout << "Server started on port " << port
            << ". Waiting for connections..." << std::endl;
  for (;;) {
    // Wait for a new connection
    tcp::socket sock(io_context);
    a.accept(sock);
    // Create a new thread to handle the client session
    std::thread(session, std::move(sock)).detach();
  }
}

int main(int argc, char *argv[]) {
  unsigned short port = 50504; // Default port
  if (argc > 1) {
    // If a port is provided on the command line, use it
    int p = std::atoi(argv[1]);
    if (p > 1024 && p < 65536) {
      port = p;
    } else {
      std::cerr << "Invalid port number '" << argv[1]
                << "'. Must be between 1025 and 65535." << std::endl;
      return 1;
    }
  }

  try {
    boost::asio::io_context io_context;
    server(io_context, port);
  } catch (const boost::system::system_error &e) {
    if (e.code() == boost::asio::error::address_in_use) {
      std::cerr << "Error: Port " << port << " is already in use." << std::endl;
      std::cerr << "Please choose a different port or stop the application "
                   "that is using it."
                << std::endl;
    } else {
      std::cerr << "An unexpected system error occurred: " << e.what()
                << std::endl;
    }
  } catch (std::exception &e) {
    std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
  }

  return 0;
}
