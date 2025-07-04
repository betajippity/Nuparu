//
// impl/cancel_at.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_IMPL_CANCEL_AT_HPP
#define BOOST_ASIO_IMPL_CANCEL_AT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>
#include <boost/asio/associated_executor.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/detail/initiation_base.hpp>
#include <boost/asio/detail/timed_cancel_op.hpp>

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace detail {

template <typename Initiation, typename Clock,
    typename WaitTraits, typename... Signatures>
struct initiate_cancel_at : initiation_base<Initiation>
{
  using initiation_base<Initiation>::initiation_base;

  template <typename Handler, typename Duration, typename... Args>
  void operator()(Handler&& handler,
      const chrono::time_point<Clock, Duration>& expiry,
      cancellation_type_t cancel_type, Args&&... args) &&
  {
    using op = detail::timed_cancel_op<decay_t<Handler>,
        basic_waitable_timer<Clock, WaitTraits>, Signatures...>;

    non_const_lvalue<Handler> handler2(handler);
    typename op::ptr p = { boost::asio::detail::addressof(handler2.value),
        op::ptr::allocate(handler2.value), 0 };
    p.p = new (p.v) op(handler2.value,
        basic_waitable_timer<Clock, WaitTraits,
          typename Initiation::executor_type>(this->get_executor(), expiry),
        cancel_type);

    op* o = p.p;
    p.v = p.p = 0;
    o->start(static_cast<Initiation&&>(*this), static_cast<Args&&>(args)...);
  }

  template <typename Handler, typename Duration, typename... Args>
  void operator()(Handler&& handler,
      const chrono::time_point<Clock, Duration>& expiry,
      cancellation_type_t cancel_type, Args&&... args) const &
  {
    using op = detail::timed_cancel_op<decay_t<Handler>,
        basic_waitable_timer<Clock, WaitTraits>, Signatures...>;

    non_const_lvalue<Handler> handler2(handler);
    typename op::ptr p = { boost::asio::detail::addressof(handler2.value),
        op::ptr::allocate(handler2.value), 0 };
    p.p = new (p.v) op(handler2.value,
        basic_waitable_timer<Clock, WaitTraits,
          typename Initiation::executor_type>(this->get_executor(), expiry),
        cancel_type);

    op* o = p.p;
    p.v = p.p = 0;
    o->start(static_cast<const Initiation&>(*this),
        static_cast<Args&&>(args)...);
  }
};

template <typename Initiation, typename Clock,
    typename WaitTraits, typename Executor, typename... Signatures>
struct initiate_cancel_at_timer : initiation_base<Initiation>
{
  using initiation_base<Initiation>::initiation_base;

  template <typename Handler, typename Duration, typename... Args>
  void operator()(Handler&& handler,
      basic_waitable_timer<Clock, WaitTraits, Executor>* timer,
      const chrono::time_point<Clock, Duration>& expiry,
      cancellation_type_t cancel_type, Args&&... args) &&
  {
    using op = detail::timed_cancel_op<decay_t<Handler>,
        basic_waitable_timer<Clock, WaitTraits, Executor>&, Signatures...>;

    non_const_lvalue<Handler> handler2(handler);
    typename op::ptr p = { boost::asio::detail::addressof(handler2.value),
        op::ptr::allocate(handler2.value), 0 };
    timer->expires_at(expiry);
    p.p = new (p.v) op(handler2.value, *timer, cancel_type);

    op* o = p.p;
    p.v = p.p = 0;
    o->start(static_cast<Initiation&&>(*this), static_cast<Args&&>(args)...);
  }

  template <typename Handler, typename Duration, typename... Args>
  void operator()(Handler&& handler,
      basic_waitable_timer<Clock, WaitTraits, Executor>* timer,
      const chrono::time_point<Clock, Duration>& expiry,
      cancellation_type_t cancel_type, Args&&... args) const &
  {
    using op = detail::timed_cancel_op<decay_t<Handler>,
        basic_waitable_timer<Clock, WaitTraits, Executor>&, Signatures...>;

    non_const_lvalue<Handler> handler2(handler);
    typename op::ptr p = { boost::asio::detail::addressof(handler2.value),
        op::ptr::allocate(handler2.value), 0 };
    timer->expires_at(expiry);
    p.p = new (p.v) op(handler2.value, *timer, cancel_type);

    op* o = p.p;
    p.v = p.p = 0;
    o->start(static_cast<const Initiation&>(*this),
        static_cast<Args&&>(args)...);
  }
};

} // namespace detail

#if !defined(GENERATING_DOCUMENTATION)

template <typename CompletionToken, typename Clock,
    typename WaitTraits, typename... Signatures>
