#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

// shared_ptr and enable_shared_from_this keeps the Connection alive
// as long as there's an operation referring to it.
class Connection
    :  public boost::enable_shared_from_this<Connection> {
  public:
    typedef boost::shared_ptr<Connection> pointer;
    static pointer create(boost::asio::io_service& io_service);
    boost::asio::ip::tcp::socket& socket();
    void start();

  private:
    Connection(boost::asio::io_service& io_service) : socket_(io_service) {}
    void receive();
    void send(std::size_t length);

    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

#endif
