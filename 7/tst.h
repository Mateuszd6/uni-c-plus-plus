#ifndef TST_H
#define TST_H

#include <functional>
#include <memory>
#include <numeric>

namespace Detail {
// Funkcja fold na zakresie wyznaczonym przez iteratory dziaĹa nastÄpujÄco:
//   functor(...functor(functor(acc, *first), *(first + 1))..., *(last - 1)) W
//   szczegĂłlnym przypadku first == last fold zwraca acc.
template <typename Iter, typename Acc, typename Functor>
Acc fold(Iter first, Iter last, Acc acc, Functor functor) {
    // Turns out that stl vendors have already implemented that.
    return std::accumulate(first, last, acc, functor);
}
} // namespace Detail

template <typename C = char>
struct TST {
  private:
    // Node class stroed in the tree if it is not empty.
    struct Node {
        C data;
        bool endof_str;
        TST<C> branch[3];

        // Explicitly saying std::shared_ptr<Node>{nullptr} instead of just
        // nullptr looks as a mistake, but if fact passing just nullptr calls
        // implicitly TST constructor from C const* which is not desired
        // behaviour. Also its pretty tricky to debug.
        Node(C data_, bool endof_str_)
            : data{data_}, endof_str{endof_str_},
              branch{std::shared_ptr<Node>{nullptr},
                     std::shared_ptr<Node>{nullptr},
                     std::shared_ptr<Node>{nullptr}} {
        }

        Node(C data_, bool endof_str_, std::shared_ptr<Node> const &center)
            : data{data_}, endof_str{endof_str_},
              branch{std::shared_ptr<Node>{nullptr}, center,
                     std::shared_ptr<Node>{nullptr}} {
        }

        Node(C data_, bool endof_str_, std::shared_ptr<Node> const &left,
             std::shared_ptr<Node> const &center,
             std::shared_ptr<Node> const &right)
            : data{data_}, endof_str{endof_str_}, branch{left, center, right} {
        }
    };

    // This is designed to behave like an optional type (node == nullptr means
    // the tree is empty), otherwise we store its value and more.
    std::shared_ptr<Node> const node;

    // Returns the next branch to go by comparing the given character ch with
    // the data stored in the current node. This assumes that the tree is not
    // empty!
    int next_branch(C ch) const {
        return (ch == node->data ? 1 : (ch > node->data ? 2 : 0));
    }

    // Traverse the tree. It returns a pair, so that it can be used by exist and
    // prefix. First value is the max depth at which the prefix was reached, and
    // the second is the last node on this path which has the endof_str value
    // equal to true. So that this traverse can be used either by 'prefix' or by
    // 'exist'.
    std::pair<size_t, size_t> traverse_impl(C const *sv, size_t depth,
                                            size_t best_ended) const {
        auto aux = [&] {
            auto next = next_branch(*sv);
            auto const &next_node = node->branch[next];
            auto next_sv = (next == 1 ? sv + 1 : sv);
            auto next_depth = (next == 1 ? depth + 1 : depth);
            auto next_best_ended =
                ((node->endof_str && next == 1) ? depth + 1 : best_ended);

            // This is (or at least should be) optimized with tail
            // recursion.
            return next_node.traverse_impl(next_sv, next_depth,
                                           next_best_ended);
        };

        return ((*sv == 0) || empty()) ? std::make_pair(depth, best_ended)
                                       : aux();
    }

    // This is a private ctor so that we can create a Tree object from the
    // previously created node.
    TST(std::shared_ptr<Node> const &other_node) : node{other_node} {
    }

  public:
    // Create a tree that contains one empty node.
    TST() : node{nullptr} {
    }

    // Creates a single tree that contais one word, which is stored in str.
    TST(std::basic_string<C> const &str)
        : node{Detail::fold(
              str.rbegin(), str.rend(), std::shared_ptr<Node>{nullptr},
              [](std::shared_ptr<Node> prev, C ch) {
                  return std::make_shared<Node>(ch, prev == nullptr, prev);
              })} {
    }

