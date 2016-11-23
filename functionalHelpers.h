// functionalHelpers.h

// Providing convenient functional-style helper functions for C++ structures
// that can be invoked without using STL iterators and will return a new
// structure rather than modifying an existing one in place.
//
// These functions are defined using a couple of methods to make them general
// purpose and not require any template arguments.
// 1. In some cases general versions of the functions are defined as a
//    precompiler macro that takes the name of the container class it will
//    return as one of its parameters.
// 2. Some utility functions are defined and overloaded for all the various
//    supported container types in order to allow performing common tasks
//    such as adding an item to the container. In the case of a reserving
//    space in the container, it may do nothing for classes where such a task
//    is not necessary or applicable.
// 3. Generally functions in <functional> and <algoritms> are not used as
//    they don't always work consistently with Qt's container classes across
//    all of the versions of Qt.
//
// See the documentation for an explanation of how to use these functions.

#ifndef __FUNCTIONAL_HELPERS_H__
#define __FUNCTIONAL_HELPERS_H__

#include <list>
#include <forward_list>
#include <vector>
#include <set>

namespace _FunctionalHelpersUtils {
    // Default behavior of reserveSize is to do nothing
    template<class T>
    inline void reserveSize(T &container, int size)
    {
        (void)container;
        (void)size;
    }
    
    // In the case of vectors, we actually do want reserveSize() to do something.
    template<class U, class V>
    inline void reserveSize(std::vector<U, V> &container, int size)
    {
        container.reserve(size);
    }
    
    // And also STL strings
    template<class U>
    inline void reserveSize(std::basic_string<U> &container, int size)
    {
        container.reserve(size);
    }
    
    template<class T, class U>
    inline void addItem(T &container, const U &item)
    {
        container.push_back(item);
    }
    
    template<class U, class V>
    inline void addItem(std::set<U, V> &container, const U &item)
    {
        container.insert(item);
    }
}

// map

template <class T, class U, class F>
U map(const T &list, F &&func)
{
    using ItType = decltype(list.cbegin());
    U result;
    _FunctionalHelpersUtils::reserveSize(result, list.size());
    
    for(ItType it = list.cbegin(); it != list.cend(); ++it) {
        _FunctionalHelpersUtils::addItem(result, std::ref(func)(decltype(*it)(*it))); \
    }
    
    return result;
}

template <class T, class F>
T map(const T &list, F &&func)
{
    return ::map<T, T, F>(list, std::move(func));
}

#define __FH_map_with_specific_return_type(NAME, RET_TYPE) \
template <class T, class F> \
auto NAME(const T &list, F &&func) \
    -> RET_TYPE<typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type> \
{ \
    using U = RET_TYPE<typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type>; \
    return ::map<T, U, F>(list, std::move(func)); \
}

__FH_map_with_specific_return_type(listMap, std::list)
__FH_map_with_specific_return_type(vectorMap, std::vector)
__FH_map_with_specific_return_type(setMap, std::set)

// compr

template <class T, class U, class F1, class F2>
U compr(const T &list, F1 &&func, F2 &&predicate)
{
    using ItType = decltype(list.cbegin());
    U result;
    
    for(ItType it = list.cbegin(); it != list.cend(); ++it) {
        if (std::ref(predicate)(decltype(*it)(*it))) {
            _FunctionalHelpersUtils::addItem(result, std::ref(func)(decltype(*it)(*it)));
        }
    }
    
    return result;
}

template <class T, class F1, class F2>
T compr(const T &list, F1 &&func, F2 &&predicate)
{
    return ::compr<T, T, F1, F2>(list, std::move(func), std::move(predicate));
}

#define __FH_compr_with_specific_return_type(NAME, RET_TYPE) \
template <class T, class F1, class F2> \
auto NAME(const T &list, F1 &&func, F2 &&predicate) \
    -> RET_TYPE<typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type> \
{ \
    using U = RET_TYPE<typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type>; \
    return ::compr<T, U, F1, F2>(list, std::move(func), std::move(predicate)); \
}

__FH_compr_with_specific_return_type(listCompr, std::list)
__FH_compr_with_specific_return_type(vectorCompr, std::vector)
__FH_compr_with_specific_return_type(setCompr, std::set)

// filter

template <class T, class U, class F>
U filter(const T &list, F &&predicate)
{
    using ItType = decltype(list.cbegin());
    U result;
    
    for(ItType it = list.cbegin(); it != list.cend(); ++it) {
        if (std::ref(predicate)(decltype(*it)(*it))) {
            _FunctionalHelpersUtils::addItem(result, *it);
        }
    }
    
    return result;
}

template <class T, class F>
T filter(const T &list, F &&predicate)
{
    return ::filter<T, T, F>(list, std::move(predicate));
}

#define __FH_filter_with_specific_return_type(NAME, RET_TYPE) \
template <class T, class F> \
auto NAME(const T &list, F &&func) \
    -> RET_TYPE<typename std::decay<decltype(*list.begin())>::type> \
{ \
    using U = RET_TYPE<typename std::decay<decltype(*list.begin())>::type>; \
    return ::filter<T, U, F>(list, std::move(func)); \
}

