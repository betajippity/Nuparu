//
// detail/exception.hpp
// ~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_EXCEPTION_HPP
#define BOOST_ASIO_DETAIL_EXCEPTION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>
#include <exception>

namespace boost {
namespace asio {

using std::exception_ptr;
using std::current_exception;
using std::rethrow_exception;

} // namespace asio
} // namespace boost

#endif // BOOST_ASIO_DETAIL_EXCEPTION_HPP
