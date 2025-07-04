//
// basic_writable_pipe.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_BASIC_WRITABLE_PIPE_HPP
#define BOOST_ASIO_BASIC_WRITABLE_PIPE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_HAS_PIPE) \
  || defined(GENERATING_DOCUMENTATION)

#include <string>
#include <utility>
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/detail/handler_type_requirements.hpp>
#include <boost/asio/detail/io_object_impl.hpp>
#include <boost/asio/detail/non_const_lvalue.hpp>
#include <boost/asio/detail/throw_error.hpp>
#include <boost/asio/detail/type_traits.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/execution_context.hpp>
#if defined(BOOST_ASIO_HAS_IOCP)
# include <boost/asio/detail/win_iocp_handle_service.hpp>
#elif defined(BOOST_ASIO_HAS_IO_URING_AS_DEFAULT)
# include <boost/asio/detail/io_uring_descriptor_service.hpp>
#else
# include <boost/asio/detail/reactive_descriptor_service.hpp>
#endif

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {

/// Provides pipe functionality.
/**
 * The basic_writable_pipe class provides a wrapper over pipe
 * functionality.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Unsafe.
 */
template <typename Executor = any_io_executor>
class basic_writable_pipe
{
private:
  class initiate_async_write_some;

public:
  /// The type of the executor associated with the object.
  typedef Executor executor_type;

  /// Rebinds the pipe type to another executor.
  template <typename Executor1>
  struct rebind_executor
  {
    /// The pipe type when rebound to the specified executor.
    typedef basic_writable_pipe<Executor1> other;
  };

  /// The native representation of a pipe.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined native_handle_type;
#elif defined(BOOST_ASIO_HAS_IOCP)
  typedef detail::win_iocp_handle_service::native_handle_type
    native_handle_type;
#elif defined(BOOST_ASIO_HAS_IO_URING_AS_DEFAULT)
  typedef detail::io_uring_descriptor_service::native_handle_type
    native_handle_type;
#else
  typedef detail::reactive_descriptor_service::native_handle_type
    native_handle_type;
#endif

  /// A basic_writable_pipe is always the lowest layer.
  typedef basic_writable_pipe lowest_layer_type;

  /// Construct a basic_writable_pipe without opening it.
  /**
   * This constructor creates a pipe without opening it.
   *
   * @param ex The I/O executor that the pipe will use, by default, to dispatch
   * handlers for any asynchronous operations performed on the pipe.
   */
  explicit basic_writable_pipe(const executor_type& ex)
    : impl_(0, ex)
  {
  }

  /// Construct a basic_writable_pipe without opening it.
  /**
   * This constructor creates a pipe without opening it.
   *
   * @param context An execution context which provides the I/O executor that
   * the pipe will use, by default, to dispatch handlers for any asynchronous
   * operations performed on the pipe.
   */
  template <typename ExecutionContext>
  explicit basic_writable_pipe(ExecutionContext& context,
      constraint_t<
        is_convertible<ExecutionContext&, execution_context&>::value,
        defaulted_constraint
      > = defaulted_constraint())
    : impl_(0, 0, context)
  {
  }

  /// Construct a basic_writable_pipe on an existing native pipe.
  /**
   * This constructor creates a pipe object to hold an existing native
   * pipe.
   *
   * @param ex The I/O executor that the pipe will use, by default, to
   * dispatch handlers for any asynchronous operations performed on the
   * pipe.
   *
   * @param native_pipe A native pipe.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  basic_writable_pipe(const executor_type& ex,
      const native_handle_type& native_pipe)
    : impl_(0, ex)
  {
    boost::system::error_code ec;
    impl_.get_service().assign(impl_.get_implementation(),
        native_pipe, ec);
    boost::asio::detail::throw_error(ec, "assign");
  }

  /// Construct a basic_writable_pipe on an existing native pipe.
  /**
   * This constructor creates a pipe object to hold an existing native
   * pipe.
   *
   * @param context An execution context which provides the I/O executor that
   * the pipe will use, by default, to dispatch handlers for any
   * asynchronous operations performed on the pipe.
   *
   * @param native_pipe A native pipe.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  template <typename ExecutionContext>
  basic_writable_pipe(ExecutionContext& context,
      const native_handle_type& native_pipe,
      constraint_t<
        is_convertible<ExecutionContext&, execution_context&>::value
      > = 0)
    : impl_(0, 0, context)
  {
    boost::system::error_code ec;
    impl_.get_service().assign(impl_.get_implementation(),
        native_pipe, ec);
    boost::asio::detail::throw_error(ec, "assign");
  }

  /// Move-construct a basic_writable_pipe from another.
  /**
   * This constructor moves a pipe from one object to another.
   *
   * @param other The other basic_writable_pipe object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_writable_pipe(const executor_type&)
   * constructor.
   */
  basic_writable_pipe(basic_writable_pipe&& other)
    : impl_(std::move(other.impl_))
  {
  }

  /// Move-assign a basic_writable_pipe from another.
  /**
   * This assignment operator moves a pipe from one object to another.
   *
   * @param other The other basic_writable_pipe object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_writable_pipe(const executor_type&)
   * constructor.
   */
  basic_writable_pipe& operator=(basic_writable_pipe&& other)
  {
    impl_ = std::move(other.impl_);
    return *this;
  }

  // All pipes have access to each other's implementations.
  template <typename Executor1>
  friend class basic_writable_pipe;

  /// Move-construct a basic_writable_pipe from a pipe of another executor type.
  /**
   * This constructor moves a pipe from one object to another.
   *
   * @param other The other basic_writable_pipe object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_writable_pipe(const executor_type&)
   * constructor.
   */
  template <typename Executor1>
  basic_writable_pipe(basic_writable_pipe<Executor1>&& other,
      constraint_t<
        is_convertible<Executor1, Executor>::value,
        defaulted_constraint
      > = defaulted_constraint())
    : impl_(std::move(other.impl_))
  {
  }

  /// Move-assign a basic_writable_pipe from a pipe of another executor type.
  /**
   * This assignment operator moves a pipe from one object to another.
   *
   * @param other The other basic_writable_pipe object from which the move will
   * occur.
   *
   * @note Following the move, the moved-from object is in the same state as if
   * constructed using the @c basic_writable_pipe(const executor_type&)
   * constructor.
   */
  template <typename Executor1>
  constraint_t<
    is_convertible<Executor1, Executor>::value,
    basic_writable_pipe&
  > operator=(basic_writable_pipe<Executor1>&& other)
  {
    basic_writable_pipe tmp(std::move(other));
    impl_ = std::move(tmp.impl_);
    return *this;
  }

  /// Destroys the pipe.
  /**
   * This function destroys the pipe, cancelling any outstanding
   * asynchronous wait operations associated with the pipe as if by
   * calling @c cancel.
   */
  ~basic_writable_pipe()
  {
  }

  /// Get the executor associated with the object.
  const executor_type& get_executor() noexcept
  {
    return impl_.get_executor();
  }

  /// Get a reference to the lowest layer.
  /**
   * This function returns a reference to the lowest layer in a stack of
   * layers. Since a basic_writable_pipe cannot contain any further layers, it
   * simply returns a reference to itself.
   *
   * @return A reference to the lowest layer in the stack of layers. Ownership
   * is not transferred to the caller.
   */
  lowest_layer_type& lowest_layer()
  {
    return *this;
  }

  /// Get a const reference to the lowest layer.
  /**
   * This function returns a const reference to the lowest layer in a stack of
   * layers. Since a basic_writable_pipe cannot contain any further layers, it
   * simply returns a reference to itself.
   *
   * @return A const reference to the lowest layer in the stack of layers.
   * Ownership is not transferred to the caller.
   */
  const lowest_layer_type& lowest_layer() const
  {
    return *this;
  }

  /// Assign an existing native pipe to the pipe.
  /*
   * This function opens the pipe to hold an existing native pipe.
   *
   * @param native_pipe A native pipe.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  void assign(const native_handle_type& native_pipe)
  {
    boost::system::error_code ec;
    impl_.get_service().assign(impl_.get_implementation(), native_pipe, ec);
    boost::asio::detail::throw_error(ec, "assign");
  }

  /// Assign an existing native pipe to the pipe.
  /*
   * This function opens the pipe to hold an existing native pipe.
   *
   * @param native_pipe A native pipe.
   *
   * @param ec Set to indicate what error occurred, if any.
   */
  BOOST_ASIO_SYNC_OP_VOID assign(const native_handle_type& native_pipe,
      boost::system::error_code& ec)
  {
    impl_.get_service().assign(impl_.get_implementation(), native_pipe, ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Determine whether the pipe is open.
  bool is_open() const
  {
    return impl_.get_service().is_open(impl_.get_implementation());
  }

  /// Close the pipe.
  /**
   * This function is used to close the pipe. Any asynchronous write operations
   * will be cancelled immediately, and will complete with the
   * boost::asio::error::operation_aborted error.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  void close()
  {
    boost::system::error_code ec;
    impl_.get_service().close(impl_.get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "close");
  }

  /// Close the pipe.
  /**
   * This function is used to close the pipe. Any asynchronous write operations
   * will be cancelled immediately, and will complete with the
   * boost::asio::error::operation_aborted error.
   *
   * @param ec Set to indicate what error occurred, if any.
   */
  BOOST_ASIO_SYNC_OP_VOID close(boost::system::error_code& ec)
  {
    impl_.get_service().close(impl_.get_implementation(), ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Release ownership of the underlying native pipe.
  /**
   * This function causes all outstanding asynchronous write operations to
   * finish immediately, and the handlers for cancelled operations will be
   * passed the boost::asio::error::operation_aborted error. Ownership of the
   * native pipe is then transferred to the caller.
   *
   * @throws boost::system::system_error Thrown on failure.
   *
   * @note This function is unsupported on Windows versions prior to Windows
   * 8.1, and will fail with boost::asio::error::operation_not_supported on
   * these platforms.
   */
#if defined(BOOST_ASIO_MSVC) && (BOOST_ASIO_MSVC >= 1400) \
  && (!defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0603)
  __declspec(deprecated("This function always fails with "
        "operation_not_supported when used on Windows versions "
        "prior to Windows 8.1."))
#endif
  native_handle_type release()
  {
    boost::system::error_code ec;
    native_handle_type s = impl_.get_service().release(
        impl_.get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "release");
    return s;
  }

  /// Release ownership of the underlying native pipe.
  /**
   * This function causes all outstanding asynchronous write operations to
   * finish immediately, and the handlers for cancelled operations will be
   * passed the boost::asio::error::operation_aborted error. Ownership of the
   * native pipe is then transferred to the caller.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @note This function is unsupported on Windows versions prior to Windows
   * 8.1, and will fail with boost::asio::error::operation_not_supported on
   * these platforms.
   */
#if defined(BOOST_ASIO_MSVC) && (BOOST_ASIO_MSVC >= 1400) \
  && (!defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0603)
  __declspec(deprecated("This function always fails with "
        "operation_not_supported when used on Windows versions "
        "prior to Windows 8.1."))
#endif
  native_handle_type release(boost::system::error_code& ec)
  {
    return impl_.get_service().release(impl_.get_implementation(), ec);
  }

  /// Get the native pipe representation.
  /**
   * This function may be used to obtain the underlying representation of the
   * pipe. This is intended to allow access to native pipe
   * functionality that is not otherwise provided.
   */
  native_handle_type native_handle()
  {
    return impl_.get_service().native_handle(impl_.get_implementation());
  }

  /// Cancel all asynchronous operations associated with the pipe.
  /**
   * This function causes all outstanding asynchronous write operations to
   * finish immediately, and the handlers for cancelled operations will be
   * passed the boost::asio::error::operation_aborted error.
   *
   * @throws boost::system::system_error Thrown on failure.
   */
  void cancel()
  {
    boost::system::error_code ec;
    impl_.get_service().cancel(impl_.get_implementation(), ec);
    boost::asio::detail::throw_error(ec, "cancel");
  }

  /// Cancel all asynchronous operations associated with the pipe.
  /**
   * This function causes all outstanding asynchronous write operations to
   * finish immediately, and the handlers for cancelled operations will be
   * passed the boost::asio::error::operation_aborted error.
   *
   * @param ec Set to indicate what error occurred, if any.
   */
  BOOST_ASIO_SYNC_OP_VOID cancel(boost::system::error_code& ec)
  {
    impl_.get_service().cancel(impl_.get_implementation(), ec);
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
  }

  /// Write some data to the pipe.
  /**
   * This function is used to write data to the pipe. The function call will
   * block until one or more bytes of the data has been written successfully,
   * or until an error occurs.
   *
   * @param buffers One or more data buffers to be written to the pipe.
   *
   * @returns The number of bytes written.
   *
   * @throws boost::system::system_error Thrown on failure. An error code of
   * boost::asio::error::eof indicates that the connection was closed by the
   * peer.
   *
   * @note The write_some operation may not transmit all of the data to the
   * peer. Consider using the @ref write function if you need to ensure that
   * all data is written before the blocking operation completes.
   *
   * @par Example
   * To write a single data buffer use the @ref buffer function as follows:
   * @code
   * pipe.write_some(boost::asio::buffer(data, size));
   * @endcode
   * See the @ref buffer documentation for information on writing multiple
   * buffers in one go, and how to use it with arrays, boost::array or
   * std::vector.
   */
  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers)
  {
    boost::system::error_code ec;
    std::size_t s = impl_.get_service().write_some(
        impl_.get_implementation(), buffers, ec);
    boost::asio::detail::throw_error(ec, "write_some");
    return s;
  }

  /// Write some data to the pipe.
  /**
   * This function is used to write data to the pipe. The function call will
   * block until one or more bytes of the data has been written successfully,
   * or until an error occurs.
   *
   * @param buffers One or more data buffers to be written to the pipe.
   *
   * @param ec Set to indicate what error occurred, if any.
   *
   * @returns The number of bytes written. Returns 0 if an error occurred.
   *
   * @note The write_some operation may not transmit all of the data to the
   * peer. Consider using the @ref write function if you need to ensure that
   * all data is written before the blocking operation completes.
   */
  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers,
      boost::system::error_code& ec)
  {
    return impl_.get_service().write_some(
        impl_.get_implementation(), buffers, ec);
  }

  /// Start an asynchronous write.
  /**
   * This function is used to asynchronously write data to the pipe. It is an
   * initiating function for an @ref asynchronous_operation, and always returns
   * immediately.
   *
   * @param buffers One or more data buffers to be written to the pipe.
   * Although the buffers object may be copied as necessary, ownership of the
   * underlying memory blocks is retained by the caller, which must guarantee
   * that they remain valid until the completion handler is called.
   *
   * @param token The @ref completion_token that will be used to produce a
   * completion handler, which will be called when the write completes.
   * Potential completion tokens include @ref use_future, @ref use_awaitable,
   * @ref yield_context, or a function object with the correct completion
   * signature. The function signature of the completion handler must be:
   * @code void handler(
   *   const boost::system::error_code& error, // Result of operation.
   *   std::size_t bytes_transferred // Number of bytes written.
   * ); @endcode
   * Regardless of whether the asynchronous operation completes immediately or
   * not, the completion handler will not be invoked from within this function.
   * On immediate completion, invocation of the handler will be performed in a
   * manner equivalent to using boost::asio::async_immediate().
   *
   * @par Completion Signature
   * @code void(boost::system::error_code, std::size_t) @endcode
   *
   * @note The write operation may not transmit all of the data to the peer.
   * Consider using the @ref async_write function if you need to ensure that all
   * data is written before the asynchronous operation completes.
   *
   * @par Example
   * To write a single data buffer use the @ref buffer function as follows:
   * @code
   * pipe.async_write_some(boost::asio::buffer(data, size), handler);
   * @endcode
   * See the @ref buffer documentation for information on writing multiple
   * buffers in one go, and how to use it with arrays, boost::array or
   * std::vector.
   */
  template <typename ConstBufferSequence,
      BOOST_ASIO_COMPLETION_TOKEN_FOR(void (boost::system::error_code,
        std::size_t)) WriteToken = default_completion_token_t<executor_type>>
  auto async_write_some(const ConstBufferSequence& buffers,
      WriteToken&& token = default_completion_token_t<executor_type>())
    -> decltype(
      async_initiate<WriteToken,
        void (boost::system::error_code, std::size_t)>(
          declval<initiate_async_write_some>(), token, buffers))
  {
    return async_initiate<WriteToken,
      void (boost::system::error_code, std::size_t)>(
        initiate_async_write_some(this), token, buffers);
  }

private:
  // Disallow copying and assignment.
  basic_writable_pipe(const basic_writable_pipe&) = delete;
  basic_writable_pipe& operator=(const basic_writable_pipe&) = delete;

  class initiate_async_write_some
  {
  public:
    typedef Executor executor_type;

    explicit initiate_async_write_some(basic_writable_pipe* self)
      : self_(self)
    {
    }

    const executor_type& get_executor() const noexcept
    {
      return self_->get_executor();
    }

    template <typename WriteHandler, typename ConstBufferSequence>
    void operator()(WriteHandler&& handler,
        const ConstBufferSequence& buffers) const
    {
      // If you get an error on the following line it means that your handler
      // does not meet the documented type requirements for a WriteHandler.
      BOOST_ASIO_WRITE_HANDLER_CHECK(WriteHandler, handler) type_check;

      detail::non_const_lvalue<WriteHandler> handler2(handler);
      self_->impl_.get_service().async_write_some(
          self_->impl_.get_implementation(), buffers,
          handler2.value, self_->impl_.get_executor());
    }

  private:
    basic_writable_pipe* self_;
  };

#if defined(BOOST_ASIO_HAS_IOCP)
  detail::io_object_impl<detail::win_iocp_handle_service, Executor> impl_;
#elif defined(BOOST_ASIO_HAS_IO_URING_AS_DEFAULT)
  detail::io_object_impl<detail::io_uring_descriptor_service, Executor> impl_;
#else
  detail::io_object_impl<detail::reactive_descriptor_service, Executor> impl_;
#endif
};

} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // defined(BOOST_ASIO_HAS_PIPE)
       //   || defined(GENERATING_DOCUMENTATION)

#endif // BOOST_ASIO_BASIC_WRITABLE_PIPE_HPP
