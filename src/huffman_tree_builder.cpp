#include "huffman_tree_builder.hpp"

#include <set>
#include <variant>

void huffman::HuffmanTreeBuilder::set_frequencies(char c, size_t frequency) {
  _frequencies[static_cast<size_t>(c)] = frequency;
}

void huffman::HuffmanTreeBuilder::add_frequency(char c, size_t frequency) {
  _frequencies[static_cast<size_t>(c)] += frequency;
}

void huffman::HuffmanTreeBuilder::read_text(std::string_view text) {
  for (char c : text) {
    add_frequency(c, 1);
  }
}

template<typename ... T>
struct VisitingHelper : T ...
{
  using T::operator() ...;
};

template<typename ... T>
VisitingHelper(T ...)->VisitingHelper<T...>;

huffman::HuffmanTree huffman::HuffmanTreeBuilder::build() {
  std::vector<std::variant<std::pair<size_t, size_t>, char>> nodes;
  nodes.reserve(_frequencies.size() * 2);
  std::set<std::pair<size_t, size_t>> tree;
  nodes.push_back(static_cast<char>(0));
  for (size_t i = 0; i < _frequencies.size(); ++i) {
    if (_frequencies[i] > 0) {
      nodes.push_back(static_cast<char>(i));
      tree.insert({ _frequencies[i], nodes.size() - 1 });
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
  return HuffmanTree(nodes);
}
