#include <gtest/gtest.h>
#include "huffman_tree_builder.hpp"
#include "huffman_coder.hpp"

#define LOREM_IPSUM \
  "Lorem ipsum dolor sit amet, \
consectetur adipiscing elit.Ut blandit elementum tortor viverra posuere.Pellentesque quis eros at leo sodales \
lobortis.Fusce dictum iaculis sapien non venenatis.Nullam quis pellentesque leo.Curabitur ullamcorper, \
nulla ut pretium placerat, metus nulla eleifend nunc, \
nec efficitur odio ex eu eros.Cras hendrerit tempus ornare.Integer tincidunt, sem id posuere accumsan, \
enim nibh blandit nulla, in lacinia justo sem at eros.Maecenas maximus velit leo, eu gravida nisl viverra vitae. \
\
Sed a nunc vulputate, suscipit neque eget, tempor eros.Sed nunc libero, ultricies et libero quis, \
imperdiet vehicula massa.Sed sem metus, viverra at mauris non, \
iaculis dapibus velit.Aliquam convallis arcu sit amet diam faucibus sagittis vitae a leo.Nulla fermentum, \
ex tincidunt tincidunt pulvinar, lacus justo tempus leo, id aliquet tellus metus id libero.Nullam tincidunt, \
eros ac vehicula bibendum, erat erat dictum tortor, a mattis ligula eros sed urna.Nunc non sem finibus, \
ullamcorper augue at, vehicula felis.Sed mi arcu, ullamcorper vel ornare ut, \
laoreet eget dolor.Nullam mattis efficitur tortor id faucibus.Nam fermentum posuere dui vitae maximus.Duis ornare \
neque elit, sed vulputate felis tempus eget.Aenean malesuada dapibus metus, \
sed tempus velit dapibus sit amet.Pellentesque nec interdum dolor.Morbi bibendum est eu arcu placerat, \
eleifend sagittis magna rutrum.Quisque vel risus suscipit, volutpat odio vel, \
ultrices est.Ut porta congue risus sed tempor. \
\
Duis malesuada, felis et tincidunt rhoncus, mauris quam condimentum erat, \
id pharetra risus nunc sed nulla.Maecenas dui sapien, porta sit amet mattis non, \
rhoncus quis neque.Nam id odio porttitor, porta velit vitae, \
lacinia urna.Nullam ut rutrum velit.Donec blandit dui ac lorem consectetur, \
vitae condimentum metus vestibulum.Praesent orci tortor, pretium id porttitor sed, \
porttitor id magna.Pellentesque quis sapien quis eros fermentum cursus vel ac eros.Vestibulum venenatis, \
mi sit amet vehicula egestas, felis turpis rutrum neque, sit amet rhoncus mauris elit at ipsum.Ut ut varius felis, \
sit amet convallis erat.Donec vitae turpis vel quam luctus ultricies nec ut velit.Aliquam tristique felis massa, \
at pellentesque velit mollis quis.Nunc convallis nec velit sit amet posuere.Quisque commodo, \
dolor quis viverra viverra, arcu ex posuere metus, \
imperdiet consectetur nulla sapien a justo.Nam venenatis ornare enim sed malesuada. \
\
Nam congue rhoncus ex sit amet convallis.Nulla facilisi.Nam ipsum magna, fringilla ut sapien at, \
dapibus aliquet elit.Cras molestie sem in volutpat congue.In semper massa eu molestie efficitur.Vestibulum viverra \
massa et lectus vestibulum, in consectetur lorem finibus.Nullam est justo, scelerisque pretium nunc at, \
accumsan convallis est.Morbi justo urna, pulvinar eu elit quis, blandit sagittis sapien. \
\
Ut finibus nibh quis felis euismod tincidunt.Nullam eleifend molestie enim eget vestibulum.Morbi sed fermentum tellus, \
ac vehicula sapien.Integer lectus ex, rutrum at pellentesque ac, \
placerat nec urna.Nullam pharetra arcu varius magna tincidunt hendrerit.Praesent interdum laoreet erat.Integer \
lobortis, dolor et feugiat mattis, nisl magna ornare quam, in accumsan odio augue non ante.Quisque dui urna, \
fermentum non aliquet eu, viverra vitae risus.Quisque luctus mollis lorem, \
aliquam gravida lacus auctor in.Aenean a malesuada ante, sed tincidunt purus."

TEST(Huffman, BasicAssertions) {
  huffman::HuffmanTreeBuilder<char> builder;
  std::string text = LOREM_IPSUM;
  builder.read_text(text.begin(), text.end());
  huffman::HuffmanCoder<char> coder(builder.build());
  auto encoded = coder.encode(text.begin(), text.end());
  ASSERT_TRUE(encoded.has_value()) << "Encoded successfully";
  EXPECT_LE(encoded.value().size(), text.size()) << "Encoded version is equal or shorter";
  auto ans = coder.decode(encoded.value(), text.size());
  ASSERT_TRUE(ans.has_value()) << "Decoded successfully";
  std::string returnred_value;
  for (auto c : ans.value()) {
    returnred_value += c;
  }
  EXPECT_EQ(returnred_value, text) << "Decoded text matches initial text";
}
