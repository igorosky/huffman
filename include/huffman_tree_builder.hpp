#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <string_view>
#include "huffman_tree.hpp"

namespace huffman {
class HuffmanTreeBuilder {
  std::array<size_t, std::numeric_limits<unsigned char>::max()> _frequencies;

public:
  HuffmanTree build();
  void set_frequencies(char, size_t);
  void add_frequency(char, size_t);
  void read_text(std::string_view);
};
}  // namespace huffman
