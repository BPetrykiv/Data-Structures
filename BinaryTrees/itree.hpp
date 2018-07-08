#ifndef ITREE_HPP
#define ITREE_HPP
#include <cstddef>

template<typename Type, typename Comparer>
class TreeInterface
{
public:
    virtual void insert(const Type& item) = 0;
    virtual void insert(Type&& item) = 0;
    virtual const Type* find(const Type& item) const = 0;
    virtual bool isEmpty() const noexcept = 0;
    virtual void remove(const Type& item) = 0;
    virtual void clear() noexcept = 0;
    virtual size_t size() const noexcept = 0;

    virtual ~TreeInterface() noexcept {}
};


#endif // ITREE_HPP
