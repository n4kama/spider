#include "main.hh"
namespace po = boost::program_options;

int dispatch(std::string arg, int debug)
{
    try
    {
        //Init one VHOST
        http::ServerConfig config = http::parse_configuration(arg, debug);
        http::VHostConfig vhost = config.vhosts_.at(0);

        //init socket
        http::DefaultSocket server_socket = http::DefaultSocket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        int len = sizeof(addr);

        //init address
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(vhost.ip_.c_str());
        addr.sin_port = htons(vhost.port_);

        //bind socket with address
        struct sockaddr *addr_not_in = (struct sockaddr *)&addr;
        server_socket.bind(addr_not_in, len);
        server_socket.listen(30);
        socklen_t *s_len = (socklen_t *)&len;
        while (1)
        {
            http::shared_socket new_s = server_socket.accept(addr_not_in, s_len);
            http::DefaultSocket sock = http::DefaultSocket(new_s->fd_get());
            http::fill_Request(sock);
            // TO COMPLETE
        }
        //DEBUG TESTING:
    }
    catch (const std::exception &e)
    {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    try
    {
        po::options_description opt_desc("Options");
        opt_desc.add_options()("help,h", "Displays options")(",t", po::value<std::string>(),
                                                             "Takes a json as argument and checks its validity")("debug,d", "debug option, prints info");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, opt_desc), vm);
        po::notify(vm);

        int debug = 0;
        if (vm.count("debug"))
            debug = 1;
        if (vm.count("help"))
        {
            std::cout << opt_desc << "\n";
            return 0;
        }
        else if (vm.count("-t"))
        {
            return dispatch(vm["-t"].as<std::string>(), debug);
        }
        else
            return dispatch(argv[1], debug);
    }
    catch (const std::exception &e)
    {
        return 1;
    }
}