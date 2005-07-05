/* netcat.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include <exception>
#include <iostream>
#include <cxxtools/tcpstream.h>
#include <cxxtools/arg.h>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::arg<const char*> ip(argc, argv, 'i', "0.0.0.0");
    cxxtools::arg<unsigned short> port(argc, argv, 'p', 1234);
    cxxtools::arg<unsigned> bufsize(argc, argv, 'b', 8192);

    // listen to a port
    cxxtools::tcp::Server server(ip, port);

    // accept a connetion
    cxxtools::tcp::iostream worker(server, bufsize);

    // copy to stdout
    std::cout << worker.rdbuf();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

