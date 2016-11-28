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
* [`contains`](#contains)
* [`omit`](#omit)
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

What kind of container is returned depends on which version of the function is used. For the first form `map`, the same type of container passed into the first parameter is what is returned. The remaining forms return a container based off of their name, so the container passed in doesn't have to be of the same type.

If applicable, the order of the items in the new container matches the original.

If the type of items in `container` is an object or pointer to an object, `callable` may be a pointer to a member function on the object's class provided it takes no arguments, e.g.:

    QStringList names = getSomeNames();
    map(names, &QString::toLower);

Examples:

    map(std::string("blah!"), toupper);
    // returns std::string("BLAH!")
    
    QList<int> numbers = {1, 2, 3, 4, 5};
    QSetMap(numbers, [] (int x) { return x*2; });
    // returns: QSet<int>({2, 4, 6, 8, 10})

## `filter`

Usage:

    filter(container, callable) -> container of same type
    listFilter(container, predicate) -> std::list
    vectorFilter(container, predicate) -> std::vector
    setFilter(container, predicate) -> std::set
    QListFilter(container, predicate) -> QList
    QVectorFilter(container, predicate) -> QVector
    QSetFilter(container, predicate) -> QSet
    QLinkedListFilter(container, predicate) -> QLinkedList

Constructs a new container containg each value in `container` test passes a truth test `predicate`. `callable` must take one argument of the same type that is in `container` and return a boolean.

What kind of container is returned depends on which version of the function is used. For the first form `filter`, the same type of container passed into the first parameter is what is returned. The remaining forms return a container based off of their name, so the container passed in doesn't have to be of the same type.

If applicable, the order of the items in the new container matches the original.

If the type of items in `container` is an object or pointer to an object, `callable` may be a pointer to a member function on the object's class provided it takes no arguments, e.g.:

    QList<QWidget *> buttons = getSomeButtons();
    QList<QWidget *> visibleButtons = filter(buttons, &QWidget::isVisible);

Examples:

    filter(std::string("abcdeFGHJI"), islower);
    // returns std::string("abcde")
    
    QList<int> numbers = {1, 2, 3, 4, 5};
    QSetFilter(numbers, [] (int x) { return (x%2 == 0); });
    // returns: QSet<int>({2, 4})

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
    std::list<int> oddNumbersDoubled = compr(numbers,
                                             [] (int x) { return x*2; },
                                             [] (int x) { return (x%2) == 1; });
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

Usage:

    extremum(container, comparator) -> value type of container
    
where `comparator` is a function that returns a bool and has two parameters whose type are the value type of `container`.

Returns the item in container for whom `comparator(item, anyOtherItem)` returns true. If there is no such item, the results are undefined.

Example:

    class StylishGarb {
    public:
        StylishGarb(int c) : coolness(c) {};
        int isSwankier(const StylishGarb &other) const { return coolness > other.coolness; };
        int coolness;
    };
    
    std::list<StylishGarb> pimpinOutfit = {StylishGarb(143), StylishGarb(9134), StylishGarb(851495)};
    extremum(pimpinOutfit, &StylishGarb::isSwankier);
    // returns StylushGarb(851495);

## `min`

Usage:

    min(container) -> value type of container
    min(container, callable) -> value type of container
    
Returns the minimum value in `container`. If the first form is used, the value type of `container` must allow use of the `<` operator as it will be used to determine the order.

If the second form is used and `callable` is provided, it will be used on each value in `container` to generate the criterion by which the value is ranked. The return value of `callable` must allow use of the `<` operator.

Example:

    std::list<std::string> words = {"once", "upon", "a", "time"};
    min(words);
    // returns "a"

    QList<QList<int> > sequences = {{1, 4, 7, 3},
                                    {6, 9, -10, 4, 5, 12},
                                    {15, -3}};
    min(sequences, &QList<int>::size);
    // returns QList<int>({15, -3})

## `max`

Usage:

    max(container) -> value type of container
    max(container, callable) -> value type of container
    
Returns the maximum value in `container`. If the first form is used, the value type of `container` must allow use of the `>` operator as it will be used to determine the order.

If the second form is used and `callable` is provided, it will be used on each value in `container` to generate the criterion by which the value is ranked. The return value of `callable` must allow use of the `>` operator.

Example:

    std::list<std::string> words = {"once", "upon", "a", "time"};
    max(words);
    // returns "upon"

    QList<QList<int> > sequences = {{1, 4, 7, 3},
                                    {6, 9, -10, 4, 5, 12},
                                    {15, -3}};
    max(sequences, &QList<int>::size);
    // returns QList<int>({6, 9, -10, 4, 5, 12})

## `reduce`

Usage:

    reduce(container, callable) -> value type of container
    reduce(container, callable, startingValue) -> type of startingValue
    
Reduces `container` to a value which is the result of running each element in `collection` through `callable`, and each iteration is passed in the result of the previous iteration. 

If the first form of `reduce` is used, the first iteration is passed in the first two elements of `container`. `callable` must be a function that takes two parameters whose type is the value type of `container` and whose return value is also the value type of `container`.

Is the second form is used, the first iteration is passed in the first element of `container` and `startingValue`. `callable` must be a function that takes two parameters where the type of the first parameter is the type of startingValue, the type of the second parameter is the value type of `container`, and whose return value is the type of `startingValue`.

Example:

    std::list<int> numbers = {1, 2, 3, 4, 5};
    reduce(numbers, [] (int x, y) { return x*y; });
    // returns 120
    
    QList<int> numbers = {1, 2, 3, 4, 5};
    reduce(numbers, [] (const QString &a, int b) { return a + QString::number(b); }, QString("concatenated: "));
    // returns QString("concatenated: 12345")

## `sum`

Usage:

    sum(container) -> value type of container
    sum(container, startingValue) -> value type of container
    
A convenience function that is equivalent to:

    reduce(container, [] (const value_type &a, const value_type &b) { return a+b; })
    
or

    reduce(container, [] (const value_type &a, const value_type &b) { return a+b; }, startingValue)
    
Example:

    std::list<int> numbers = {1, 2, 3, 4, 5};
    sum(numbers);
    // Returns 15

    sum(numbers, 100);
    // Returns 115

## `sorted`

Usage:

    sorted(container) -> container of same type
    sorted(container, comparator) -> container of same type
    
Constructs a new container containing the items of `container` in sorted order.

For the first form of `sorted`, items are compared using the `<` operator.

For the second form, items are compared using the function `comparator`. `comparator` must return a `bool` and take two parameters whose type is the value type of `container`.

Example:

    QStringList words = {"once", "upon", "a", "time"};
    sorted(words);
    // Returns QStringList({"a", "once", "time", "upon"})
    
    QStringList numbers = {"12", "354", "2", "46"};
    sorted(numbers);
    // Returns QStringList({"12", "2", "354", "46"})
    
    sorted(numbers, [] (const QString &a, const QString &b) { return a.toInt() < b.toInt(); });
    // Returns QStringList(("2", "12", "46", "354"))

## `contains`

Usage:

    contains(container, value) -> bool
    
Returns true if `container` has at least one item that is equal to `value`. Comparison is done using the `==` operator.

Example:

    contains(std::list({1, 2, 3, 4, 5}), 2);
    // Returns true

## `omit`

Usage:

    omit(container1, value) -> container of same type as container1
    omit(container1, container2) -> container of same type as container1
    listOmit(container1, value) -> std::list
    listOmit(container1, container2) -> std::list
    vectorOmit(container1, value) -> std::vector
    vectorOmit(container1, container2) -> std::vector
    setOmit(container1, value) -> std::set
    setOmit(container1, container2) -> std::set
    QListOmit(container1, value) -> QList
    QListOmit(container1, container2) -> QList
    QVectorOmit(container1, value) -> QVector
    QVectorOmit(container1, container2) -> QVector
    QSetOmit(container1, value) -> QSet
    QSetOmit(container1, container2) -> QSet
    QLinkedListOmit(container1, value) -> QLinkedList
    QLinkedListOmit(container1, container2) -> QLinkedList
    
If the second parameter is not a container type (i.e. does not define a `const_iterator`), constructs a new container that contains each value in `container1` that is not equal to the second parameter `value`.

If the second parameter is a container type, constructs a new container that contains each value in `container1` that is not equal to any value in `container2`.

What kind of container is returned depends on which version of the function is used. For the first two forms `omit`, the same type of container passed into the first parameter is what is returned. The remaining forms return a container based off of their name, so `container1` doesn't have to be of the same type.

If applicable, the order of the items in the new container matches the original.

Comparison is done using the `==` operator.

Example:

    omit(std::list<int>({1, 2, 3, 4, 5}), 3);
    // returns std::list<int>({1, 2, 4, 5})
    
    std::set<int> exclude = {1, 2, 3};
    omit(std::list<int>({1, 2, 3, 4, 5}), exclude);
    // return std::list<int>({4, 5})


## Future work and contributing

Part of the point of this project was to familiarize myself with some of the more esoteric aspects of C++11. (And boy howdy does it get esoteric.) If there's a more efficient or elegant way to implement any of these functions, I'd love to see it! Also, while I tried to keep performance in mind when writing these, I'm not 100% sure I got it right, particularly when it comes to C++11's automatic move semantics and such. So if there's an error there I'd be happy to see it corrected.

Not every STL and Qt container is currently supported -- just the ones I use a lot -- particularly for the functions that return a specific type based on their name (like listMap, vectorMap, etc). However it should in most cases be trivially easy to support another container if it uses STL-style iterators and implements a `begin`, `end`, `cbegin`, and `cend` methods like Qt's containers do. That said, these functions could be rewritten to be more agnostic about which method is used for iterator over the containers.

And obviously, I'll be adding more functions to this bit by bit.