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
#include <string>

namespace FuncHelpUtils {
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
    
    // Helper to determine whether there's a const_iterator for T.
    template<typename T>
    struct has_const_iterator {
    private:
        template<typename C> static char test(typename C::const_iterator*);
        template<typename C> static int  test(...);
    public:
        enum { value = sizeof(test<T>(0)) == sizeof(char) };
    };
    
    template<class F, class...Args>
    struct is_callable
    {
        template<class U> static auto test(U* p) -> decltype(std::ref(*p)(std::declval<Args>()...), void(), std::true_type());
        template<class U> static auto test(...) -> decltype(std::false_type());
        static constexpr bool value = decltype(test<F>(0))::value;
    };
    
    template <class T>
    using decay_t = typename std::decay<T>::type;
    
    template< bool B, class T = void >
    using enable_if_t = typename std::enable_if<B,T>::type;
    
    template <class Container>
    using iterator_deref = decltype(*std::declval<Container &>().begin());
    
    template <class Container>
    using iterator_deref_decay = decay_t<iterator_deref<Container> >;
    
    template <class Container>
    using reverse_iterator_deref = decltype(*std::declval<Container &>().rbegin());
    
    template <class Container>
    using reverse_iterator_deref_decay = decay_t<reverse_iterator_deref<Container> >;
    
    template <class Container, class F>
    using func_container_result_undecayed = decltype(std::ref(std::declval<F &>())(iterator_deref<Container>(*std::declval<Container &>().begin())));
    
    template <class Container, class F>
    using func_container_result = decay_t<func_container_result_undecayed<Container, F> >;
}

// map

template <class OutType,
          class InType,
          class F>
OutType map(const InType &container, const F &func)
{
    OutType result;
    
    for(auto const &val : container) {
        FuncHelpUtils::addItem(result, std::ref(func)(decltype(val)(val)));
    }
    
    return result;
}

template <template <class...> class InContainer,
          template <class...> class OutContainer = InContainer,
          class InType,
          class F>
auto map(const InContainer<InType> &container, const F &func)
 -> OutContainer<FuncHelpUtils::func_container_result<InContainer<InType>, F> >
{
    using OutType = FuncHelpUtils::func_container_result<InContainer<InType>, F>;
    return map<OutContainer<OutType>, InContainer<InType> >(container, func);
}

template <template <class...> class OutContainer,
          template <class...> class InContainer,
          class InType,
          class F,
          class = FuncHelpUtils::enable_if_t<!std::is_same<OutContainer<int>, InContainer<int> >::value> >
auto map(const InContainer<InType> &container, const F &func)
 -> OutContainer<FuncHelpUtils::func_container_result<InContainer<InType>, F> >
{
    return map<InContainer, OutContainer>(container, func);
}

// compr

template <class OutType,
          class InType,
          class F1,
          class F2>
OutType compr(const InType &container, const F1 &func, const F2 &predicate)
{
    OutType result;
    
    for(auto const &val : container) {
        if (std::ref(predicate)(decltype(val)(val))) {
            FuncHelpUtils::addItem(result, std::ref(func)(decltype(val)(val)));
        }
    }
    
    return result;
}

template <template <class...> class InContainer,
          template <class...> class OutContainer = InContainer,
          class InType,
          class F1,
          class F2>
auto compr(const InContainer<InType> &container, const F1 &func, const F2 &predicate)
 -> OutContainer<FuncHelpUtils::func_container_result<InContainer<InType>, F1> >
{
    using OutType = FuncHelpUtils::func_container_result<InContainer<InType>, F1>;
    return compr<OutContainer<OutType>, InContainer<InType> >(container, func, predicate);
}

template <template <class...> class OutContainer,
          template <class...> class InContainer,
          class InType,
          class F1,
          class F2,
          class = FuncHelpUtils::enable_if_t<!std::is_same<OutContainer<int>, InContainer<int> >::value> >
auto compr(const InContainer<InType> &container, const F1 &func, const F2 &predicate)
 -> OutContainer<FuncHelpUtils::func_container_result<InContainer<InType>, F1> >
{
    return compr<InContainer, OutContainer>(container, func, predicate);
}

// filter

template <template <class...> class InContainer,
          template <class...> class OutContainer = InContainer,
          class T,
          class F>