struct async_result<
    cancel_at_t<CompletionToken, Clock, WaitTraits>, Signatures...>
  : async_result<CompletionToken, Signatures...>
{
  template <typename Initiation, typename RawCompletionToken, typename... Args>
  static auto initiate(Initiation&& initiation,
      RawCompletionToken&& token, Args&&... args)
    -> decltype(
      async_initiate<
        conditional_t<
          is_const<remove_reference_t<RawCompletionToken>>::value,
            const CompletionToken, CompletionToken>,
        Signatures...>(
          declval<detail::initiate_cancel_at<
            decay_t<Initiation>, Clock, WaitTraits, Signatures...>>(),
          token.token_, token.expiry_, token.cancel_type_,
          static_cast<Args&&>(args)...))
  {
    return async_initiate<
      conditional_t<
        is_const<remove_reference_t<RawCompletionToken>>::value,
          const CompletionToken, CompletionToken>,
      Signatures...>(
        detail::initiate_cancel_at<
          decay_t<Initiation>, Clock, WaitTraits, Signatures...>(
            static_cast<Initiation&&>(initiation)),
        token.token_, token.expiry_, token.cancel_type_,
        static_cast<Args&&>(args)...);
  }
};

template <typename CompletionToken, typename Clock,
    typename WaitTraits, typename Executor, typename... Signatures>
struct async_result<
    cancel_at_timer<CompletionToken, Clock, WaitTraits, Executor>,
    Signatures...>
  : async_result<CompletionToken, Signatures...>
{
  template <typename Initiation, typename RawCompletionToken, typename... Args>
  static auto initiate(Initiation&& initiation,
      RawCompletionToken&& token, Args&&... args)
    -> decltype(
      async_initiate<
        conditional_t<
          is_const<remove_reference_t<RawCompletionToken>>::value,
            const CompletionToken, CompletionToken>,
        Signatures...>(
          declval<detail::initiate_cancel_at_timer<
            decay_t<Initiation>, Clock, WaitTraits, Executor, Signatures...>>(),
          token.token_, &token.timer_, token.expiry_, token.cancel_type_,
          static_cast<Args&&>(args)...))
  {
    return async_initiate<
      conditional_t<
        is_const<remove_reference_t<RawCompletionToken>>::value,
          const CompletionToken, CompletionToken>,
      Signatures...>(
        detail::initiate_cancel_at_timer<
          decay_t<Initiation>, Clock, WaitTraits, Executor, Signatures...>(
            static_cast<Initiation&&>(initiation)),
        token.token_, &token.timer_, token.expiry_, token.cancel_type_,
        static_cast<Args&&>(args)...);
  }
};

template <typename Clock, typename WaitTraits, typename... Signatures>
struct async_result<partial_cancel_at<Clock, WaitTraits>, Signatures...>
{
  template <typename Initiation, typename RawCompletionToken, typename... Args>
  static auto initiate(Initiation&& initiation,
      RawCompletionToken&& token, Args&&... args)
    -> decltype(
      async_initiate<
        const cancel_at_t<
          default_completion_token_t<associated_executor_t<Initiation>>,
          Clock, WaitTraits>&,
        Signatures...>(
          static_cast<Initiation&&>(initiation),
          cancel_at_t<
            default_completion_token_t<associated_executor_t<Initiation>>,
            Clock, WaitTraits>(
              default_completion_token_t<associated_executor_t<Initiation>>{},
              token.expiry_, token.cancel_type_),
          static_cast<Args&&>(args)...))
  {
    return async_initiate<
      const cancel_at_t<
        default_completion_token_t<associated_executor_t<Initiation>>,
        Clock, WaitTraits>&,
      Signatures...>(
        static_cast<Initiation&&>(initiation),
        cancel_at_t<
          default_completion_token_t<associated_executor_t<Initiation>>,
          Clock, WaitTraits>(
            default_completion_token_t<associated_executor_t<Initiation>>{},
            token.expiry_, token.cancel_type_),
        static_cast<Args&&>(args)...);
  }
};

template <typename Clock, typename WaitTraits,
    typename Executor, typename... Signatures>
struct async_result<
    partial_cancel_at_timer<Clock, WaitTraits, Executor>, Signatures...>
{
  template <typename Initiation, typename RawCompletionToken, typename... Args>
  static auto initiate(Initiation&& initiation,
      RawCompletionToken&& token, Args&&... args)
    -> decltype(
      async_initiate<Signatures...>(
        static_cast<Initiation&&>(initiation),
        cancel_at_timer<
          default_completion_token_t<associated_executor_t<Initiation>>,
          Clock, WaitTraits, Executor>(
            default_completion_token_t<associated_executor_t<Initiation>>{},
            token.timer_, token.expiry_, token.cancel_type_),
        static_cast<Args&&>(args)...))
  {
    return async_initiate<Signatures...>(
        static_cast<Initiation&&>(initiation),
        cancel_at_timer<
          default_completion_token_t<associated_executor_t<Initiation>>,
          Clock, WaitTraits, Executor>(
            default_completion_token_t<associated_executor_t<Initiation>>{},
            token.timer_, token.expiry_, token.cancel_type_),
        static_cast<Args&&>(args)...);
  }
};

#endif // !defined(GENERATING_DOCUMENTATION)

} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_IMPL_CANCEL_AT_HPP
