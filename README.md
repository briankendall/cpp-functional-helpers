# Functional Helpers for C++11

A couple of headers that provide convenient functional-style helper functions that are otherwise missing from C++11. Inspired by Python and libraries like underscore.js. These functions have the following advantages:

- Easy to use: don't require any template arguments or iterators
- Easy to chain: returns new containers rather than modifying existing ones in place
- Flexible: works with any STL or Qt container (or can be modified to do so easily) and can use any callable, including lambdas and pointer to member functions

## Contents

* [How to include](#how-to-include)
* [`map`](#map)
* [`filter`](#filter)
* [`compr`](#compr)
* [`allOf`](#allOf)
* [`anyOf`](#anyOf)
* [`extremum`](#extremum)
* [`min`](#min)
* [`max`](#max)
* [`reduce`](#reduce)
* [`sum`](#sum)
* [`sorted`](#sorted)
* [Future work and contributing](#future-work-and-contributing)

## How to include

Just include functionalHelpers.h if you want to use it with STL containers. Include qtFunctionalHelpers.h if your project uses Qt's core library and you want to use it with Qt's containers.

## `map`

Usage:

    map(container, callable) -> container of same type
    listMap(container, callable) -> std::list
    vectorMap(container, callable) -> std::vector
    setMap(container, callable) -> std::set
    QListMap(container, callable) -> QList
    QVectorMap(container, callable) -> QVector
    QSetMap(container, callable) -> QSet
    QLinkedListMap(container, callable) -> QLinkedList

Constructs a new container by mapping each value in `container` through a transformation function `callable`. `callable` must take one argument of the same type that is in `container`.

What kind of container is returned depends on which version of the function is used. For the first form `map`, the same type of container passed into the first argument is what is returned. The remaining forms return a container based off of their name, so the container passed in doesn't have to be of the same type.

If applicable, the order of the items in the new container matches the original.

If the type of items in `container` is an object or pointer to an object, `callable` may be a pointer to a member function on the object's class provided it takes no arguments, e.g.:

    QStringList names = getSomeNames();
    map(names, &QString::toLower);

## `filter`

Usage:

    listFilter(container, predicate) -> std::list
    vectorFilter(container, predicate) -> std::vector
    setFilter(container, predicate) -> std::set
    QListFilter(container, predicate) -> QList
    QVectorFilter(container, predicate) -> QVector
    QSetFilter(container, predicate) -> QSet
    QLinkedListFilter(container, predicate) -> QLinkedList

Constructs a new container containg each value in `container` test passes a truth test `predicate`. `callable` must take one argument of the same type that is in `container` and return a boolean. What kind of container is returned depends on which version of the function is used. If applicable, the order of the items in the new container matches the original.

If the type of items in `container` is an object or pointer to an object, `callable` may be a pointer to a member function on the object's class provided it takes no arguments, e.g.:

    QList<QWidget *> buttons = getSomeButtons();
    QList<QWidget *> visibleButtons = QListFilter(buttons, &QWidget::isVisible);

## `compr`

Usage:

    listCompr(container, callable, predicate) -> std::list
    vectorCompr(container, callable, predicate) -> std::vector
    setCompr(container, callable, predicate) -> std::set
    QListCompr(container, callable, predicate) -> QList
    QVectorCompr(container, callable, predicate) -> QVector
    QSetCompr(container, callable, predicate) -> QSet
    QLinkedListCompr(container, callable, predicate) -> QLinkedList

Inspired by Python's list comprehensions (hence 'compr'), this is a convenience function that is equivalent to:

    map(filter(container, predicate), callable)

Technically it's slightly more efficient in that as only one structure is constructed. Like the above functions, order is maintained when applicable, and pointers to member functions can be used.

Example:

    std::list<int> numbers = {1, 2, 3, 4, 5};
    std::list<int> oddNumbersDoubled = listCompr(numbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 1; });
    // returns: {2, 6, 10}

## `allOf`

Usage:

    allOf(container, predicate) -> bool
    
Returns true if all of the values in `container` pass the truth test `predicate`.

Example:

    bool isLowerCase(const std::string &s) { return s == map(s, tolower); }
    std::list<std::string> words1 = {"these", "are", "words"};
    std::list<std::string> words2 = {"these", "are", "DIFFERENT!!", "words"};
    allOf(words1, isLowerCase); // returns true
    allOf(words2, isLowerCase); // returns false

## `anyOf`

Usage:

    anyOf(container, predicate) -> bool
    
Returns true if at least one of the values in `container` pass the truth test `predicate`.

Example:

    bool isUpperCase(const std::string &s) { return s == map(s, toupper); }
    std::list<std::string> words1 = {"these", "are", "words"};
    std::list<std::string> words2 = {"these", "are", "DIFFERENT!!", "words"};
    anyOf(words1, isUpperCase); // returns false
    anyOf(words2, isUpperCase); // returns true

## `extremum`

Docs coming soon...

## `min`

Docs coming soon...

## `max`

Docs coming soon...

## `reduce`

Docs coming soon...

## `sum`

Docs coming soon...

## `sorted`

Docs coming soon...

## Future work and contributing

Part of the point of this project was to familiarize myself with some of the more esoteric aspects of C++11. (And boy howdy does it get esoteric.) If there's a more efficient or elegant way to implement any of these functions, I'd love to see it! Also, while I tried to keep performance in mind when writing these, I'm not 100% sure I got it right, particularly when it comes to C++11's automatic move semantics and such. So if there's an error there I'd be happy to see it corrected.

Not every STL and Qt container is currently supported -- just the ones I use a lot -- particularly for the functions that return a specific type based on their name (like listMap, vectorMap, etc). However it should in most cases be trivially easy to support another container.

And obviously, I'll be adding more functions to this bit by bit.