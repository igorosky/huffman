#pragma once

#include <optional>
#include <string>
#include <vector>
#include "huffman_tree.hpp"

namespace huffman {
class HuffmanCoder {
  HuffmanTree _huffman_tree;

public:
  explicit HuffmanCoder(HuffmanTree);

  std::optional<std::vector<char>> encode(std::string_view text) const;
  std::optional<std::string> decode(const std::vector<char>& data, size_t len) const;
};
}  // namespace huffman
