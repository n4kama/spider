/**
 * \file vhost/vhost.hh
 * \brief VHost declaration.
 */

#pragma once

#include <openssl/ssl.h>

#include "config/config.hh"
#include "error/not-implemented.hh"
#include "request/request.hh"
#include "vhost/connection.hh"
#include "config/config.hh"

namespace http
{
    // FIXME: iterator to data remaining from next request.
    using remaining_iterator = int;
    /**
     * \class VHost
     * \brief Abstract class representing a VHost.
     */
    class VHost
    {
    public:
        /**
         * \brief Create a VHost from its configuration.
         *
         * \param conf VHostConfig virtual host configuration.
         */
        explicit VHost(const VHostConfig&);

        VHost() = delete;
        VHost(const VHost&) = delete;
        VHost& operator=(const VHost&) = delete;
        VHost(VHost&&) = delete;
        VHost& operator=(VHost&&) = delete;
        virtual ~VHost() = default;

        /**
         * \brief Send response depending on the VHost type.
         *
         * \param req Request.
         * \param conn Connection.
         * \param begin remaining_iterator of received data.
         * \param end remaining_iterator of received data.
         */
        virtual void respond(const Request&, Connection, remaining_iterator,
                             remaining_iterator) = 0;

        inline const VHostConfig& conf_get() const noexcept
        {
            return conf_;
        }

        VHostConfig get_conf() const
        {
            return conf_;
        }

        SSL_CTX *get_ctx()
        {
            return ssl_ctx_.get();
        }


    private:
        /**
         *  \brief VHost configuration.
         */
        VHostConfig conf_;

        /**
         * \brief VHost's SSL context.
         *
         * From ssl(3):
         *
         * SSL_CTX (SSL Context)
         *   This is the global context structure which is created by a server
         *   or client once per program life-time and which holds mainly default
         *   values for the SSL structures which are later created for the
         *   connections.
         *
         * Warning: with this unique_ptr syntax, you'll need to instanciate the
         * pointer with both a value and a Deleter function.
         */
        std::unique_ptr<SSL_CTX, decltype(SSL_CTX_free)*> ssl_ctx_;
    };
    int sni_cb(SSL* s, int* , http::VHost* arg);
    using shared_vhost = std::shared_ptr<VHost>;
} // namespace http
