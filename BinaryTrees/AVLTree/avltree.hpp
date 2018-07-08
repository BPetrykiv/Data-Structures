#ifndef AVLTREE_HPP
#define AVLTREE_HPP
#include <cstddef>
#include <type_traits>
#include <iterator>
#include <sstream>
#include "itree.hpp"
#include <stack>

template<typename T,
         typename Comparer = std::less<T>>

class AVLTree : public TreeInterface<T, Comparer>
{
public:
    using pointer = T*;
    using value_type = T;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;

    template<typename Type, typename EqType>
    using require = std::enable_if_t<
                    std::is_convertible<
                    typename std::iterator_traits<Type>::iterator_category,
                    EqType>::value>;


private:
    static constexpr int ALLOWED_UNBALANCE = 1;
    struct AVLTreeNode;
    using AVLTreeNodePtr = AVLTreeNode*;

    struct AVLTreeNode
    {
        value_type element;
        AVLTreeNodePtr left;
        AVLTreeNodePtr right;
        size_type height;


        AVLTreeNode(const_reference elem,
                    AVLTreeNodePtr lt = nullptr,
                    AVLTreeNodePtr rt = nullptr,
                    size_type h = 0)
            : element{elem}, left{lt}, right{rt}, height{h} {}

        AVLTreeNode(const AVLTreeNode&) = delete;
        AVLTreeNode& operator=(const AVLTreeNode&) = delete;
    };

    class const_iterator
    {
        friend class AVLTree<T, Comparer>;
    private:
        std::stack<AVLTreeNodePtr> st;
        AVLTreeNodePtr cur;

        void fill_stack(AVLTreeNodePtr p_root)
        {
            AVLTreeNodePtr p = p_root;
            while(p)
            {
                st.push(p);
                p = p->left;
            }
        }

        const_iterator(AVLTreeNodePtr p_root)
            : cur{nullptr}
        {
            fill_stack(p_root);
            if(!st.empty()){
                cur = st.top();
                st.pop();
            }
        }

    public:

        using self_type = const_iterator;
        using iterator_category = std::forward_iterator_tag;

        self_type operator++()
        {
            if(!st.empty())
            {
                cur = st.top();
                st.pop();
                fill_stack(cur->right);
            }
            else cur = nullptr;

            return *this;

        }
        self_type operator++(int)
        {
            if(!st.empty())
            {
                cur = st.top();
                st.pop();
                fill_stack(cur->right);
            }
            else
                cur = nullptr;
            return *this;
        }

        const_reference operator*() const
        {
            return cur->element;
        }
        const_pointer operator->() const
        {
            return &cur->element;
        }


        friend bool operator==(const const_iterator& it1, const const_iterator& it2)
        {
            return it1.cur == it2.cur;
        }
        friend bool operator != (const const_iterator& it1, const const_iterator& it2)
        {
            return !(it1 == it2);
        }
    };

    size_type height(AVLTreeNodePtr t) const noexcept {
        return t ? t->height : 0;
    }

    template<typename E>
    void insertImpl(AVLTreeNodePtr& root, E&& elem){

        if(!root){
            root = new AVLTreeNode{std::forward<E>(elem)};
        } else if(Comparer()(root->element, elem)) {
            insertImpl(root->right, std::forward<E>(elem));
        } else {
            insertImpl(root->left, std::forward<E>(elem));
        }
        balance(root);
    }

    void balance(AVLTreeNodePtr& t) noexcept {
        if(!t) return;

        int lrh = static_cast<int>(height(t->left)) - height(t->right);
        int rlh = static_cast<int>(height(t->right)) - height(t->left);

        // checking for overflow for lrh (0-1 can be the overflow)
        if(lrh > ALLOWED_UNBALANCE)
        {
            if(height(t->left->left) >= height(t->left->right))
                rotateLeftChild(t);
            else
                doubleRotateRightChild(t);
        }
        else if(rlh > ALLOWED_UNBALANCE)
        {
            if(height(t->right->right) >= height(t->right->left))
                rotateRightChild(t);
            else
                doubleRotateLeftChild(t);
        }
        t->height = std::max(height(t->left), height(t->right)) + 1;
    }

    void rotateLeftChild(AVLTreeNodePtr& k2) noexcept {
        AVLTreeNodePtr k1 = k2->left;
        if(k2)
        {           
            k2->left = k1->right;
            k1->right = k2;
            k2->height = std::max(height(k2->left), height(k2->right)) + 1;
            k1->height = std::max(height(k1->left), k2->height) + 1;
            k2 = k1;
        }
    }

    void rotateRightChild(AVLTreeNodePtr& k2) noexcept {
        AVLTreeNodePtr k1 = k2->right;
        if(k1)
        {
            k2->right = k1->left;
            k1->left = k2;
            k2->height = std::max(height(k2->right), height(k2->left))+1;
            k1->height = std::max(height(k1->right), k2->height)+1;
            k2 = k1;
        }
    }

    void doubleRotateRightChild(AVLTreeNodePtr& k) noexcept {
        rotateRightChild(k->left);
        rotateLeftChild(k);
    }

    void doubleRotateLeftChild(AVLTreeNodePtr& k) noexcept {
        rotateLeftChild(k->right);
        rotateRightChild(k);
    }

