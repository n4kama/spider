/**
 * \file socket/socket.hh
 * \brief Socket declaration.
 */

#pragma once

#include <config/config.hh>
#include <memory>
#include <sys/socket.h>

#include "misc/fd.hh"

namespace http
{
    /**
     * \struct Socket
     * \brief Value object representing a socket.
     *
     * socket(7)
     */
    struct Socket
    {
        /**
         * \brief Create a Socket from a fd.
         */
        explicit Socket(const misc::shared_fd&);

        Socket() = default;
        Socket(const Socket&) = default;
        Socket& operator=(const Socket&) = default;
        Socket(Socket&&) = default;
        Socket& operator=(Socket&&) = default;
        virtual ~Socket() = default;

        /**
         * \brief recv(2).
         */
        virtual ssize_t recv(void* dst, size_t len) = 0;

        /**
         * \brief send(2).
         */
        virtual ssize_t send(const void* src, size_t len) = 0;

        /**
         * \brief sendfile(2).
         */
        virtual ssize_t sendfile(misc::shared_fd&, off_t&, size_t) = 0;

        /**
         * \brief bind(2).
         */
        virtual void bind(const sockaddr* addr, socklen_t addrlen) = 0;

        /**
         * \brief listen(2).
         */
        virtual void listen(int backlog) = 0;

        /**
         * \brief setsockopt(2).
         */
        virtual void setsockopt(int level, int optname, int optval) = 0;

        /**
         * \brief accept(2).
         */
        virtual std::shared_ptr<Socket> accept(sockaddr* addr,
                                               socklen_t* addrlen) = 0;

        /**
         * \brief connect(2).
         */
        virtual void connect(const sockaddr*, socklen_t) = 0;

        const misc::shared_fd fd_get() const noexcept
        {
            return fd_;
        }

        bool is_ipv6() const noexcept
        {
            return ipv6_;
        }

        bool is_ssl() const noexcept
        {
            return is_ssl_;
        }

        void ipv6_set(bool ipv6) noexcept
        {
            ipv6_ = ipv6;
        }

        bool killed() const noexcept
        {
            return killed_;
        }

        void killed_set(bool k) noexcept
        {
            killed_ = k;
        }

        size_t get_recv_data()
        {
            return data_recv_;
        }

        void set_recv_data(size_t i)
        {
            data_recv_ = i;
        }

        void increment_recv_data(size_t i)
        {
            data_recv_ += i;
        }

        virtual int set_non_block() = 0;

    protected:
        /**
         * \brief File descriptor of the socket.
         */
        const misc::shared_fd fd_;
        /**
         * Either ipv4 or ipv6.
         */
        bool killed_ = false;
        bool ipv6_ = false;
        bool is_ssl_ = false;
        std::shared_ptr<http::VHostConfig> vhost_;
        size_t data_recv_ = 0;
    };

    using shared_socket = std::shared_ptr<Socket>;
} // namespace http