    // Same as calling std::basic_string constructor.
    TST(C const *str) : TST{std::basic_string<C>(str)} {
    }

    // Same as calling tree + (str.data())
    TST operator+(std::basic_string<C> const &str) const {
        return ((*this) + str.data());
    }

    // Returns a new tree that contains all previous words plus the new word,
    // pointed by sv. Passing nullptr results in undefined behaviour.
    TST operator+(C const *sv) const {
        auto aux = [&] {
            auto next = next_branch(*sv); // This is the index of new branch.
            auto new_branch = (node->branch[next] + (next == 1 ? sv + 1 : sv));

            std::shared_ptr<Node> branches[3];
            std::transform(node->branch, node->branch + 3, branches,
                           [](auto x) { return x.node; });
            branches[next] = new_branch.node; // Replace with new branch.

            auto new_node_sp = std::make_shared<Node>(
                node->data, node->endof_str || (*(sv + 1) == 0), branches[0],
                branches[1], branches[2]);

            return TST{new_node_sp};
        };

        return ((*sv == 0) ? (*this) : (empty()) ? TST{sv} : aux());
    }

    // Return the value stored in the current node, or throws exception when the
    // node is empty.
    C value() const {
        return (empty() ? throw std::logic_error(__FUNCTION__) : node->data);
    }

    // Return true if the current node is the end of string, or throws exception
    // when the node is empty.
    bool word() const {
        return (empty() ? throw std::logic_error(__FUNCTION__)
                        : node->endof_str);
    }

    // Return the left subtree of the current tree, or throws exception when the
    // node is empty.
    TST left() const {
        return (empty() ? throw std::logic_error(__FUNCTION__)
                        : node->branch[0]);
    }

    // Return the center subtree of the current tree, or throws exception when
    // the node is empty.
    TST center() const {
        return (empty() ? throw std::logic_error(__FUNCTION__)
                        : node->branch[1]);
    }

    // Return the right subtree of the current tree, or throws exception when
    // the node is empty.
    TST right() const {
        return (empty() ? throw std::logic_error(__FUNCTION__)
                        : node->branch[2]);
    }

    // Returns true if the current node is empty.
    bool empty() const {
        return node == nullptr;
    }

    // Returns true if the string exists in the tree.
    bool exist(std::basic_string<C> const &str) const {
        auto [_, best_ended] = traverse_impl(str.data(), 0, 0);
        // With some reason, exist("") should return false.
        return (str.size() > 0 && best_ended == str.size());
    }

    // Wyszukuje najdĹuĹźszy wspĂłlny prefiks sĹowa str i sĹĂłw zawartych w danym
    // drzewie. PrzykĹad: jeĹli tst skĹada siÄ ze sĹĂłw "category", "functor"
    // oraz "theory" to tst.prefix("catamorphism") daje rezultat "cat".
    // NOTE: I think this is a great place to use std::basic_string_view to
    // avoid memory allocation forced by the current api.
    std::basic_string<C> prefix(std::basic_string<C> const &str) const {
        auto [depth, _] = traverse_impl(str.data(), 0, 0);
        return str.substr(0, depth);
    }

    // Tree fold fucntion. The functor takes the reference to the tree, and
    // iterators to the beginning and the end of the list containing results of
    // calling fold on the current node children.
    template <typename Acc, typename Functor>
    Acc fold(Acc acc, Functor functor) const {
        return empty()
                   ? acc
                   : Detail::fold(
                         node->branch, node->branch + 3, functor(acc, value()),
                         [&functor](Acc ac, TST const &t) -> Acc {
                             return t.empty() ? ac : t.fold(ac, functor);
                         });
    }

    // Returns the size of the tree. Node that the lambda ignores C, as it node
    // values are not necesarry to compute the number of nodes.
    size_t size() const {
        return this->fold(
            size_t{0}, [](size_t acc, C) -> size_t { return acc + size_t{1}; });
    }
};

#endif // TST_H
