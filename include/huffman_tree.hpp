#pragma once

#include <array>
#include <limits>
#include <optional>
#include <variant>
#include <vector>

namespace huffman {
class HuffmanCoder;
class HuffmanTreeBuilder;
class HuffmanTree {
  std::array<std::optional<std::vector<bool>>, std::numeric_limits<unsigned char>::max()> _codes;
  std::vector<std::variant<std::pair<size_t, size_t>, char>> _tree;

  friend HuffmanTreeBuilder;
  friend HuffmanCoder;

  explicit HuffmanTree(std::vector<std::variant<std::pair<size_t, size_t>, char>>);

public:
  HuffmanTree() = delete;
  std::optional<char> get_char(const std::vector<bool> &) const;
  const std::optional<std::vector<bool>>& get_code(char) const;
};
}  // namespace huffman
