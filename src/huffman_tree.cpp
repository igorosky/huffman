#include "huffman_tree.hpp"

template<typename ... T>
struct VisitingHelper : T ...
{
  using T::operator() ...;
};

template<typename ... T>
VisitingHelper(T ...)->VisitingHelper<T...>;

huffman::HuffmanTree::HuffmanTree(std::vector<std::variant<std::pair<size_t, size_t>, char>> tree)
: _tree(std::move(tree)) {
  std::vector<std::pair<std::vector<bool>, size_t>> dfs;
  dfs.push_back({ { }, 0 });
  while (!dfs.empty()) {
    auto [code, p] = std::move(dfs.back());
    dfs.pop_back();
    std::visit(VisitingHelper{
      [this, &dfs, &code](std::pair<size_t, size_t> v) {
        auto [p, q] = v;
        code.push_back(false);
        dfs.push_back({ code, p });
        code.back() = true;
        dfs.push_back({ std::move(code), q });
      },
      [this, &code](char c) {
        _codes[static_cast<size_t>(static_cast<unsigned char>(c))] = std::move(code);
      },
    }, _tree[p]);
  }
}

std::optional<char> huffman::HuffmanTree::get_char(const std::vector<bool>& code) const {
  size_t p = 0;
  if (_tree.empty()) {
    return std::nullopt;
  }
  for (auto v : code) {
    if (std::holds_alternative<char>(_tree[p])) {
      return std::nullopt;
    }
    auto [left, right] = std::get<std::pair<size_t, size_t>>(_tree[p]);
    p = v ? right : left;
  }
  if (auto ans = std::get_if<char>(&_tree[p])) {
    return *ans;
  }
  return std::nullopt;
}

const std::optional<std::vector<bool>> & huffman::HuffmanTree::get_code(char c) const {
  return _codes[static_cast<size_t>(static_cast<unsigned char>(c))];
}
