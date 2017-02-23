# spaceteam

A C++ web server

## Build Server

`$ make`

## Run Server

`$ ./webserver <config>`

## Build and Run Tests

`$ make check`

## Config File Format

The config file must specify:

* which port the server will listen on

    ````
    port <port number>;
    ````
    
* each request handler

    ```
    path <handler URI prefix> <handler name> {
      <handler-specific config statements>
    }
    ```

### Example Config File

```
# This is a comment.

port 2020;  # This is also a comment.

path / StaticHandler {
  root /foo/bar;
}

path /echo EchoHandler {}

# Default response handler if no handlers match.
default NotFoundHandler {}
```

## Contributing Guide

### Source Code Layout

All header files can be found in the `include/` directory. All `.cc` files can be found in either the `src/` directory or the `config_parser/` directory.

Here are the primary classes that make up the Server:

* `Server`
  * top-level class
  * takes a config filename as input
  * uses the `NginxConfigParser` to parse and process the config
  * initializes each request handler
  * creates asynchronous `Connection`s to listen to requests
* `Connection`
  * reads a single request from a socket
  * invokes the matching `RequestHandler`
* `RequestHandler`
  * given a request, will generate the correct response
  * base class that all request handlers must inherit from

Here are the helper classes:

* `Request`
  * parses the raw HTTP request
  * holds the HTTP request data
* `Response`
  * holds the HTTP response data
  * generates a properly formatted HTTP response using the given data
* `NginxConfigParser`
  * parses the config file into an `NginxConfig` object
* `NginxConfig` and `NginxConfigStatement`
  * these hold the config, parsed into tokens

### Adding a New Request Handler

All request handlers must inherit from the `RequestHandler` base class.

To add a new request handler:

1. Create a new class that inherits from `RequestHandler`
2. Implement the pure virtual functions `RequestHandler::Init` and `RequestHandler::HandleRequest` in the new request handler
3. Add the `REGISTER_REQUEST_HANDLER(<handler name>)` macro to the new request handler's header file to register it with the `Server`
4. Add the new request handler to your config file

We provide several request handlers (`EchoHandler`, `StaticHandler`, `NotFoundHandler`, `StatusHandler`) to use as examples for implementing your own request handlers.

## Team Members

* Crystal Hsieh
* Jason Liu
* Ryan Voong

## Based on

* [async TCP daytime server](http://www.boost.org/doc/libs/1_63_0/doc/html/boost_asio/tutorial.html#boost_asio.tutorial.tutdaytime3)
* [async TCP echo server](http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp)
* [HTTP server](http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/examples/cpp11_examples.html#boost_asio.examples.cpp11_examples.http_server)
