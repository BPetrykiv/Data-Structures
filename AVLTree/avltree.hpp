#ifndef AVLTREE_HPP
#define AVLTREE_HPP
#include <cstddef>
#include <type_traits>
#include <iterator>

template<typename T,
         typename Comparer = std::less<T>>

class AVLTree
{

public:
    using pointer = T*;
    using value_type = T;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;

private:

    struct AVLTreeNode;
    using AVLTreeNodePtr = AVLTreeNode*;
    static constexpr int ALLOWED_UNBALANCE = 1;

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
        else if(Comparer()(root->element, elem)){
            return searchImpl(root->left, elem);
        }
        else return &root->element;
    }

    template<typename C = char,
             typename traits = std::char_traits<C>>
    void printImpl(AVLTreeNodePtr root, std::basic_ostream<C, traits>& os) const {

        if(!root) return;
        printImpl(root->left, os);
        os << root->element << " ";
        printImpl(root->right, os);
    }

    AVLTreeNodePtr __minimum(AVLTreeNodePtr root) const noexcept {
        if(root->left)
            root = root->left;
        return root;
    }

    void removeImpl(AVLTreeNodePtr& root, const T& elem) {

        if(!root) return;

        const T& key = root->element;

        if(Comparer()(elem, key)){
            removeImpl(root->right, elem);
        }
        else if(Comparer()(elem,key)){
            removeImpl(root->left, elem);
        }
        else
        {
            // there are no any children
            if(!root->left && !root->right)
                delete root;

            // if both child are exist
            else if(root->left && root->right)
            {
                AVLTreeNodePtr temp = __minimum(root->right);
                root->element = temp->element;
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
            _size--;
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


    AVLTreeNodePtr copyring(AVLTreeNodePtr other) noexcept(std::is_nothrow_constructible<T>())
    {
        if(other)
        {
            AVLTreeNodePtr new_root = new AVLTreeNode(other->element);
            new_root->left = copyring(other->left);
            new_root->right = copyring(other->right);
            return new_root;
        }
        return nullptr;
    }

    template<typename Container>
    void adding(Container& c, AVLTreeNodePtr root) const
    {
        if(!root) return;
        adding(c, root->left);
        c.push_back(root->element);
        adding(c, root->right);

    }

    AVLTreeNodePtr head;
    std::size_t _size;


public:
    AVLTree() noexcept
        : head{nullptr},
       _size{0}
    {}

    AVLTree(const AVLTree& tree) noexcept(std::is_nothrow_copy_constructible<T>())
        : _size(tree._size)
    {
        copyring(tree);
    }
    AVLTree(AVLTree&& tree) noexcept
        : head(tree.head), _size(tree._size)
    {
        tree.head = nullptr;
        tree._size = 0;
    }

    AVLTree(std::initializer_list<T> list)
    {
        for(auto&& it : list)
            insert(it);
    }

    template<typename InputIterator,
             typename = std::enable_if_t<
                 std::is_convertible<
                 typename std::iterator_traits<InputIterator>::iterator_category,
                 std::input_iterator_tag>::value>>
    AVLTree(InputIterator first, InputIterator last)
    {
        for(auto it = first; it != last; ++it)
            insert(*it);
    }

    AVLTree& operator=(const AVLTree& tree)
    {
        if(this != &tree)
        {
            clear();
            copyring(tree);
            _size = tree.size;
        }
        return *this;
    }

    AVLTree& operator=(AVLTree&& tree)
    {
        if(this != &tree)
        {
            clear();
            head = std::move(tree.head);
            tree.head = nullptr;
            _size = tree._size;
            tree._size = 0;
        }
        return *this;
    }

    AVLTree& operator=(std::initializer_list<T> list)
    {
        clear();
        for(auto&& it : list)
            insert(it);
        return *this;
    }


    void insert(const T& elem)
    {
        insertImpl(head, elem);
        _size++;
    }
    void insert(T&& elem)
    {
        insertImpl(head, std::move(elem));
        _size++;
    }

    template<typename... Args>
    void emplace(Args&&... args)
    {
        insertImpl(head, std::forward<Args...>(args...));
    }

    bool isEmpty() const noexcept
    {
        return head == nullptr;
    }
    const_pointer find(const T& elem) const
    {
        return searchImpl(head, elem);
    }
    void remove(const T& elem)
    {
        removeImpl(head, elem);
    }

    void clear() noexcept
    {
       _size = 0;
       clearImpl(head);
    }

    constexpr std::size_t size() const noexcept {return _size;}

    template<typename C = char,
             typename traits = std::char_traits<C>>
    void print(std::basic_ostream<C, traits>& os)
    {
        printImpl(head, os);
    }

    template<typename SequenceContainer>
    SequenceContainer toContainer() const
    {
        SequenceContainer cntr;
        adding(cntr, head);
        return cntr;
    }

    ~AVLTree() noexcept
    {
        clear();
    }

};


#endif // AVLTREE_HPP