auto filter(const InContainer<T> &container, const F &predicate)
 -> OutContainer<T>
{
    OutContainer<T> result;
    
    for(const T &val : container) {
        if (std::ref(predicate)(decltype(val)(val))) {
            FuncHelpUtils::addItem(result, val);
        }
    }
    
    return result;
}

template <template <class...> class OutContainer,
          template <class...> class InContainer,
          class T,
          class F,
          class = FuncHelpUtils::enable_if_t<!std::is_same<OutContainer<int>, InContainer<int> >::value> >
auto filter(const InContainer<T> &container, const F &predicate)
 -> OutContainer<T>
{
    return filter<InContainer, OutContainer>(container, predicate);
}

// reject

template <template <class...> class InContainer,
          template <class...> class OutContainer = InContainer,
          class T,
          class F>
auto reject(const InContainer<T> &container, const F &predicate)
 -> OutContainer<T>
{
    OutContainer<T> result;
    
    for(const T &val : container) {
        if (!std::ref(predicate)(val)) {
            FuncHelpUtils::addItem(result, val);
        }
    }
    
    return result;
}

template <template <class...> class OutContainer,
          template <class...> class InContainer,
          class T,
          class F,
          class = FuncHelpUtils::enable_if_t<!std::is_same<OutContainer<int>, InContainer<int> >::value> >
auto reject(const InContainer<T> &container, const F &predicate)
 -> OutContainer<T>
{
    return reject<InContainer, OutContainer>(container, predicate);
}

// all of

template <class T, class F>
bool allOf(T const &list, const F &f)
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
bool anyOf(T const &list, const F &f)
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

namespace FuncHelpUtils {
    template <class T, class F>
    auto extremumBase(const T &list, const F &comp)
     -> const typename std::decay<decltype(*list.begin())>::type *
    {
        using U = typename std::decay<decltype(*list.begin())>::type;
        using V = decltype(list.cbegin());
        const U *extremumValue = nullptr;
        
        for(V it = list.cbegin(); it != list.cend(); ++it) {
            if (!extremumValue || std::ref(comp)(decltype(*it)(*it), *extremumValue)) {
                extremumValue = &(*it);
            }
        }
    
        return extremumValue;
    }
}

template <class T, class F>
auto extremum(const T &list, const F &comp)
 -> FuncHelpUtils::iterator_deref_decay<T>
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    const U *extremumValue = FuncHelpUtils::extremumBase(list, comp);
    
    if (extremumValue) {
        return *extremumValue;
    } else {
        return U();
    }
}

template <class T, class F, class U>
auto extremum(const T &list, const F &comp, const U &defaultVal)
 -> FuncHelpUtils::iterator_deref_decay<T>
{
    const U *extremumValue = FuncHelpUtils::extremumBase(list, comp);
    
    if (extremumValue) {
        return *extremumValue;
    } else {
        return defaultVal;
    }
}

// min(container)

template <class T>
auto min(const T &list)
 -> FuncHelpUtils::iterator_deref_decay<T>
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    return extremum(list, [] (const U &a, const U &b) {return a < b; });
}

template <class T,
          class U,
          class = FuncHelpUtils::enable_if_t<!FuncHelpUtils::is_callable<U, FuncHelpUtils::iterator_deref<T> >::value> >
auto min(const T &list, const U &defaultVal)
 -> U
{
    return extremum(list, [] (const U &a, const U &b) {return a < b; }, defaultVal);
}

// min(container, func)

namespace FuncHelpUtils {
    template <class T, class F>
    auto minBase(const T &list, const F &func)
      -> const FuncHelpUtils::iterator_deref_decay<T> *
    {
        using U = FuncHelpUtils::iterator_deref_decay<T>;
        using V = decltype(list.cbegin());
        using W = func_container_result<T, F>;
        const U *extremumValue = nullptr;
        W extremumComparator;
        
        for(V it = list.cbegin(); it != list.cend(); ++it) {
            W currentComparator = std::ref(func)(decltype(*it)(*it));
            
            if (!extremumValue || currentComparator < extremumComparator) {
                extremumValue = &(*it);
                extremumComparator = currentComparator;
            }
        }
    
        return extremumValue;
    }
}

template <class T,
          class F,
          class = FuncHelpUtils::enable_if_t<FuncHelpUtils::is_callable<F, FuncHelpUtils::iterator_deref<T> >::value> >
