#include "socket/ssl-socket.hh"

#include "misc/openssl/ssl.hh"
namespace http
{
    SSLSocket::SSLSocket(int domain, int type, int protocol, SSL_CTX* ssl_ctx)
        : Socket{std::make_shared<misc::FileDescriptor>(
              sys::socket(domain, type, protocol))}
        , ssl_{SSL_new(ssl_ctx), &SSL_free}
    {
        set_non_block();
        SSL_set_fd(ssl_.get(), fd_->fd_);
        ssl::accept(ssl_.get());
        is_ssl_ = true;
    }

    SSLSocket::SSLSocket(const misc::shared_fd& fd, SSL_CTX* ssl_ctx)
        : Socket{fd}
        , ssl_{SSL_new(ssl_ctx), &SSL_free}
    {
        set_non_block();
        SSL_set_fd(ssl_.get(), fd_->fd_);
        ssl::accept(ssl_.get());
        is_ssl_ = true;
    }

    ssize_t SSLSocket::recv(void* dst, size_t)
    {
        SSL_accept(ssl_.get());
        ssl::accept(ssl_.get());
        int r = ssl::read(ssl_.get(), (char*)dst, 4096);
        return r;
    }

    ssize_t SSLSocket::send(const void* dst, size_t len)
    {
        SSL_accept(ssl_.get());
        ssl::accept(ssl_.get());
        int r = ssl::write(ssl_.get(), (char*)dst, len);
        return r;
    }

    ssize_t SSLSocket::sendfile(misc::shared_fd& in_fd, off_t&, size_t)
    {
        char buf[4096];
        int r = 0;
        int nb_read = read(in_fd->fd_, buf, 4096);
		if (nb_read >= 0)
		{
			r = send(buf, (int)nb_read);
		}
        return r;
    }

    void SSLSocket::bind(const sockaddr* addr, socklen_t addrlen)
    {
        sys::bind(*fd_, addr, addrlen);
    }

    void SSLSocket::listen(int backlog)
    {
        sys::listen(*fd_, backlog);
    }

    void SSLSocket::setsockopt(int level, int optname, int optval)
    {
        sys::setsockopt(*fd_, level, optname, &optval, sizeof(int));
    }

    shared_socket SSLSocket::accept(sockaddr* addr, socklen_t* addrlen)
    {
        return std::make_shared<SSLSocket>(
            std::make_shared<misc::FileDescriptor>(
                sys::accept(*fd_, addr, addrlen)),
            nullptr);
    }
    void SSLSocket::connect(const sockaddr* , socklen_t )
    {
        /*
        addr++;
        int okk = l;
        okk++;
        */
        ssl::connect(ssl_.get());
        //sys::connect(*fd_, addr, l);
    }

    int SSLSocket::set_non_block()
    {
        setsockopt(SOL_SOCKET, SO_REUSEPORT, 1);
        setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
        return sys::fcntl_wrapper(fd_->fd_, O_NONBLOCK);
    }
} // namespace http
