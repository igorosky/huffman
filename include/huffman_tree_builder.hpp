#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include "huffman_tree.hpp"
#include "types_def.hpp"

namespace huffman {
template<HUFFMAN_TYPE T>
class HuffmanTreeBuilder {
  std::unordered_map<T, size_t> _frequencies;

  static std::unordered_map<T, std::vector<bool>> generate_codes(
    const std::vector<std::variant<std::pair<size_t, size_t>, T>>& tree);
  static std::vector<std::variant<std::pair<size_t, size_t>, T>> generate_tree(
    const std::unordered_map<T, std::vector<bool>>& codes);

public:
  std::optional<HuffmanTree<T>> build() const;
  std::optional<HuffmanTree<T>> build_limited_size(uint16_t) const;
  void set_frequencies(const T&, size_t);
  void add_frequency(const T&, size_t);
  template<typename I>
  ITER_REQUIREMENT(I, T)
  void read_text(I, I);
  static std::optional<HuffmanTree<T>> from_sizes(std::vector<std::pair<uint16_t, T>>);
};
}  // namespace huffman


// Implementation

template<HUFFMAN_TYPE T>
void huffman::HuffmanTreeBuilder<T>::set_frequencies(const T& c, size_t frequency) {
  _frequencies[c] = frequency;
}

template<HUFFMAN_TYPE T>
void huffman::HuffmanTreeBuilder<T>::add_frequency(const T& c, size_t frequency) {
  _frequencies[c] += frequency;
}

template<HUFFMAN_TYPE T>
template<typename I>
ITER_REQUIREMENT(I, T)
void huffman::HuffmanTreeBuilder<T>::read_text(I begin, const I end) {
  for (; begin != end; ++begin) {
    add_frequency(*begin, 1);
  }
}

template<typename ... T>
struct VisitingHelper : T ... {
  using T::operator() ...;
};

template<typename ... T>
VisitingHelper(T ...)->VisitingHelper<T...>;

template<HUFFMAN_TYPE T>
std::optional<huffman::HuffmanTree<T>> huffman::HuffmanTreeBuilder<T>::build() const {
  if (_frequencies.empty()) {
    return std::nullopt;
  }
  std::vector<std::variant<std::pair<size_t, size_t>, T>> nodes;
  nodes.reserve(_frequencies.size() * 2);
  std::set<std::pair<size_t, size_t>> tree;
  nodes.push_back(std::make_pair(0, 0));
  for (auto [v, frequency] : _frequencies) {
    if (frequency > 0) {
      nodes.push_back(v);
      tree.insert({ frequency, nodes.size() - 1 });
    }
  }
  while (tree.size() > 1) {
    auto [frequency1, node1] = *tree.begin();
    auto [frequency2, node2] = *(++tree.begin());
    tree.erase(tree.begin(), ++++ tree.begin());
    nodes.push_back(std::make_pair(node1, node2));
    tree.insert({ frequency1 + frequency2, nodes.size() - 1 });
  }
  nodes[0] = nodes[tree.begin()->second];

  return HuffmanTree<T>(nodes, generate_codes(nodes));
}

template<HUFFMAN_TYPE T>
std::optional<huffman::HuffmanTree<T>> huffman::HuffmanTreeBuilder<T>::build_limited_size(uint16_t limit) const {
  struct Coin {
    size_t frequency;
    std::unordered_map<T, uint16_t> values;

    bool operator<(const Coin& other) const {
      return frequency < other.frequency;
    }
  };


  if (_frequencies.empty() || std::pow(2, limit) < static_cast<double>(_frequencies.size())) {
    return std::nullopt;
  }
  if (limit == 0) {  // You really dont need that
    return std::nullopt;
  }
  std::multiset<Coin> initial_set;
  for (auto [v, frequency] : _frequencies) {
    initial_set.insert(Coin{ frequency, { { v, 1 } } });
  }
  std::vector<std::multiset<Coin>> coins(static_cast<size_t>(limit));
  for (size_t i = 1; i < coins.size(); ++i) {
    coins[i] = initial_set;
  }
  coins[0] = std::move(initial_set);

  while (coins.size() > 1) {
    auto last_set = std::move(coins.back());
    coins.pop_back();
    auto b = last_set.begin();
    std::advance(b, 1);
    for (auto a = last_set.begin(); std::distance(a, last_set.end()) > 2; std::advance(a, 2), std::advance(b, 2)) {
      std::unordered_map<T, uint16_t> new_values(a->values);
      for (const auto& [v, count] : b->values) {
        new_values[v] += count;
      }
      coins.back().insert({ a->frequency + b->frequency, std::move(new_values) });
    }
  }

  std::unordered_map<T, uint16_t> counts;
  size_t i = 0;
  const size_t requirement = (_frequencies.size() - 1) * 2;
  for (const auto& coin : coins[0]) {
    for (const auto& [v, count] : coin.values) {
      counts[v] += count;
    }
    if (++i == requirement) {
      break;
    }
  }

  for (auto [v, frequency] : _frequencies) {
    if (counts[v] == 0) {
      counts[v] = limit;
    }
  }

  std::vector<std::pair<uint16_t, T>> counts_vec;
  counts_vec.reserve(counts.size());
  for (auto [v, count] : counts) {
    counts_vec.push_back({ count, v });
  }

  return from_sizes(std::move(counts_vec));
}

