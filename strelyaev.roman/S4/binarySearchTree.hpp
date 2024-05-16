#ifndef BINARYSEARCHTREE_HPP
#define BINARYSEARCHTREE_HPP
#include <functional>
#include "node.hpp"
#include "treeIterator.hpp"
#include "constTreeIterator.hpp"

namespace strelyaev
{
  template< typename Key, typename T, typename Compare = std::less< Key > >
  class Tree
  {
    using node_t = detail::Node< Key, T >;
    using iterator_t = Iterator< Key, T >;
    using c_iterator_t = ConstIterator< Key, T >;
    using tree_t = Tree< Key, T, Compare >;
    public:
      Tree():
        root_(nullptr),
        cmp_(Compare()),
        size_(0)
      {}

      Tree(const tree_t& other):
        Tree()
      {
        if (other.root_)
        {
          for (auto it = other.cbegin(); it != other.cend(); it++)
          {
            insert(*it);
          }
        }
      }

      Tree(tree_t&& other):
        root_(other.root_),
        cmp_(other.cmp_),
        size_(other.size_)
      {
        other.root_ = nullptr;
        other.cmp_ = Compare();
        other.size_ = 0;
      }

      tree_t& operator=(const tree_t& other)
      {
        if (this != std::addressof(other))
        {
          tree_t temp(other);
          std::swap(root_, temp.root_);
        }
        return *this;
      }

      tree_t& operator=(tree_t&& other)
      {
        if (this != std::addressof(other))
        {
          clear(root_);
          std::swap(root_, other.root_);
          std::swap(size_, other.size_);
          std::swap(cmp_, other.cmp_);
        }
        return *this;
      }

      ~Tree()
      {
        clear();
      }

      void swap(tree_t& other)
      {
        std::swap(other.root_, root_);
        std::swap(other.size_, size_);
      }

      iterator_t insert(const std::pair< Key, T >& pair)
      {
        return insert(pair.first, pair.second);
      }

      iterator_t insert(const Key& key, const T& value)
      {
        if (!root_)
        {
          root_ = new node_t(nullptr, nullptr, nullptr, key, value);
          size_++;
          return iterator_t(root_);
        }
        node_t* current = root_;
        node_t* parent = nullptr;
        while (current)
        {
          parent = current;
          if (cmp_(key, current->data_.first))
          {
            current = current->left_;
          }
          else
          {
            current = current->right_;
          }
        }
        iterator_t result(nullptr);
        if (cmp_(key, parent->data_.first))
        {
          parent->left_ = new node_t(nullptr, parent, nullptr, key, value);
          result.node_ = parent->left_;
        }
        else
        {
          parent->right_ = new node_t(nullptr, parent, nullptr, key, value);
          result.node_ = parent->right_;
        }
        size_++;
        while (parent)
        {
          updateHeight(parent);
          balance(parent);
          parent = parent->parent_;
        }
        return result;
      }

      iterator_t find(const Key& key)
      {
        node_t* current = root_;
        while (current)
        {
          if (current->data_.first == key)
          {
            return iterator_t(current);
          }
          else if (cmp_(current->data_.first, key))
          {
            current = current->right_;
          }
          else
          {
            current = current->left_;
          }
        }
        return iterator_t(current);
      }

      size_t count(const Key& key) const
      {
        node_t* current = root_;
        while (current)
        {
          if (current->data_.first == key)
          {
            return 1;
          }
          else if (cmp_(key, current->data_.first))
          {
            current = current->left_;
          }
          else
          {
            current = current->right_;
          }
        }
        return 0;
      }

      std::pair< iterator_t, iterator_t > equal_range(const Key& key)
      {
        iterator_t it = find(key);
        if (it == end())
        {
          return std::make_pair(end(), end());
        }
        iterator_t next = it;
        ++next;
        return std::make_pair(it, next);
      }

      iterator_t begin() noexcept
      {
        if (root_ == nullptr)
        {
          return iterator_t(nullptr);
        }
        node_t* current = root_;
        while (current->left_)
        {
          current = current->left_;
        }
        return iterator_t(current);
      }

      iterator_t end() noexcept
      {
        return iterator_t(nullptr);
      }

      c_iterator_t cbegin() const noexcept
      {
        if (root_ == nullptr)
        {
          return c_iterator_t(nullptr);
        }
        node_t* current = root_;
        while (current->left_)
        {
          current = current->left_;
        }
        return c_iterator_t(current);
      }

      c_iterator_t cend() const noexcept
      {
        return c_iterator_t(nullptr);
      }

      T& at(const Key& key)
      {
        auto it = find(key);
        if (it == end())
        {
          throw std::out_of_range("Out of range");
        }
        return it->second;
      }

      T& operator[](const Key& key) noexcept
      {
        auto it = find(key);
        if (it == end())
        {
          it = insert(key, T());
        }
        return it->second;
      }

      bool empty() noexcept
      {
        return size_ == 0;
      }

      size_t size() noexcept
      {
        return size_;
      }

      void clear() noexcept
      {
        clear(root_);
        root_ = nullptr;
      }

    private:
      node_t* root_;
      Compare cmp_;
      size_t size_;

      void clear(node_t* node) noexcept
      {
        if (node)
        {
          clear(node->right_);
          clear (node->left_);
          delete node;
        }
      }

      int getHeight(node_t* node)
      {
        if (!node)
        {
          return -1;
        }
        return node->height_;
      }

      int getBalance(node_t* node)
      {
        if (!node)
        {
          return 0;
        }
        return getHeight(node->right_) - getHeight(node->left_);
      }

      void updateHeight(node_t* node)
      {
        node->height_ = std::max(getHeight(node->left_), getHeight(node->right_)) + 1;
      }

      void rotateRight(node_t* node)
      {
        std::swap(node->left_->data_, node->data_);
        node_t* buffer = node->right_;
        node->right_ = node->left_;

        node->left_ = node->right_->left_;
        if (node->left_)
        {
          node->left_->parent_ = node;
        }

        node->right_->left_ = node->right_->right_;

        node->right_->right_ = buffer;
        if (buffer)
        {
          buffer->parent_ = node->right_;
        }

        updateHeight(node->right_);
        updateHeight(node);
      }

      void rotateLeft(node_t* node)
      {
        std::swap(node->right_->data_, node->data_);
        node_t* buffer = node->left_;
        node->left_ = node->right_;

        node->right_ = node->left_->right_;
        if (node->right_)
        {
          node->right_->parent_ = node;
        }

        node->left_->right_ = node->left_->left_;

        node->left_->left_ = buffer;
        if (buffer)
        {
          buffer->parent_ = node->left_;
        }

        updateHeight(node->left_);
        updateHeight(node);
      }

      void balance(node_t* node)
      {
        int balance = getBalance(node);
        if (balance == -2)
        {
          if (getBalance(node->left_) == 1)
          {
            rotateLeft(node->left_);
          }
          rotateRight(node);
        }
        else if (balance == 2)
        {
          if (getBalance(node->right_) == -1)
          {
            rotateRight(node->right_);
          }
          rotateLeft(node);
        }
      }
  };
}

#endif