auto min(const T &list, const F &func)
 -> FuncHelpUtils::iterator_deref_decay<T>
{
    using U = FuncHelpUtils::iterator_deref_decay<T>;
    const U *extremumValue = FuncHelpUtils::minBase(list, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return U();
    }
}

template <class T, class F, class U>
U min(const T &list, const F &func, const U &defaultVal)
{
    const U *extremumValue = FuncHelpUtils::minBase(list, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return defaultVal;
    }
}

// max(container)

template <class T>
auto max(const T &list)
 -> FuncHelpUtils::iterator_deref_decay<T>
{
    using U = FuncHelpUtils::iterator_deref_decay<T>;
    return extremum(list, [] (const U &a, const U &b) {return a > b; });
}

template <class T,
          class U,
          class = FuncHelpUtils::enable_if_t<!FuncHelpUtils::is_callable<U, FuncHelpUtils::iterator_deref<T> >::value> >
auto max(const T &list, const U &defaultVal)
 -> U
{
    return extremum(list, [] (const U &a, const U &b) {return a > b; }, defaultVal);
}

// max(container, func)

namespace FuncHelpUtils {
    template <class T, class F>
    auto maxBase(const T &list, const F &func)
        -> const FuncHelpUtils::iterator_deref_decay<T> *
    {
        using U = FuncHelpUtils::iterator_deref_decay<T>;
        using V = decltype(list.cbegin());
        using W = func_container_result<T, F>;
        const U *extremumValue = nullptr;
        W extremumComparator;
        
        for(V it = list.cbegin(); it != list.cend(); ++it) {
            W currentComparator = std::ref(func)(decltype(*it)(*it));
            
            if (!extremumValue || currentComparator > extremumComparator) {
                extremumValue = &(*it);
                extremumComparator = currentComparator;
            }
        }
    
        return extremumValue;
    }
}

template <class T,
          class F,
          class = FuncHelpUtils::enable_if_t<FuncHelpUtils::is_callable<F, FuncHelpUtils::iterator_deref<T> >::value> >
auto max(const T &list, const F &func)
 -> FuncHelpUtils::iterator_deref_decay<T>
{
    using U = FuncHelpUtils::iterator_deref_decay<T>;
    const U *extremumValue = FuncHelpUtils::maxBase(list, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return U();
    }
}

template <class T, class F, class U>
U max(const T &list, const F &func, const U &defaultVal)
{
    const U *extremumValue = FuncHelpUtils::maxBase(list, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return defaultVal;
    }
}

// reduce

template <class T, class F>
auto reduce(const T &list, const F &func)
 -> FuncHelpUtils::iterator_deref_decay<T>
{
    using U = FuncHelpUtils::iterator_deref_decay<T>;
    using V = decltype(list.cbegin());
    V it = list.cbegin();
    
    if (it == list.cend()) {
        return U();
    }
    
    U memo = *it;
    ++it;
    
    while(it != list.cend()) {
        memo = std::ref(func)(U(memo), decltype(*it)(*it));
        ++it;
    }

    return memo;
}

template <class T, class F, class U>
U reduce(const T &list, const F &func, U memo)
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
 -> FuncHelpUtils::iterator_deref_decay<T>
{
    return reduce(list, [] (const U &a, const U &b) { return a+b; }, memo);
}

template <class T>
auto sum(const T &list)
 -> FuncHelpUtils::iterator_deref_decay<T>
{
    using U = FuncHelpUtils::iterator_deref_decay<T>;
    return reduce(list, [] (const U &a, const U &b) { return a+b; });
}

// sorted

// Since std::sort requires a random access iterator, we have to define
// different versions of sorted for both std::list and std::forward_list, and
// any classes derived from them. This is accomplished by using std::enable_if
// when determining the return type.

// NB: we're using enable_if in the return type since using it as a
// template argument will cause some of these definitions of sorted to
// be duplicates.

template <class T>
auto sorted(const T &list)
 -> FuncHelpUtils::enable_if_t<!std::is_base_of<std::list<typename T::value_type>, T>::value &&
                               !std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>
{
    T result(list);
    std::sort(result.begin(), result.end());
    return result;
}

template <class T, class F>
auto sorted(const T &list, const F &comp)
 -> FuncHelpUtils::enable_if_t<!std::is_base_of<std::list<typename T::value_type>, T>::value &&
                               !std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>
{
    T result(list);
    std::sort(result.begin(), result.end(), std::ref(comp));
    return result;
}