template<HUFFMAN_TYPE T>
std::unordered_map<T, std::vector<bool>> huffman::HuffmanTreeBuilder<T>::generate_codes(
  const std::vector<std::variant<std::pair<size_t, size_t>, T>>& tree) {
  std::unordered_map<T, std::vector<bool>> codes;
  std::vector<std::pair<std::vector<bool>, size_t>> dfs;
  dfs.push_back({ { }, 0 });
  while (!dfs.empty()) {
    auto [code, p] = std::move(dfs.back());
    dfs.pop_back();
    std::visit(VisitingHelper{
      [&dfs, &code](std::pair<size_t, size_t> v) {
        auto [p, q] = v;
        code.push_back(false);
        dfs.push_back({ code, p });
        code.back() = true;
        dfs.push_back({ std::move(code), q });
      },
      [&codes, &code](T c) {
        codes[c] = std::move(code);
      },
  }, tree[p]);
  }
  return codes;
}

template<HUFFMAN_TYPE T>
std::optional<huffman::HuffmanTree<T>> huffman::HuffmanTreeBuilder<T>::from_sizes(
  std::vector<std::pair<uint16_t, T>> sizes) {
  if (sizes.empty()) {
    return std::nullopt;
  }
  std::sort(sizes.begin(), sizes.end());
  std::vector<bool> current_code;
  current_code.reserve(sizes.back().first);
  std::unordered_map<T, std::vector<bool>> codes;

  bool valid = true;
  for (auto [count, v] : sizes) {
    if (!valid) {
      return std::nullopt;
    }
    if (count > current_code.size()) {
      current_code.resize(count, false);
    }
    codes.insert({ v, current_code });

    auto p = current_code.size() - 1;
    while (current_code[p]) {
      if (p == 0) {
        valid = false;
        break;
      }
      current_code[p] = false;
      --p;
    }
    current_code[p] = true;
  }

  return huffman::HuffmanTree<T>(generate_tree(codes), codes);
}

template<HUFFMAN_TYPE T>
std::vector<std::variant<std::pair<size_t, size_t>, T>> huffman::HuffmanTreeBuilder<T>::generate_tree(
  const std::unordered_map<T, std::vector<bool>>& codes) {
  std::unordered_map<std::vector<bool>, T> code_to_val;
  for (const auto& [v, code] : codes) {
    code_to_val.insert({ code, v });
  }
  std::vector<std::variant<std::pair<size_t, size_t>, T>> ans;
  ans.resize(codes.size() * 2 - 1, std::make_pair(0, 0));
  std::vector<std::pair<size_t, std::vector<bool>>> dfs;
  dfs.push_back({ 0, { } });
  size_t q = 1;
  while (!dfs.empty()) {
    auto [p, current_code] = std::move(dfs.back());
    dfs.pop_back();
    auto iter = code_to_val.find(current_code);
    if (iter != code_to_val.end()) {
      ans[p] = iter->second;
      continue;
    }
    current_code.push_back(true);
    dfs.push_back({ q++, current_code });
    ans[p] = std::make_pair(q, q - 1);
    current_code.back() = false;
    dfs.push_back({ q++, std::move(current_code) });
  }
  return ans;
}
