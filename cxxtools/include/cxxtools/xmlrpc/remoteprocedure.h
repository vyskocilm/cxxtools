/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef cxxtools_xmlrpc_RemoteProcedure_h
#define cxxtools_xmlrpc_RemoteProcedure_h

#include <cxxtools/xmlrpc/api.h>
#include <cxxtools/xmlrpc/client.h>
#include <cxxtools/deserializer.h>
#include <cxxtools/serializer.h>
#include <cxxtools/signal.h>
#include <string>

namespace cxxtools {

namespace xmlrpc {

class Fault;

class CXXTOOLS_XMLRPC_API IRemoteProcedure
{
    friend class Client;

    public:
        IRemoteProcedure(Client& client, const std::string& name)
        : _client(&client)
        , _name(name)
        { }

        IRemoteProcedure(Client& client, const char* name);

        virtual ~IRemoteProcedure()
        { }

        Client* client()
        { return _client; }

        const std::string& name() const
        { return _name; }

        Signal<const Fault&> fault;

    protected:
        virtual void onFinished() = 0;

    private:
        Client* _client;
        std::string _name;
};


template <typename R,
          typename A1 = cxxtools::Void,
          typename A2 = cxxtools::Void>
class RemoteProcedure : public IRemoteProcedure
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : IRemoteProcedure(client, name)
        { }

        ~RemoteProcedure()
        {}

        void begin(const A1& a1, const A2& a2)
        {
            _a1.begin(a1);
            _a2.begin(a2);
            _r.begin(_result);

            ISerializer* argv[2] = { &_a1, &_a2 };
            this->client()->beginCall(_r, *this, argv, 2);
        }

        const R& call(const A1& a1, const A2& a2)
        {
            _a1.begin(a1);
            _a2.begin(a2);
            _r.begin(_result);

            ISerializer* argv[2] = { &_a1, &_a2 };
            this->client()->call(_r, *this, argv, 2);
            return _result;
        }

        const R& operator()(const A1& a1, const A2& a2)
        {
            return this->call(a1, a2);
        }

        const R& result()
        { return _result; }

        Signal<const R&> finished;

    protected:
        void onFinished()
        { finished.send(_result); }

    private:
        R _result;
        Deserializer<R> _r;
        Serializer<A1> _a1;
        Serializer<A2> _a2;
};


template <typename R,
          typename A1>
class RemoteProcedure<R, A1, cxxtools::Void> : public IRemoteProcedure
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : IRemoteProcedure(client, name)
        { }

        ~RemoteProcedure()
        {}

        void begin(const A1& a1)
        {
            _a1.begin(a1);
            _r.begin(_result);

            ISerializer* argv[1] = { &_a1 };
            this->client()->beginCall(_r, *this, argv, 1);
        }

        const R& call(const A1& a1)
        {
            _a1.begin(a1);
            _r.begin(_result);

            ISerializer* argv[1] = { &_a1 };
            this->client()->call(_r, *this, argv, 1);
            return _result;
        }

        const R& operator()(const A1& a1)
        {
            return this->call(a1);
        }

        const R& result()
        { return _result; }

        Signal<const R&> finished;

    protected:
        void onFinished()
        { finished.send(_result); }

    private:
        R _result;
        Deserializer<R> _r;
        Serializer<A1> _a1;
};


template <typename R>
class RemoteProcedure<R, cxxtools::Void, cxxtools::Void> : public IRemoteProcedure
{
    public:
        RemoteProcedure(Client& client, const std::string& name)
        : IRemoteProcedure(client, name)
        { }

        ~RemoteProcedure()
        {}

        void begin()
        {
            _r.begin(_result);

            ISerializer* argv[1] = { 0 };
            this->client()->beginCall(_r, *this, argv, 0);
        }

        const R& call()
        {
            _r.begin(_result);

            ISerializer* argv[1] = { 0 };
            this->client()->call(_r, *this, argv, 0);
            return _result;
        }

        const R& operator()()
        {
            return this->call();
        }

        const R& result()
        { return _result; }

        Signal<const R&> finished;

    protected:
        void onFinished()
        { finished.send(_result); }

    private:
        R _result;
        Deserializer<R> _r;
};

}

}

#endif