template <class T>
auto sorted(const T &list)
 -> FuncHelpUtils::enable_if_t<std::is_base_of<std::list<typename T::value_type>, T>::value, T>
{
    T result(list);
    result.sort();
    return result;
}

template <class T, class F>
auto sorted(const T &list, const F &comp)
 -> FuncHelpUtils::enable_if_t<std::is_base_of<std::list<typename T::value_type>, T>::value, T>
{
    T result(list);
    result.sort(std::ref(comp));
    return result;
}

template <class T>
auto sorted(const T &list)
 -> FuncHelpUtils::enable_if_t<std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>
{
    T result(list);
    result.sort();
    return result;
}

template <class T, class F>
auto sorted(const T &list, const F &comp)
 -> FuncHelpUtils::enable_if_t<std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>
{
    T result(list);
    result.sort(std::ref(comp));
    return result;
}

// contains

template <class T, class U>
bool contains(const T &container, const U &val)
{
    using ItType = decltype(container.cbegin());
    
    for(ItType it = container.cbegin(); it != container.cend(); ++it) {
        if ((*it) == val) {
            return true;
        }
    }
    
    return false;
}

template <class T, class U>
bool contains(const std::set<T, U> &container, const T &val)
{
    return container.find(val) != container.end();
}

// omit


template <template <class...> class InContainer,
          template <class...> class OutContainer = InContainer,
          class ValType,
          class = FuncHelpUtils::enable_if_t<!FuncHelpUtils::has_const_iterator<ValType>::value> >
auto omit(const InContainer<ValType> &container, const ValType &omitted)
 -> OutContainer<ValType>
{
    OutContainer<ValType> result;
    
    for(const ValType &val : container) {
        if (!(val == omitted)) {
            FuncHelpUtils::addItem(result, val);
        }
    }
    
    return result;
}

template <template <class...> class OutContainer,
          template <class...> class InContainer,
          class ValType,
          class = FuncHelpUtils::enable_if_t<!FuncHelpUtils::has_const_iterator<ValType>::value>,
          class = FuncHelpUtils::enable_if_t<!std::is_same<OutContainer<int>, InContainer<int> >::value> >
auto omit(const InContainer<ValType> &container, const ValType &omitted)
 -> OutContainer<ValType>
{
    return omit<InContainer, OutContainer>(container, omitted);
}

template <template <class...> class InContainer,
          template <class...> class OutContainer = InContainer,
          class ValType,
          class Container,
          class = FuncHelpUtils::enable_if_t<FuncHelpUtils::has_const_iterator<Container>::value> >
auto omit(const InContainer<ValType> &container, const Container &omitted)
 -> OutContainer<ValType>
{
    OutContainer<ValType> result;
    
    for(const ValType &val : container) {
        if (!contains(omitted, val)) {
            FuncHelpUtils::addItem(result, val);
        }
    }
    
    return result;
}

template <template <class...> class OutContainer,
          template <class...> class InContainer,
          class ValType,
          class Container,
          class = FuncHelpUtils::enable_if_t<FuncHelpUtils::has_const_iterator<Container>::value>,
          class = FuncHelpUtils::enable_if_t<!std::is_same<OutContainer<int>, InContainer<int> >::value> >
auto omit(const InContainer<ValType> &container, const Container &omitted)
 -> OutContainer<ValType>
{
    return omit<InContainer, OutContainer>(container, omitted);
}

// reversed

template <class T>
T reversed(const T &container)
{
    using ItType = decltype(container.crbegin());
    T result;
    
    for(ItType it = container.crbegin(); it != container.crend(); ++it) {
        FuncHelpUtils::addItem(result, *it);
    }
    
    return result;
}

// first

template <class T>
auto first(const T &container)
 -> FuncHelpUtils::iterator_deref_decay<T>
{
    using U = typename std::decay<decltype(*container.begin())>::type;
    
    if (container.size() == 0) {
        return U();
    } else {
        return *(container.cbegin());
    }
}

template <class T, class U>
auto first(const T &container, const U &defaultValue)
 -> FuncHelpUtils::iterator_deref_decay<T>
{
    if (container.size() == 0) {
        return defaultValue;
    } else {
        return *(container.cbegin());
    }
}

// last