    const_pointer searchImpl(AVLTreeNodePtr root, const T& elem) const {

        if(!root) return nullptr;
        else if(Comparer()(root->element, elem)){
            return searchImpl(root->right, elem);
        }
        else if(Comparer()(elem, root->element)){
            return searchImpl(root->left, elem);
        }
        else return &root->element;
    }


    AVLTreeNodePtr __minimum(AVLTreeNodePtr root) const noexcept {
        if(root->left)
            root = root->left;
        return root;
    }

    void removeImpl(AVLTreeNodePtr& root, const T& elem) {

        if(!root) return;

        const T& key = root->element;

        if(Comparer()(elem,key)){
            removeImpl(root->left, elem);
        }
        else if(Comparer()(key, elem)){
            removeImpl(root->right, elem);
        }
        else
        {
            // there are no any children
            if(!root->left && !root->right){
                delete root;
                root = nullptr;
            }

            // if both child are exist
            else if(root->left && root->right)
            {
                AVLTreeNodePtr temp = __minimum(root->right);

                value_type eltemp = temp->element;
                root->element = std::move(eltemp);

                removeImpl(root->right, temp->element);
            }
            // only one child exist
            else
            {
                if(!root->left ){
                    AVLTreeNodePtr pRight = root->right;
                    delete root;
                    root = pRight;
                }
                else {
                    AVLTreeNodePtr pLeft = root->left;
                    delete root;
                    root = pLeft;
                }

            }
            m_size--;
        }
        balance(root);
    }

    void clearImpl(AVLTreeNodePtr& root) noexcept {
        if(root)
        {
            clearImpl(root->left);
            clearImpl(root->right);
            delete root;
            root = nullptr;
        }
    }



    template<typename Container>
    void toContainerImpl(Container& c, AVLTreeNodePtr root) const
    {
        if(!root)
            return;

        toContainerImpl(c, root->left);
        try
        {
            c.push_back(root->element);
        }catch(...)
        {
            c.clear();
            throw;
        }

        toContainerImpl(c, root->right);
    }
    AVLTreeNodePtr clone(AVLTreeNodePtr other)
    {
        if(other == nullptr)
            return nullptr;
        return new AVLTreeNode(other->element, clone(other->left), clone(other->right));
    }

    AVLTreeNodePtr m_root;
    std::size_t m_size;


public:
    AVLTree() noexcept
        : m_root{nullptr},
       m_size{0}
    {}

    AVLTree(const AVLTree& tree)
        : m_root{nullptr}, m_size{0}
    {
       m_root = clone(tree.m_root);
       m_size = tree.size();
    }

    AVLTree(AVLTree&& tree) noexcept
        : m_root(tree.m_root), m_size(tree.m_size)
    {
        tree.m_root = nullptr;
        tree.m_size = 0;
    }

    AVLTree(std::initializer_list<T> list)
        : AVLTree()
    {
        AVLTree<T> temp;
        for(auto&& it : list)
            temp.insert(it);

        swap(temp);
    }

    template<typename InputIterator,
             typename = require<InputIterator, std::input_iterator_tag>>
    AVLTree(InputIterator first, InputIterator last)
        : AVLTree()
    {
        AVLTree<T, Comparer> temp;
        for(auto it = first; it != last; ++it)
            temp.insert(*it);

        swap(temp);
    }

    AVLTree& operator=(AVLTree tree) noexcept
    {
        swap(tree);
        return *this;
    }

    AVLTree& operator=(AVLTree&& tree) noexcept
    {
        swap(tree);
        return *this;
    }

    AVLTree& operator=(std::initializer_list<T> list)
    {
        AVLTree<T> temp(list);
        swap(temp);
        return *this;
    }


    void insert(const T& elem) override
    {
        insertImpl(m_root, elem);
        m_size++;
    }

    void insert(T&& elem) override
    {
        insertImpl(m_root, std::move(elem));
        m_size++;
    }

    template<typename... Args>
    void emplace(Args&&... args)
    {
        insertImpl(m_root, std::forward<Args...>(args...));
        m_size++;
    }

    bool isEmpty() const noexcept override
    {
        return m_root == nullptr;
    }

    const_pointer find(const T& elem) const override
    {
        return searchImpl(m_root, elem);
    }

    void remove(const T& elem) override
    {
        removeImpl(m_root, elem);
    }

    void clear() noexcept override
    {
       m_size = 0;
       clearImpl(m_root);
    }

    constexpr std::size_t size() const noexcept override{return m_size;}


    const_iterator begin()
    {
        return AVLTree::const_iterator(m_root);
    }

    const_iterator end()
    {
        return AVLTree::const_iterator(nullptr);
    }


    void swap(AVLTree<T, Comparer>& other) noexcept
    {
        std::swap(this->m_root, other.m_root);
        std::swap(this->m_size, other.m_size);
    }

    template<typename SequenceContainer>
    SequenceContainer toContainer() const
    {
        SequenceContainer cntr;
        toContainerImpl(cntr, m_root);
        return cntr;
    }

    ~AVLTree() noexcept override
    {
        clear();
    }

};

namespace std
{
    template<typename T, typename Comparer = std::less<T>>
    void swap(AVLTree<T, Comparer>& t1, AVLTree<T, Comparer>& t2) noexcept
    {
        t1.swap(t2);
    }
}


#endif // AVLTREE_HPP
