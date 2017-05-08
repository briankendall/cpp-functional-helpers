// functionalHelpers.h

// Providing convenient functional-style helper functions for C++ structures
// that can be invoked without using STL iterators and will return a new
// structure rather than modifying an existing one in place.
//
// These functions are defined using a couple of methods to make them general
// purpose and not require any template arguments.
// 1. Functions that return a container can be called without template
//    arguments, and the return type will be the same type of container as
//    was passed in. Alternative, these same functions can take one
//    template argument that defines what type of container is returned.
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

#ifndef FUNCTIONAL_HELPERS_H_GUARD
#define FUNCTIONAL_HELPERS_H_GUARD

#include <list>
#include <forward_list>
#include <vector>
#include <set>
#include <string>

namespace FuncHelpUtils {
    // Default behavior of reserveSize is to do nothing
    template<class Container>
    inline void reserveSize(Container &container, int size)
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
    
    template<class Container, class U>
    inline void addItem(Container &container, const U &item)
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
    
    // Helper to determine whether there's a const_reverse_iterator for T.
    template<typename T>
    struct has_const_reverse_iterator {
    private:
        template<typename C> static char test(typename C::const_reverse_iterator*);
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
    
    template <class Val, class F>
    using func_result = decay_t<decltype(std::ref(std::declval<F &>())(std::declval<Val &>()))>;
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
          class ValType,
          class F>
auto filter(const InContainer<ValType> &container, const F &predicate)
 -> OutContainer<ValType>
{
    OutContainer<ValType> result;
    
    for(const ValType &val : container) {
        if (std::ref(predicate)(decltype(val)(val))) {
            FuncHelpUtils::addItem(result, val);
        }
    }
    
    return result;
}

template <template <class...> class OutContainer,
          template <class...> class InContainer,
          class ValType,
          class F,
          class = FuncHelpUtils::enable_if_t<!std::is_same<OutContainer<int>, InContainer<int> >::value> >
auto filter(const InContainer<ValType> &container, const F &predicate)
 -> OutContainer<ValType>
{
    return filter<InContainer, OutContainer>(container, predicate);
}

// reject

template <template <class...> class InContainer,
          template <class...> class OutContainer = InContainer,
          class ValType,
          class F>
auto reject(const InContainer<ValType> &container, const F &predicate)
 -> OutContainer<ValType>
{
    OutContainer<ValType> result;
    
    for(const ValType &val : container) {
        if (!std::ref(predicate)(val)) {
            FuncHelpUtils::addItem(result, val);
        }
    }
    
    return result;
}

template <template <class...> class OutContainer,
          template <class...> class InContainer,
          class ValType,
          class F,
          class = FuncHelpUtils::enable_if_t<!std::is_same<OutContainer<int>, InContainer<int> >::value> >
auto reject(const InContainer<ValType> &container, const F &predicate)
 -> OutContainer<ValType>
{
    return reject<InContainer, OutContainer>(container, predicate);
}

// all of

template <class Container, class F>
bool allOf(Container const &container, const F &f)
{
    for(auto const &val : container) {
        if (!std::ref(f)(decltype(val)(val))) {
            return false;
        }
    }
    return true;
}

// any of

template <class Container, class F>
bool anyOf(Container const &container, const F &f)
{
    for(auto const &val : container) {
        if (std::ref(f)(decltype(val)(val))) {
            return true;
        }
    }
    return false;
}

// extremum

namespace FuncHelpUtils {
    template <class Container, class F>
    auto extremumBase(const Container &container, const F &comp)
     -> const iterator_deref_decay<Container> *
    {
        using ValType = iterator_deref_decay<Container>;
        const ValType *extremumValue = nullptr;
        
        for(auto const &val : container) {
            if (!extremumValue || std::ref(comp)(decltype(val)(val), *extremumValue)) {
                extremumValue = &(val);
            }
        }
    
        return extremumValue;
    }
}

template <class Container, class F>
auto extremum(const Container &container, const F &comp)
 -> FuncHelpUtils::iterator_deref_decay<Container>
{
    using ValType = FuncHelpUtils::iterator_deref_decay<Container>;
    const ValType *extremumValue = FuncHelpUtils::extremumBase(container, comp);
    
    if (extremumValue) {
        return *extremumValue;
    } else {
        return ValType();
    }
}

template <class Container, class F, class ValType>
auto extremum(const Container &container, const F &comp, const ValType &defaultVal)
 -> FuncHelpUtils::iterator_deref_decay<Container>
{
    const ValType *extremumValue = FuncHelpUtils::extremumBase(container, comp);
    
    if (extremumValue) {
        return *extremumValue;
    } else {
        return defaultVal;
    }
}

// min(container)

template <class Container>
auto min(const Container &container)
 -> FuncHelpUtils::iterator_deref_decay<Container>
{
    using ValType = FuncHelpUtils::iterator_deref_decay<Container>;
    return extremum(container, [] (const ValType &a, const ValType &b) {return a < b; });
}

template <class Container,
          class ValType,
          class = FuncHelpUtils::enable_if_t<!FuncHelpUtils::is_callable<ValType, FuncHelpUtils::iterator_deref<Container> >::value> >
auto min(const Container &container, const ValType &defaultVal)
 -> ValType
{
    return extremum(container, [] (const ValType &a, const ValType &b) {return a < b; }, defaultVal);
}

// min(container, func)

namespace FuncHelpUtils {
    template <class Container, class F>
    auto minBase(const Container &container, const F &func)
      -> const FuncHelpUtils::iterator_deref_decay<Container> *
    {
        using ValType = FuncHelpUtils::iterator_deref_decay<Container>;
        using W = func_container_result<Container, F>;
        const ValType *extremumValue = nullptr;
        W extremumComparator;
        
        for(auto const &val : container) {
            W currentComparator = std::ref(func)(decltype(val)(val));
            
            if (!extremumValue || currentComparator < extremumComparator) {
                extremumValue = &(val);
                extremumComparator = currentComparator;
            }
        }
    
        return extremumValue;
    }
}

template <class Container,
          class F,
          class = FuncHelpUtils::enable_if_t<FuncHelpUtils::is_callable<F, FuncHelpUtils::iterator_deref<Container> >::value> >
auto min(const Container &container, const F &func)
 -> FuncHelpUtils::iterator_deref_decay<Container>
{
    using ValType = FuncHelpUtils::iterator_deref_decay<Container>;
    const ValType *extremumValue = FuncHelpUtils::minBase(container, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return ValType();
    }
}

template <class Container, class F, class ValType>
ValType min(const Container &container, const F &func, const ValType &defaultVal)
{
    const ValType *extremumValue = FuncHelpUtils::minBase(container, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return defaultVal;
    }
}

// max(container)

template <class Container>
auto max(const Container &container)
 -> FuncHelpUtils::iterator_deref_decay<Container>
{
    using ValType = FuncHelpUtils::iterator_deref_decay<Container>;
    return extremum(container, [] (const ValType &a, const ValType &b) {return a > b; });
}

template <class Container,
          class ValType,
          class = FuncHelpUtils::enable_if_t<!FuncHelpUtils::is_callable<ValType, FuncHelpUtils::iterator_deref<Container> >::value> >
auto max(const Container &container, const ValType &defaultVal)
 -> ValType
{
    return extremum(container, [] (const ValType &a, const ValType &b) {return a > b; }, defaultVal);
}

// max(container, func)

namespace FuncHelpUtils {
    template <class Container, class F>
    auto maxBase(const Container &container, const F &func)
        -> const FuncHelpUtils::iterator_deref_decay<Container> *
    {
        using ValType = FuncHelpUtils::iterator_deref_decay<Container>;
        using W = func_container_result<Container, F>;
        const ValType *extremumValue = nullptr;
        W extremumComparator;
        
        for(auto const &val : container) {
            W currentComparator = std::ref(func)(decltype(val)(val));
            
            if (!extremumValue || currentComparator > extremumComparator) {
                extremumValue = &(val);
                extremumComparator = currentComparator;
            }
        }
    
        return extremumValue;
    }
}

template <class Container,
          class F,
          class = FuncHelpUtils::enable_if_t<FuncHelpUtils::is_callable<F, FuncHelpUtils::iterator_deref<Container> >::value> >
auto max(const Container &container, const F &func)
 -> FuncHelpUtils::iterator_deref_decay<Container>
{
    using ValType = FuncHelpUtils::iterator_deref_decay<Container>;
    const ValType *extremumValue = FuncHelpUtils::maxBase(container, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return ValType();
    }
}

template <class Container, class F, class ValType>
ValType max(const Container &container, const F &func, const ValType &defaultVal)
{
    const ValType *extremumValue = FuncHelpUtils::maxBase(container, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return defaultVal;
    }
}

// reduce

template <class Container, class F>
auto reduce(const Container &container, const F &func)
 -> FuncHelpUtils::iterator_deref_decay<Container>
{
    using ValType = FuncHelpUtils::iterator_deref_decay<Container>;
    using V = decltype(container.cbegin());
    V it = container.cbegin();
    
    if (it == container.cend()) {
        return ValType();
    }
    
    ValType memo = *it;
    ++it;
    
    while(it != container.cend()) {
        memo = std::ref(func)(ValType(memo), decltype(*it)(*it));
        ++it;
    }

    return memo;
}

template <class Container, class F, class ValType>
ValType reduce(const Container &container, const F &func, ValType memo)
{
    for(auto const &val : container) {
        memo = std::ref(func)(ValType(memo), decltype(val)(val));
    }

    return memo;
}

// sum

template <class Container, class ValType>
auto sum(const Container &container, ValType memo)
 -> FuncHelpUtils::iterator_deref_decay<Container>
{
    return reduce(container, [] (const ValType &a, const ValType &b) { return a+b; }, memo);
}

template <class Container>
auto sum(const Container &container)
 -> FuncHelpUtils::iterator_deref_decay<Container>
{
    using ValType = FuncHelpUtils::iterator_deref_decay<Container>;
    return reduce(container, [] (const ValType &a, const ValType &b) { return a+b; });
}

// sorted

// Since std::sort requires a random access iterator, we have to define
// different versions of sorted for both std::list and std::forward_list, and
// any classes derived from them. This is accomplished by using std::enable_if
// when determining the return type.

// NB: we're using enable_if in the return type since using it as a
// template argument will cause some of these definitions of sorted to
// be duplicates.

template <class Container>
auto sorted(const Container &container)
 -> FuncHelpUtils::enable_if_t<!std::is_base_of<std::list<typename Container::value_type>, Container>::value &&
                               !std::is_base_of<std::forward_list<typename Container::value_type>, Container>::value, Container>
{
    Container result(container);
    std::sort(result.begin(), result.end());
    return result;
}

template <class Container, class F>
auto sorted(const Container &container, const F &comp)
 -> FuncHelpUtils::enable_if_t<!std::is_base_of<std::list<typename Container::value_type>, Container>::value &&
                               !std::is_base_of<std::forward_list<typename Container::value_type>, Container>::value, Container>
{
    Container result(container);
    std::sort(result.begin(), result.end(), std::ref(comp));
    return result;
}

template <class Container>
auto sorted(const Container &container)
 -> FuncHelpUtils::enable_if_t<std::is_base_of<std::list<typename Container::value_type>, Container>::value, Container>
{
    Container result(container);
    result.sort();
    return result;
}

template <class Container, class F>
auto sorted(const Container &container, const F &comp)
 -> FuncHelpUtils::enable_if_t<std::is_base_of<std::list<typename Container::value_type>, Container>::value, Container>
{
    Container result(container);
    result.sort(std::ref(comp));
    return result;
}

template <class Container>
auto sorted(const Container &container)
 -> FuncHelpUtils::enable_if_t<std::is_base_of<std::forward_list<typename Container::value_type>, Container>::value, Container>
{
    Container result(container);
    result.sort();
    return result;
}

template <class Container, class F>
auto sorted(const Container &container, const F &comp)
 -> FuncHelpUtils::enable_if_t<std::is_base_of<std::forward_list<typename Container::value_type>, Container>::value, Container>
{
    Container result(container);
    result.sort(std::ref(comp));
    return result;
}

// contains

template <class Container, class ValType>
bool contains(const Container &container, const ValType &targetVal)
{
    for(auto const &val : container) {
        if (val == targetVal) {
            return true;
        }
    }
    
    return false;
}

template <class Container, class Alloc>
bool contains(const std::set<Container, Alloc> &container, const Container &val)
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

template <class Container>
Container reversed(const Container &container)
{
    Container result;
    
    for(auto it = container.rbegin(); it != container.rend(); ++it) {
        FuncHelpUtils::addItem(result, *it);
    }
    
    return result;
}

// first

template <class Container>
auto first(const Container &container)
 -> FuncHelpUtils::iterator_deref_decay<Container>
{
    using ValType = FuncHelpUtils::iterator_deref_decay<Container>;
    
    if (container.size() == 0) {
        return ValType();
    } else {
        return *(container.cbegin());
    }
}

template <class Container, class ValType>
auto first(const Container &container, const ValType &defaultValue)
 -> FuncHelpUtils::iterator_deref_decay<Container>
{
    if (container.size() == 0) {
        return defaultValue;
    } else {
        return *(container.cbegin());
    }
}

// last

template <class Container>
auto last(const Container &container)
 -> FuncHelpUtils::reverse_iterator_deref_decay<Container>
{
    using ValType = FuncHelpUtils::iterator_deref_decay<Container>;
    
    if (container.size() == 0) {
        return ValType();
    } else {
        return *(container.crbegin());
    }
}

template <class Container, class ValType>
auto last(const Container &container, const ValType &defaultValue)
 -> FuncHelpUtils::reverse_iterator_deref_decay<Container>
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


template <template <class...> class OutContainer,
          class F>
auto mapRange(int start, int end, int inc, F func)
 -> OutContainer<FuncHelpUtils::func_result<int, F> >
{
    using ValType = FuncHelpUtils::func_result<int, F>;
    OutContainer<ValType> result;
    int sign = (inc < 0) ? -1 : 1;
    
    for(int i = start; (i*sign) < (end*sign); i += inc) {
        FuncHelpUtils::addItem(result, std::ref(func)(i));
    }
    
    return result;
}

template <template <class...> class OutContainer,
          class F1,
          class F2>
auto mapRange(int start, int end, int inc, F1 func, F2 predicate)
 -> OutContainer<FuncHelpUtils::func_result<int, F1> >
{
    using ValType = FuncHelpUtils::func_result<int, F1>;
    OutContainer<ValType> result;
    int sign = (inc < 0) ? -1 : 1;
    
    for(int i = start; (i*sign) < (end*sign); i += inc) {
        if (std::ref(predicate)(i)) {
            FuncHelpUtils::addItem(result, std::ref(func)(i));
        }
    }
    
    return result;
}

template <template <class...> class OutContainer,
          class F>
auto mapRange(int start, int end, F func)
-> OutContainer<FuncHelpUtils::func_result<int, F> >
{
    return mapRange<OutContainer>(start, end, 1, func);
}

template <template <class...> class OutContainer,
          class F1,
          class F2>
auto mapRange(int start, int end, F1 func, F2 predicate)
 -> OutContainer<FuncHelpUtils::func_result<int, F1> >
{
    return mapRange<OutContainer>(start, end, 1, func, predicate);
}

template <template <class...> class OutContainer,
          class F>
auto mapRange(int end, F func)
-> OutContainer<FuncHelpUtils::func_result<int, F> >
{
    return mapRange<OutContainer>(0, end, 1, func);
}

template <template <class...> class OutContainer,
          class F1,
          class F2>
auto mapRange(int end, F1 func, F2 predicate)
 -> OutContainer<FuncHelpUtils::func_result<int, F1> >
{
    return mapRange<OutContainer>(0, end, 1, func, predicate);
}

// flatten

template <template <class...> class NestedContainer,
          template <class...> class InContainer,
          template <class...> class OutContainer = NestedContainer,
          class ValType>
auto flatten(const InContainer< NestedContainer<ValType> > &container)
 -> OutContainer<ValType>
{
    OutContainer<ValType> result;
    
    for(auto const &nested : container) {
        for(auto const &val : nested) {
            FuncHelpUtils::addItem(result, val);
        }
    }
    
    return result;
}

template <template <class...> class OutContainer,
          template <class...> class InContainer,
          template <class...> class NestedContainer,
          class ValType,
          class = FuncHelpUtils::enable_if_t<!std::is_same<OutContainer<int>, NestedContainer<int> >::value> >
auto flatten(const InContainer<NestedContainer<ValType> > &container)
 -> OutContainer<ValType>
{
    return flatten<NestedContainer, InContainer, OutContainer>(container);
}

#endif // FUNCTIONAL_HELPERS_H_GUARD
