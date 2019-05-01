
#include "main.hh"

#include <misc/addrinfo/addrinfo.hh>
#include <vector>

#include "events/timer.hh"
#include "socket/ssl-socket.hh"

namespace http
{
    // Dispatcher is routing the requests to the corresponding vhost
    http::Dispatcher dispatcher = http::Dispatcher();

    void init_ssl()
    {
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        SSL_library_init();
    }

    void init_vhost(http::ServerConfig config)
    {
        // fills the dispatcher with the vhosts
        for (unsigned i = 0; i < config.vhosts_.size(); i++)
        {
            dispatcher.add_vhost(config.vhosts_.at(i));
        }
    }

    void
    init_listeners(std::vector<std::shared_ptr<http::ListenerEW>>& listeners,
                   http::ServerConfig config, int i)
    {
        addrinfo info = {};
        info.ai_family = PF_UNSPEC;
        info.ai_socktype = SOCK_STREAM;
        info.ai_protocol = IPPROTO_TCP;
        info.ai_flags = AI_PASSIVE;
        addrinfo* result;
        getaddrinfo(config.vhosts_.at(i).ip_.c_str(),
                    std::to_string(config.vhosts_.at(i).port_).c_str(), &info,
                    &result);
        for (addrinfo* addr = result; addr != NULL; addr = addr->ai_next)
        {
            http::DefaultSocket server_socket = http::DefaultSocket(
                addr->ai_family, addr->ai_socktype, addr->ai_protocol);
            std::shared_ptr<DefaultSocket> sock =
                std::make_shared<DefaultSocket>(server_socket.fd_get());
            sock->bind(addr->ai_addr, addr->ai_addrlen);
            sock->listen(3000);
            sock->ipv6_set(server_socket.is_ipv6());
            std::shared_ptr<ListenerEW> listener =
                event_register.register_ew<ListenerEW, shared_socket>(
                    sock, std::forward<TimeoutConfig>(config.timeoutConf_));
            listeners.emplace_back(listener);
        }
        freeaddrinfo(result);
    }

    void end_loop(std::vector<std::shared_ptr<http::ListenerEW>>& listeners)
    {
        for (unsigned i = 0; i < listeners.size(); i++)
        {
            event_register.unregister_ew(listeners.at(i).get());
        }
        ERR_free_strings();
        EVP_cleanup();
    }
    int start_server(std::string arg)
    {
        try
        {
            init_ssl();

            http::ServerConfig config = http::parse_configuration(arg);
            init_vhost(config);

            // Check the integrity of the vhosts stored in dispatcher
            if (dispatcher.check_integrity())
                return 1;

            std::vector<std::shared_ptr<http::ListenerEW>> listeners =
                std::vector<std::shared_ptr<http::ListenerEW>>();

            for (unsigned i = 0; i < config.vhosts_.size(); i++)
            {
                init_listeners(listeners, config, i);
            }

            // run loop
            http::EventLoop event_loop = event_register.loop_get();
            event_loop();

            end_loop(listeners);

        } catch (const std::exception& e)
        {
            std::cerr << e.what() << "\n";
            return 1;
        }
        return 0;
    }

} // namespace http

int main(int argc, char* argv[])
{
    try
    {
        if (argc == 2)
            return http::start_server(argv[1]);
        else if (argc == 3)
        {
            if (argv[1][0] == '-' && argv[1][1] == 't')
            {
                try
                {
                    http::parse_configuration(argv[2]);
                    return 0;
                } catch (const std::exception& e)
                {
                    return 1;
                }
            }
            return 1;
        }
        return 1;
    } catch (const std::exception& e)
    {
        return 1;
    }
}