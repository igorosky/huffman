#pragma once

#include <cstddef>
#include <set>
#include <unordered_map>
#include <variant>
#include "huffman_tree.hpp"
#include "types_def.hpp"

namespace huffman {
template<HUFFMAN_TYPE T>
class HuffmanTreeBuilder {
  std::unordered_map<T, size_t> _frequencies;

public:
  HuffmanTree<T> build() const;
  void set_frequencies(const T&, size_t);
  void add_frequency(const T&, size_t);
  template<typename I>
  ITER_REQUIREMENT(I, T)
  void read_text(I, I);
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
struct VisitingHelper : T ...
{
  using T::operator() ...;
};

template<typename ... T>
VisitingHelper(T ...)->VisitingHelper<T...>;

template<HUFFMAN_TYPE T>
huffman::HuffmanTree<T> huffman::HuffmanTreeBuilder<T>::build() const {
  std::vector<std::variant<std::pair<size_t, size_t>, T>> nodes;
  nodes.reserve(_frequencies.size() * 2);
  std::set<std::pair<size_t, size_t>> tree;
  nodes.push_back(static_cast<T>(0));
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
    }, nodes[p]);
  }

  return HuffmanTree<T>(nodes, codes);
}
