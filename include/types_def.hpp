#pragma once

#if __cplusplus >= 202002L
#include <vector>  // for std::hash
#include <concepts>
namespace huffman {
template<typename T>
concept HuffmanType = requires(T t) {
  { std::hash<T>{ }(t) }->std::convertible_to<size_t>;
  { t < t }->std::same_as<bool>;
};

template<typename T, typename Y>
concept ReturnOrRef = std::same_as<T, Y>|| std::same_as<T, const Y&>|| std::same_as<T, Y&>;

template<typename T, typename Y>
concept IterConcept = requires(T t) {
  { ++t }->ReturnOrRef<T>;
  { *t }->ReturnOrRef<Y>;
  { t != t }->std::same_as<bool>;
};
}  // namespace huffman

#define HUFFMAN_TYPE huffman::HuffmanType
#define ITER_REQUIREMENT(iter, b) requires huffman::IterConcept<iter, b>
#define HAS_MINUS(a, b) requires { a - b; }
#else
#include <type_traits>
#define HUFFMAN_TYPE typename
#define ITER_REQUIREMENT(iter, b)
namespace huffman {
template<typename T, typename Y, typename = void>
struct HasMinus : std::false_type { };
template<typename T, typename Y>
struct HasMinus<T, Y, std::void_t<decltype(std::declval<T>() - std::declval<Y>())>>: std::true_type { };
}  // namespace huffman
#define HAS_MINUS(a, b) huffman::HasMinus<decltype(a), decltype(b)>::value
#endif