template <class T>
auto last(const T &container)
 -> FuncHelpUtils::reverse_iterator_deref_decay<T>
{
    using U = typename std::decay<decltype(*container.begin())>::type;
    
    if (container.size() == 0) {
        return U();
    } else {
        return *(container.crbegin());
    }
}

template <class T, class U>
auto last(const T &container, const U &defaultValue)
 -> FuncHelpUtils::reverse_iterator_deref_decay<T>
{
    if (container.size() == 0) {
        return defaultValue;
    } else {
        return *(container.crbegin());
    }
}

// range

template <template <class...> class OutContainer>
auto range(int start, int end, int inc=1)
 -> OutContainer<int>
{
    OutContainer<int> result;
    int sign = (inc < 0) ? -1 : 1;
    
    for(int i = start; (i*sign) < (end*sign); i += inc) {
        FuncHelpUtils::addItem(result, i);
    }
    
    return result;
}

template <template <class...> class OutContainer>
auto range(int end)
 -> OutContainer<int>
{
    return range<OutContainer>(0, end);
}

// mapRange

#define __FH_mapRange_with_return_type(NAME, RET_TYPE) \
template <class F1> \
auto NAME(int start, int end, int inc, const F1 &func) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    using U = typename std::decay<decltype(std::ref(func)(0))>::type; \
    RET_TYPE<U> result; \
    int sign = (inc < 0) ? -1 : 1; \
     \
    for(int i = start; (i*sign) < (end*sign); i += inc) { \
        FuncHelpUtils::addItem(result, std::ref(func)(i)); \
    } \
     \
    return result; \
} \
 \
template <class F1, class F2> \
auto NAME(int start, int end, int inc, const F1 &func, const F2 &predicate) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    using U = typename std::decay<decltype(std::ref(func)(0))>::type; \
    RET_TYPE<U> result; \
    int sign = (inc < 0) ? -1 : 1; \
     \
    for(int i = start; (i*sign) < (end*sign); i += inc) { \
        if (std::ref(predicate)(i)) { \
            FuncHelpUtils::addItem(result, std::ref(func)(i)); \
        } \
    } \
     \
    return result; \
} \
 \
template <class F1> \
auto NAME(int start, int end, const F1 &func) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    return NAME(start, end, 1, func); \
} \
 \
template <class F1, class F2> \
auto NAME(int start, int end, const F1 &func, const F2 &predicate) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    return NAME(start, end, 1, func, predicate); \
} \
 \
template <class F1> \
auto NAME(int end, const F1 &func) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    return NAME(0, end, 1, func); \
} \
 \
template <class F1, class F2> \
auto NAME(int end, const F1 &func, const F2 &predicate) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    return NAME(0, end, 1, func, predicate); \
}

__FH_mapRange_with_return_type(listMapRange, std::list)
__FH_mapRange_with_return_type(vectorMapRange, std::vector)
__FH_mapRange_with_return_type(setMapRange, std::set)

// flatten

template <class T, class U>
inline U flatten(const T &container)
{
    U result;
    
    for(auto it = container.cbegin(); it != container.cend(); ++it) {
        for(auto jt = it->cbegin(); jt != it->cend(); ++jt) {
            FuncHelpUtils::addItem(result, *jt);
        }
    }
    
    return result;
}

template <template <class...> class T1, template <class...> class T2, class U, class ... Rest1, class ... Rest2>
auto flatten(const T1< T2<U, Rest2...>, Rest1...> &container)
 -> T2<typename std::decay<decltype(*(container.cbegin()->cbegin()))>::type>
{
    using V = typename std::decay<decltype(*(container.cbegin()->cbegin()))>::type;
    return ::flatten<T1< T2<U> >, T2<V> >(container);
}


#define __FH_flatten_with_return_type(NAME, RET_TYPE) \
template <class T> \
auto NAME(const T &container) \
 -> RET_TYPE<typename std::decay<decltype(*(container.cbegin()->cbegin()))>::type> \
{ \
    using U = RET_TYPE<typename std::decay<decltype(*(container.cbegin()->cbegin()))>::type>; \
    return ::flatten<T, U>(container); \
}

__FH_flatten_with_return_type(listFlatten, std::list)
__FH_flatten_with_return_type(vectorFlatten, std::vector)
__FH_flatten_with_return_type(setFlatten, std::set)

#endif // __FUNCTIONAL_HELPERS_H__
