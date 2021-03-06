#pragma once

#include <utility>
#include <type_traits>
#include <future>
#include <variant>
#include <type_traits>

// TODO: Break this up into smaller files.

namespace futar {

template <typename T, typename... Args>
struct first_type {
  using type = T;
};

template <typename T, typename... Args>
using first_type_t = typename first_type<T, Args...>::type;

template <typename...>
struct pack_includes_impl_;

template <typename T, typename U, typename... Us>
struct pack_includes_impl_<T, U, Us...> {
  static constexpr bool value = std::is_same<T, U>::value || pack_includes_impl_<T, Us...>::value;
};

template <typename T, typename U>
struct pack_includes_impl_<T, U> {
  static constexpr bool value = std::is_same<T, U>::value;
};

template <typename T>
struct pack_includes_impl_<T> {
  static constexpr bool value = false;
};

// check if Ts includes the type T
template <typename T, typename... Ts>
struct pack_includes {
  static constexpr bool value = pack_includes_impl_<T, Ts...>::value;
};

template <typename... Ts>
struct parameter_pack {
  template <template <typename...> typename T>
  struct apply {
    using type = T<Ts...>;
  };

  template <template <typename> typename Fn>
  struct map {
    using type = parameter_pack<Fn<Ts>...>;
  };

  template <typename T>
  struct includes {
    static constexpr bool value = pack_includes<T, Ts...>::value;
  };

  template <typename T>
  struct add {
    using type = parameter_pack<T, Ts...>;
  };
};

template <typename...>
struct remove_duplicates_impl_;

template <typename T>
struct remove_duplicates_impl_<void, T> {
  using type = parameter_pack<T>;
};

// XXX: I have created a pack_includes<T> with value = false
//      so that this works.  It's unclear whether pack_includes<T>,
//      should really exist, but right now.

template <typename T, typename... Ts>
struct remove_duplicates_impl_<std::enable_if_t<!pack_includes<T, Ts...>::value && sizeof...(Ts) >= 1>, T, Ts...> {
  static_assert(sizeof...(Ts) >= 1);
  using type = typename remove_duplicates_impl_<void, Ts...>::type:: template add<T>::type;
};

template <typename T, typename... Ts>
struct remove_duplicates_impl_<std::enable_if_t<pack_includes<T, Ts...>::value && sizeof...(Ts) >= 1>, T, Ts...> {
  static_assert(sizeof...(Ts) >= 1);
  using type = typename remove_duplicates_impl_<void, Ts...>::type;
};

template <typename... Ts>
struct remove_duplicates {
  using type = typename remove_duplicates_impl_<void, Ts...>::type;
};

template <typename T>
struct is_future {

  template <typename U>
  static constexpr decltype(std::declval<U>().get(), bool())
  test_get(int) {
    return true;
  }

  template <typename U>
  static constexpr bool test_get(...) {
    return false;
  }

  template <typename U>
  static constexpr
    std::enable_if_t<std::is_same<decltype(std::declval<U>().wait_for(std::chrono::seconds(0))),
                                  std::future_status>::value, bool>
  test_wait_for(int) {
    return true;
  }

  template <typename U>
  static constexpr bool test_wait_for(...) {
    return false;
  }

  using future_type = std::remove_cv_t<std::remove_reference_t<T>>;

  static constexpr bool value = test_get<future_type>(int()) && test_wait_for<future_type>(int());
};

template <typename T, typename Enabler = void>
struct decay_future {
  using type = T;
};

template <typename T>
struct decay_future<T, std::enable_if_t<is_future<T>::value>> {
  using type = decltype(std::declval<T>().get());
};

template <typename T>
using decay_future_t = typename decay_future<T>::type;

template <typename T, typename Enabler = void>
struct decay_chain {
  using type = T;
};

template <typename T>
struct decay_chain<T, std::enable_if_t<is_future<T>::value>> {
  using type = typename decay_chain<decltype(std::declval<T>().get())>::type;
};

template <typename T>
using decay_chain_t = typename decay_chain<T>::type;

template <typename T, typename Enabler = void>
struct chain_types_impl_;

template <typename T>
struct chain_types_impl_<T, std::enable_if_t<is_future<T>::value>> {
  using type = typename chain_types_impl_<decltype(std::declval<T>().get()), void>::type:: template add<T>::type;
};

template <typename T>
struct chain_types_impl_<T, std::enable_if_t<!is_future<T>::value>> {
  using type = parameter_pack<T>;
};

template <typename T>
struct chain_types {
  using type = typename chain_types_impl_<T, void>::type:: template map<std::remove_reference_t>::type;
};

template <typename T>
using chain_types_t = typename chain_types<T>::type;

// XXX: for a future chain T, returns a type std::variant<Ts...>
//      where Ts is a parameter pack containing

template <typename T>
using chain_variant_t = typename chain_types_t<T>:: template apply<remove_duplicates>::type::type:: template apply<std::variant>::type;

} // end futar
