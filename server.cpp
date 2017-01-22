#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// shared_ptr and enable_shared_from_this keeps the connection alive
// as long as there's an operation referring to it.
class tcp_connection
    :  public boost::enable_shared_from_this<tcp_connection> {
  public:
  	typedef boost::shared_ptr<tcp_connection> pointer;

  	static pointer create(boost::asio::io_service& io_service) {
  	  return pointer(new tcp_connection(io_service));
  	}

  	tcp::socket& socket() {
  	  return socket_;
  	}

    // async_write() serves data to the client
  	void start() {
  	  // data meant to be sent is stored in message_
   	  message_ = "put message here";

      boost::asio::async_write(socket_, boost::asio::buffer(message_),
      	  boost::bind(&tcp_connection::handle_write, shared_from_this(),
      	  	  boost::asio::placeholders::error,
      	  	  boost::asio::placeholders::bytes_transferred));
  	}

  private:
  	tcp_connection(boost::asio::io_service& io_service) : socket_(io_service) {}

  	void handle_write(const boost::system::error_code& error,
  	    size_t bytes_transferred) {}

    tcp::socket socket_;
    std::string message_;
};

class tcp_server {
  public:
  	tcp_server (boost::asio::io_service& io_service)
  	    : acceptor_(io_service, tcp::endpoint(tcp::v4(), 13)) {
  	  start_accept();
  	}

  private:
  	// creates a socket and initiates asynchronous accept operation
  	// to wait for a new connection
    void start_accept() {
      tcp_connection::pointer new_connection =
          tcp_connection::create(acceptor_.get_io_service());

      acceptor_.async_accept(new_connection->socket(),
      	boost::bind(&tcp_server::handle_accept, this, new_connection,
      	  boost::asio::placeholders::error));
    }

    void handle_accept(tcp_connection::pointer new_connection,
        const boost::system::error_code& error) {
      if (!error) {
      	new_connection->start();
      }

      start_accept();
    }

    tcp::acceptor acceptor_;
};


// Main function to run server
int main() {
  // Creates server object to accept incoming client connections
  try {
    boost::asio::io_service io_service;
    tcp_server server(io_service);
    io_service.run();
  } catch (std::exception& e) {
  	std::cerr << e.what() << std::endl;
  }

  return 0;
}