#include "huffman_coder.hpp"

#include <utility>

huffman::HuffmanCoder::HuffmanCoder(HuffmanTree huffman_tree)
: _huffman_tree(std::move(huffman_tree)) { }

std::optional<std::vector<char>> huffman::HuffmanCoder::encode(std::string_view text) const {
  size_t p = 0;
  std::vector<char> ans;
  ans.reserve(text.size());
  for (char c : text) {
    auto coded = _huffman_tree.get_code(c);
    if (!coded.has_value()) {
      return std::nullopt;
    }
    for (bool v : coded.value()) {
      if (p / 8 >= ans.size()) {
        ans.push_back(0);
      }
      if (v) {
        ans.back() |= static_cast<char>(0x80 >> static_cast<int>(p % 8));
      }
      ++p;
    }
  }
  return ans;
}

std::optional<std::string> huffman::HuffmanCoder::decode(const std::vector<char>& data, size_t len) const {
  size_t p = 0;
  size_t q = 0;
  if (auto c = std::get_if<char>(&_huffman_tree._tree[0])) {
    return std::string(len, *c);
  }
  std::string ans;
  ans.reserve(len);
  while (ans.size() < len) {
    auto [left, right] = std::get<std::pair<size_t, size_t>>(_huffman_tree._tree[q]);
    q = ((data[p / 8] & (0x80 >> (p % 8))) != 0) ? right : left;
    if (auto c = std::get_if<char>(&_huffman_tree._tree[q])) {
      ans.push_back(*c);
      q = 0;
    }
    ++p;
  }
  return ans;
}
