#include "vhost-static-file.hh"
using namespace http;

VHostStaticFile VHostStaticFile::get_vsf(const VHostConfig& ext_conf)
{
    return VHostStaticFile::VHostStaticFile(ext_conf);
}

VHostStaticFile::VHostStaticFile(const VHostConfig& ext_conf)
: conf_(ext_conf) {}

void VHostStaticFile::respond(const Request& req, Connection conn, remaining_iterator begin,
                     remaining_iterator end)
{
    //En rapport avec connexion.hh qui doit etre coder...

    http::DefaultSocket sock = DefaultSocket(conn.sock_->fd_get());
    conn.sock_->~Socket();
    request_server(req, sock);   //Il faut coder connexion si c'est pas deja fait et cree l'attribut socket_
}