__FH_filter_with_specific_return_type(listFilter, std::list)
__FH_filter_with_specific_return_type(vectorFilter, std::vector)
__FH_filter_with_specific_return_type(setFilter, std::set)

// all of

template <class T, class F>
bool allOf(T const &list, F &&f)
{
    using U = decltype(list.cbegin());
    for(U it = list.cbegin(); it != list.cend(); ++it) {
        if (!std::ref(f)(decltype(*it)(*it))) {
            return false;
        }
    }
    return true;
}

// any of

template <class T, class F>
bool anyOf(T const &list, F &&f)
{
    using U = decltype(list.cbegin());
    for(U it = list.cbegin(); it != list.cend(); ++it) {
        if (std::ref(f)(decltype(*it)(*it))) {
            return true;
        }
    }
    return false;
}

// extremum

template <class T, class F>
auto extremum(const T &list, F &&comp)
    -> typename std::decay<decltype(*list.begin())>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    using V = decltype(list.cbegin());
    const U *extremumValue = nullptr;
    
    for(V it = list.cbegin(); it != list.cend(); ++it) {
        if (!extremumValue || std::ref(comp)(decltype(*it)(*it), *extremumValue)) {
            extremumValue = &(*it);
        }
    }

    return *extremumValue;
}

// min(container)

template <class T>
auto min(const T &list)
    -> typename std::decay<decltype(*list.begin())>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    return extremum(list, [] (const U &a, const U &b) {return a < b; });
}

// min(container, func)

template <class T, class F>
auto min(const T &list, F &&func)
    -> typename std::decay<decltype(*list.begin())>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    using V = decltype(list.cbegin());
    using W = typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type;
    const U *extremumValue = nullptr;
    W extremumComparator;
    
    for(V it = list.cbegin(); it != list.cend(); ++it) {
        W currentComparator = std::ref(func)(decltype(*it)(*it));
        
        if (!extremumValue || currentComparator < extremumComparator) {
            extremumValue = &(*it);
            extremumComparator = currentComparator;
        }
    }

    return *extremumValue;
}

// max(container)

template <class T>
auto max(const T &list)
    -> typename std::decay<decltype(*list.begin())>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    return extremum(list, [] (const U &a, const U &b) {return a > b; });
}

// max(container, func)

template <class T, class F>
auto max(const T &list, F &&func)
    -> typename std::decay<decltype(*list.begin())>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    using V = decltype(list.cbegin());
    using W = typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type;
    const U *extremumValue = nullptr;
    W extremumComparator;
    
    for(V it = list.cbegin(); it != list.cend(); ++it) {
        W currentComparator = std::ref(func)(decltype(*it)(*it));
        
        if (!extremumValue || currentComparator > extremumComparator) {
            extremumValue = &(*it);
            extremumComparator = currentComparator;
        }
    }

    return *extremumValue;
}

// reduce

template <class T, class F, class U>
auto reduce(const T &list, F &&func, U memo)
    -> typename std::decay<decltype(*list.begin())>::type
{
    using V = decltype(list.cbegin());
    
    for(V it = list.cbegin(); it != list.cend(); ++it) {
        memo = std::ref(func)(U(memo), decltype(*it)(*it));
    }

    return memo;
}

// sum

template <class T, class U>
auto sum(const T &list, U memo)
    -> typename std::decay<decltype(*list.begin())>::type
{
    return reduce(list, [] (const U &a, const U &b) { return a+b; }, memo);
}

template <class T>
auto sum(const T &list)
    -> typename std::decay<decltype(*list.begin())>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    U memo = U();
    return reduce(list, [] (const U &a, const U &b) { return a+b; }, memo);
}

// sorted

// Since std::sort requires a random access iterator, we have to define
// different versions of sorted for both std::list and std::forward_list, and
// any classes derived from them. This is accomplished by using std::enable_if
// when determining the return type.

template <class T>
auto sorted(const T &list)
    -> typename std::enable_if<!std::is_base_of<std::list<typename T::value_type>, T>::value &&
                               !std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    std::sort(result.begin(), result.end());
    return result;
}

template <class T, class F>
auto sorted(const T &list, F &&comp)
    -> typename std::enable_if<!std::is_base_of<std::list<typename T::value_type>, T>::value &&
                               !std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    std::sort(result.begin(), result.end(), std::ref(comp));
    return result;
}

template <class T>
auto sorted(const T &list)
    -> typename std::enable_if<std::is_base_of<std::list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    result.sort();
    return result;
}

template <class T, class F>
auto sorted(const T &list, F &&comp)
    -> typename std::enable_if<std::is_base_of<std::list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    result.sort(std::ref(comp));
    return result;
}

template <class T>
auto sorted(const T &list)
    -> typename std::enable_if<std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    result.sort();
    return result;
}

template <class T, class F>
auto sorted(const T &list, F &&comp)
    -> typename std::enable_if<std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    result.sort(std::ref(comp));
    return result;
}

#endif // __FUNCTIONAL_HELPERS_H__
