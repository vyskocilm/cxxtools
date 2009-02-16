/*
 * Copyright (C) 2004 Marc Boris Duerner
 * Copyright (C) 2005 Aloysius Indrayanto
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
#include "cxxtools/ioerror.h"
#include <string>

namespace cxxtools {

IOError::IOError(const std::string& what, const SourceInfo& si)
: std::ios::failure(what + si)
{
}


IOError::IOError(const char* msg)
: std::ios::failure(msg)
{
}


IOTimeout::IOTimeout()
: IOError("Timeout")
{
}


AccessFailed::AccessFailed(const std::string& resource, const SourceInfo& si)
: IOError("could not access " + resource, si)
{
}


PermissionDenied::PermissionDenied(const std::string& resource, const SourceInfo& si)
: AccessFailed(resource , si)
{
}


DeviceNotFound::DeviceNotFound(const std::string& device, const SourceInfo& si)
: AccessFailed(device, si)
{}



FileNotFound::FileNotFound(const std::string& path, const SourceInfo& si)
: AccessFailed(path, si)
{}


DirectoryNotFound::DirectoryNotFound(const std::string& path, const SourceInfo& si)
: AccessFailed(path, si)
{ }


IOPending::IOPending(const std::string& what, const SourceInfo& si)
: IOError(what, si)
{
}


IOPending::IOPending(const char* msg)
: IOError(msg)
{
}
} // namespace cxxtools
