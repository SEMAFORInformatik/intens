#ifndef __ZHELPERS_HPP_INCLUDED__
#define __ZHELPERS_HPP_INCLUDED__

//  Include a bunch of headers that we will need in the examples

#include <zmq.hpp> // https://github.com/zeromq/cppzmq

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
//  Receive 0MQ string from socket and convert into string
static std::string
s_recv (zmq::socket_t & socket) {

    zmq::message_t message;
#ifdef CPPZMQ_VERSION
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    zmq::recv_result_t nbytes;
#else
    zmq::detail::recv_result_t nbytes;
#endif
    nbytes = socket.recv(message);
#else
    socket.recv(&message);
#endif

    return std::string(static_cast<char*>(message.data()), message.size());
}

//  Convert string to 0MQ string and send to socket
static bool
s_send (zmq::socket_t & socket, const std::string & string) {

    zmq::message_t message(string.size());
    memcpy (message.data(), string.data(), string.size());

#ifdef CPPZMQ_VERSION
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    zmq::recv_result_t nbytes;
#else
    zmq::detail::recv_result_t nbytes;
#endif
    nbytes = socket.send (message, zmq::send_flags::none);
    bool rc;
    try {
      rc = (nbytes.value() >= 0);
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    } catch(const std::bad_optional_access& e) {
#else
    } catch(const std::exception& e) {
#endif
      std::cerr << e.what() << std::endl;
      rc = false;
    }
#else
    bool rc = socket.send (message);
#endif
    return (rc);
}

//  Sends string as 0MQ string, as multipart non-terminal
static bool
s_sendmore (zmq::socket_t & socket, const std::string & string) {

    zmq::message_t message(string.size());
    memcpy (message.data(), string.data(), string.size());

#ifdef CPPZMQ_VERSION
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    zmq::recv_result_t nbytes;
#else
    zmq::detail::recv_result_t nbytes;
#endif
    nbytes = socket.send(message, zmq::send_flags::sndmore);
    bool rc;
    try {
      rc = (nbytes.value() >= 0);
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
    } catch(const std::bad_optional_access& e) {
#else
    } catch(const std::exception& e) {
#endif
      std::cerr << e.what() << std::endl;
      rc = false;
    }
#else
    bool rc = socket.send (message, ZMQ_SNDMORE);
#endif
    return (rc);
}

//  Receives all message parts from socket, prints neatly
//
static void
s_dump (zmq::socket_t & socket)
{
    std::cout << "----------------------------------------" << std::endl;

    while (1) {
        //  Process all parts of the message
        zmq::message_t message;
#ifdef CPPZMQ_VERSION
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
        zmq::recv_result_t nbytes;
#else
        zmq::detail::recv_result_t nbytes;
#endif
        nbytes = socket.recv(message);
#else
        socket.recv(&message);
#endif

        //  Dump the message as text or binary
        int size = message.size();
        std::string data(static_cast<char*>(message.data()), size);

        bool is_text = true;

        int char_nbr;
        unsigned char byte;
        for (char_nbr = 0; char_nbr < size; char_nbr++) {
            byte = data [char_nbr];
            if (byte < 32 || byte > 127)
                is_text = false;
        }
        std::cout << "[" << std::setfill('0') << std::setw(3) << size << "]";
        for (char_nbr = 0; char_nbr < size; char_nbr++) {
            if (is_text)
                std::cout << (char)data [char_nbr];
            else
                std::cout << std::setfill('0') << std::setw(2)
                   << std::hex << (unsigned int) data [char_nbr];
        }
        std::cout << std::endl;

        //  Multipart detection
#if CPPZMQ_VERSION >= ZMQ_MAKE_VERSION(4, 7, 0)
        int more = socket.get(zmq::sockopt::rcvmore);
#else
        int more = 0;           //  Multipart detection
        size_t more_size = sizeof (more);
        socket.getsockopt (ZMQ_RCVMORE, &more, &more_size);
#endif
        if (!more)
            break;              //  Last message part
    }
}



#endif
