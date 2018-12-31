# Functional Helpers for C++11

A couple of headers that provide convenient functional-style helper functions that are otherwise missing from C++11. Inspired by Python and libraries like underscore.js. These functions have the following advantages:

- Easy to use: don't require any template arguments or iterators
- Easy to chain: returns new containers rather than modifying existing ones in place
- Flexible: works with any STL container, Qt container, or any other container type that works with a ranged for, and it can use any callable, including lambdas and pointer to member functions

## Contents

* [How to include](#how-to-include)
* [`map`](#map)
* [`filter`](#filter)
* [`reject`](#reject)
* [`compr`](#compr)
* [`allOf`](#allof)
* [`anyOf`](#anyof)
* [`extremum`](#extremum)
* [`min`](#min)
* [`max`](#max)
* [`reduce`](#reduce)
* [`sum`](#sum)
* [`sorted`](#sorted)
* [`contains`](#contains)
* [`omit`](#omit)
* [`reversed`](#reversed)
* [`first`](#first)
* [`last`](#last)
* [`range`](#range)
* [`mapRange`](#maprange)
* [`flatten`](#flatten)
* [Future work and contributing](#future-work-and-contributing)

## How to include

Just include functionalHelpers.h if you want to use it with STL containers. Include qtFunctionalHelpers.h if your project uses Qt's core library and you want to use it with Qt's containers.

## `map`

Usage:

    map(container, callable) -> container of same type
    map<ContainerType>(container, callable) -> ContainerType

Constructs a new container by mapping each value in `container` through a transformation function `callable`. `callable` must take one argument of the same type that is in `container` and return a value. The value type of the new container will be the same as the return type of `callable`.

What kind of container is returned depends on which version of the function is used. For the first form, the same type of container passed into the first parameter is what is returned. The second form returns a container of the type passed in to the template argument.

If applicable, the order of the items in the new container matches the original.

If the type of items in `container` is an object or pointer to an object, `callable` may be a pointer to a member function on the object's class provided it takes no arguments, e.g.:

    QStringList names = getSomeNames();
    map(names, &QString::toLower);

Examples:

    map(std::string("blah!"), toupper);
    // returns std::string("BLAH!")
    
    QList<int> numbers = {1, 2, 3, 4, 5};
    map<QSet>(numbers, [] (int x) { return x*2; });
    // returns: QSet<int>({2, 4, 6, 8, 10})

## `filter`

Usage:

    filter(container, callable) -> container of same type
    filter<ContainerType>(container, callable) -> ContainerType

Constructs a new container containing each value in `container` that passes a truth test `predicate`. `predicate ` must take one argument of the same type that is in `container` and return a boolean. This function is the opposite of `reject`.

What kind of container is returned depends on which version of the function is used. For the first form, the same type of container passed into the first parameter is what is returned. The second form returns a container of the type passed in to the template argument.

If applicable, the order of the items in the new container matches the original.

If the type of items in `container` is an object or pointer to an object, `callable` may be a pointer to a member function on the object's class provided it takes no arguments, e.g.:

    QList<QWidget *> buttons = getSomeButtons();
    QList<QWidget *> visibleButtons = filter(buttons, &QWidget::isVisible);

Examples:

    filter(std::string("abcdeFGHIJ"), islower);
    // returns std::string("abcde")
    
    QList<int> numbers = {1, 2, 3, 4, 5};
    filter<QSet>(numbers, [] (int x) { return (x%2 == 0); });
    // returns: QSet<int>({2, 4})

## `reject`

Usage:

    reject(container, callable) -> container of same type
    reject<ContainerType>(container, predicate) -> ContainerType

Constructs a new container containing each value in `container` that fails a truth test `predicate`. `predicate ` must take one argument of the same type that is in `container` and return a boolean. This function is the opposite of `filter`.

What kind of container is returned depends on which version of the function is used. For the first form, the same type of container passed into the first parameter is what is returned. The second form returns a container of the type passed in to the template argument, and whose value type is the same as `container`.

If applicable, the order of the items in the new container matches the original.

If the type of items in `container` is an object or pointer to an object, `callable` may be a pointer to a member function on the object's class provided it takes no arguments, e.g.:

    QList<QWidget *> buttons = getSomeButtons();
    QList<QWidget *> visibleButtons = filter(buttons, &QWidget::isVisible);

Examples:

    reject(std::string("abcdeFGHIJ"), islower);
    // returns std::string("FGHIJ")
    
    QList<int> numbers = {1, 2, 3, 4, 5};
    reject<QSet>(numbers, [] (int x) { return (x%2 == 0); });
    // returns: QSet<int>({1, 3, 5})

## `compr`

Usage:

    compr(container, callable, predicate) -> container of same type
    compr<ContainerType>(container, callable, predicate) -> ContainerType

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
    
Returns true if all of the values in `container` pass the truth test `predicate`. `predicate ` must take one argument of the same type that is in `container` and return a boolean.

Example:

    bool isLowerCase(const std::string &s) { return s == map(s, tolower); }
    std::list<std::string> words1 = {"these", "are", "words"};
    std::list<std::string> words2 = {"these", "are", "DIFFERENT!!", "words"};
    allOf(words1, isLowerCase); // returns true
    allOf(words2, isLowerCase); // returns false

## `anyOf`

Usage:

    anyOf(container, predicate) -> bool
    
Returns true if at least one of the values in `container` pass the truth test `predicate`. `predicate ` must take one argument of the same type that is in `container` and return a boolean.

Example:

    bool isUpperCase(const std::string &s) { return s == map(s, toupper); }
    std::list<std::string> words1 = {"these", "are", "words"};
    std::list<std::string> words2 = {"these", "are", "DIFFERENT!!", "words"};
    anyOf(words1, isUpperCase); // returns false
    anyOf(words2, isUpperCase); // returns true

## `extremum`

Usage:

    extremum(container, comparator) -> value type of container
    extremum(container, comparator, defaultValue) -> value type of container
    
where `comparator` is a function that returns a bool and has two parameters whose type are the value type of `container`.

Returns the item in container for whom `comparator(item, anyOtherItem)` returns true. If there is no such item, the results are undefined.

If `container` is empty, the first form returns an instance of `container`'s value type using its default constructor (or 0 for a fundamental type). Note that a default value is constructed if and only if `container` is empty.

The second form returns `defaultValue` if `container` is empty.

Example:

    class StylishGarb {
    public:
        StylishGarb(int c=0) : coolness(c) {};
        bool isSwankier(const StylishGarb &other) const { return coolness > other.coolness; };
        int coolness;
    };
    
    std::list<StylishGarb> pimpinOutfit = {StylishGarb(143), StylishGarb(9134), StylishGarb(851495)};
    extremum(pimpinOutfit, &StylishGarb::isSwankier);
    // returns StylushGarb(851495)
    
    extremum(std::list<StylushGarb>(), &StylishGarb::isSwankier);
    // returns StylishGarb(0)
    

## `min`

Usage:

    min(container) -> value type of container
    min(container, defaultValue) -> value type of container
    min(container, callable) -> value type of container
    min(container, callable, defaultValue) -> value type of container
    
Returns the minimum value in `container`. If the first or second forms are used, the value type of `container` must allow use of the `<` operator as it will be used to determine the order.

If the third or fourth form is used and `callable` is provided, it will be used on each value in `container` to generate the criterion by which the value is ranked. The return value of `callable` must allow use of the `<` operator.

If `container` is empty, the first and third forms return an instance of `container`'s value type using its default constructor (or 0 for a fundamental type). Note that a default value is constructed if and only if `container` is empty.

The second and fourth forms return `defaultValue` if `container` is empty.

**Important note:** if the value type of `container` is callable with a single argument whose type is also the value type of `container`, then it is not possible to use the second form of `min`. This is because, when calling `min` with two arguments, the compiler determines whether to use the second or third form of `min` depending on whether the second argument is callable in this manner. See the last two examples below for a demonstration of how this can happen.

Examples:

    std::list<std::string> words = {"once", "upon", "a", "time"};
    min(words);
    // returns "a"

    QList<QList<int> > sequences = {{1, 4, 7, 3},
                                    {6, 9, -10, 4, 5, 12},
                                    {15, -3}};
    min(sequences, &QList<int>::size);
    // returns QList<int>({15, -3})
    
    min(std::list<int>());
    // returns 0
    
    min(std::list<int>(), 123);
    // returns 123
    
    class CallableClass {
    public:
        CallableClass(int a=0) : value(a) {};
        int operator()(const CallableClass &other) const { return -1 * other.value; };
        bool operator<(const CallableClass &other) const { return value < other.value; };
        int value;
    };
    
    min(list<CallableClass>(), CallableClass(123));
    // returns CallableClass(0)
    // Note: does not return CallableClass(123) because the third form of min is
    // being used, not the second
	
    min(list<CallableClass>({CallableClass(1), CallableClass(2)}), CallableClass(0));
    // returns CallableClass(2)
    // Note: does not return CallableClass(1) because the third form of min is
    // being used, and CallableClass::operator()(const CallableClass &other)
    // returns *the opposite* of other.value.

## `max`

Usage:

    max(container) -> value type of container
    max(container, defaultValue) -> value type of container
    max(container, callable) -> value type of container
    max(container, callable, defaultValue) -> value type of container
    
Returns the maximum value in `container`. If the first or second form is used, the value type of `container` must allow use of the `>` operator as it will be used to determine the order.

If the third or fourth form is used and `callable` is provided, it will be used on each value in `container` to generate the criterion by which the value is ranked. The return value of `callable` must allow use of the `>` operator.

If `container` is empty, the first and third forms return an instance of `container`'s value type using its default constructor (or 0 for a fundamental type). Note that a default value is constructed if and only if `container` is empty.

The second and fourth forms return `defaultValue` if `container` is empty.

**Important note:** if the value type of `container` is callable with a single argument whose type is also the value type of `container`, then it is not possible to use the second form of `max`. This is because, when calling `max` with two arguments, the compiler determines whether to use the second or third form of `max` depending on whether the second argument is callable in this manner. See the last two examples below for a demonstration of how this can happen.

Example:

    std::list<std::string> words = {"once", "upon", "a", "time"};
    max(words);
    // returns "upon"

    QList<QList<int> > sequences = {{1, 4, 7, 3},
                                    {6, 9, -10, 4, 5, 12},
                                    {15, -3}};
    max(sequences, &QList<int>::size);
    // returns QList<int>({6, 9, -10, 4, 5, 12})
    
    max(std::list<int>());
    // returns 0
    
    max(std::list<int>(), 123);
    // returns 123
    
    class CallableClass {
    public:
        CallableClass(int a=0) : value(a) {};
        int operator()(const CallableClass &other) const { return -1 * other.value; };
        bool operator<(const CallableClass &other) const { return value < other.value; };
        int value;
    };
    
    max(list<CallableClass>(), CallableClass(123));
    // returns CallableClass(0)
    // Note: does not return CallableClass(123) because the third form of max is
    // being used, not the second
	
    max(list<CallableClass>({CallableClass(1), CallableClass(2)}), CallableClass(0));
    // returns CallableClass(1)
    // Note: does not return CallableClass(1) because the third form of max is
    // being used, and CallableClass::operator()(const CallableClass &other)
    // returns *the opposite* of other.value.

## `reduce`

Usage:

    reduce(container, callable) -> value type of container
    reduce(container, callable, startingValue) -> type of startingValue
    
Reduces `container` to a value which is the result of running each element in `collection` through `callable`, and each iteration is passed in the result of the previous iteration. 

If the first form of `reduce` is used, the first iteration is passed in the first two elements of `container`. `callable` must be a function that takes two parameters whose type is the value type of `container` and whose return value is also the value type of `container`. If passed an empty list, returns an instance of `container`'s value type using its default constructor.

Is the second form is used, the first iteration is passed in the first element of `container` and `startingValue`. `callable` must be a function that takes two parameters where the type of the first parameter is the type of startingValue, the type of the second parameter is the value type of `container`, and whose return value is the type of `startingValue`. If passed an empty list, returns an instance of `startingValue`'s value type using its default constructor.

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
    sorted(QStringList, Qt::CaseSensitivity) -> QStringList
    
Constructs a new container containing the items of `container` in sorted order.

For the first form of `sorted`, items are compared using the `<` operator.

For the second form, items are compared using the function `comparator`. `comparator` must return a `bool` and take two parameters whose type is the value type of `container`.

The third form is a convenience function that allows sorting QStringLists in a case-insensitive fashion.

Example:

    QStringList words = {"Once", "upon", "a", "time"};
    sorted(words);
    // Returns QStringList({"Once", "a", "time", "upon"})
    
    sorted(words, Qt::CaseInsensitive);
    // Returns QStringList({"a", "Once", "time", "upon"})
    
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
    omit<ContainerType>(container1, value) -> ContainerType
    omit<ContainerType>(container1, container2) -> ContainerType
    
If the second parameter is not a container type (i.e. does not define a `const_iterator`), constructs a new container that contains each value in `container1` that is not equal to the second parameter `value`.

If the second parameter is a container type, constructs a new container that contains each value in `container1` that is not equal to any value in `container2`.

What kind of container is returned depends on which version of the function is used. For the first two forms, the same type of container passed into the first parameter is what is returned. The last two forms returns a container of the type passed in to the template argument.

If applicable, the order of the items in the new container matches the original.

Comparison is done using the `==` operator.

Example:

    omit(std::list<int>({1, 2, 3, 4, 5}), 3);
    // returns std::list<int>({1, 2, 4, 5})
    
    std::set<int> exclude = {1, 2, 3};
    omit(std::list<int>({1, 2, 3, 4, 5}), exclude);
    // returns std::list<int>({4, 5})

## `reversed`

Usage:

    reversed(container) -> container of same type
    
Returns a new container that contains all the items of `container` but in reverse order.

Note: you can pass unordered containers (like `set`) into this function and it won't result in a compiler error, though doing so will of course return an identical container.

Example:

    reversed(QList<int>({1, 2, 3, 4, 5});
    // Returns QList<int>({5, 4, 3, 2, 1})

## `first`

Usage:

    first(container) -> value type of container
    first(container, defaultValue) -> value type of container
    
Returns the first element of `container` with bounds checking.

If `container` is empty, the first form returns an instance of `container`'s value type using its default constructor (or 0 for a fundamental type). Note that a default value is constructed if and only if `container` is empty.

The second form returns `defaultValue` if `container` is empty.

Note: you can pass unordered containers (like `set`) into this function and it may not result in a compiler error, though its return value will be an undefined and probably arbitrary element in `container`.

Example:

    first(std::list<int>({1, 2, 3, 4, 5});
    // Returns 1

    first(std::list<int>());
    // Returns 0

    first(std::list<int>(), 42);
    // Returns 42

## `last`

Usage:

    last(container) -> value type of container
    last(container, defaultValue) -> value type of container

Returns the last element of `container` with bounds checking.

If `container` is empty, the first form returns an instance of `container`'s value type using its default constructor (or 0 for a fundamental type). Note that a default value is constructed if and only if `container` is empty.

The second form returns `defaultValue` if `container` is empty.

Note: you can pass unordered containers (like `set`) into this function and it may not result in a compiler error, though its return value will be an undefined and probably arbitrary element in `container`.

Example:

    last(QList<int>({1, 2, 3, 4, 5});
    // Returns 5

    last(QList<int>());
    // Returns 0

    last(QList<int>(), 42);
    // Returns 42
    
## `range`

Usage:

    range<ContainerType>(start, end, increment) -> ContainerType<int>
    range<ContainerType>(start, end) -> ContainerType <int>
    range<ContainerType>(end) -> ContainerType<int>
    
Inspired by the `range` function in python, returns a container containing an arithmetic progression similar to what one would see in a basic `for` loop. Specifically, the result of `range` can be defined using the following psuedo-code for when `inc` is positive:

    for(int i = start; i < end; i += inc)
        list.append(i);

When `inc` is negative, the conditional changes to `i > end`.

If `inc` is omitted, it defaults to 1. If `start` is omitted, it defaults to 0.

What kind of container is returned depends on what is passed into the template argument.

Example:

    range<std::list>(10) // returns std::list<int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9})
    range<std::vector>(1, 6) // returns std::vector<int>({1, 2, 3, 4, 5})
    range<QList>(5, 0, -1) // returns QList<int>({5, 4, 3, 2, 1})

## `mapRange`

Usage:

     mapRange<ContainerType>(start, end, increment, callable) -> ContainerType
     mapRange<ContainerType>(start, end, callable) -> ContainerType
     mapRange<ContainerType>(end, callable) -> ContainerType
     mapRange<ContainerType>(start, end, increment, callable, predicate) -> ContainerType
     mapRange<ContainerType>(start, end, callable, predicate) -> ContainerType
     mapRange<ContainerType>(end, callable, predicate) -> ContainerType

A convenience function that is the equivalent of:

    map(filter(range(start, end, increment), predicate), callable)
    
or, if `predicate` is omitted:

    map(range(start, end, increment), callable)
    
but is more efficient as only one container is constructed.

As with `range`, when increment is omitted, it defaults to 1, and when start is omitted, it defaults to 0.

What kind of container is returned depends on what is passed into the template argument.

Examples:

    mapRange<std::list>(5, [] (int x) { return x*x; });
    // returns std::list({0, 1, 4, 9, 16})
    mapRange<std::vector>(-1, -6, -1, [] (int x) { return x*x; });
    // returns std::vector({1, 4, 9, 16, 25})
    mapRange<QList>(1, 15, [] (int x) { return x*x; }, [] (int x) { return (x%3) != 0; });
    // returns QList((1, 4, 16, 25, 49, 64, 100, 121, 169, 196))

## `flatten`

Usage:

    flatten(container of containers) -> container of same type as inner container
    flatten<ContainerType>(container of containers) -> ContainerType

Constructs a new container by adding all of the inner items of a container of containers.

What kind of container is returned depends on which version of the function is used. For the first form `flatten`, the return value is the same type as the inner container, i.e. if a value of type `ListType1<ListType2<ValueType> >` is passed in, then the result is `ListType2<ValueType>`. The second form return a container based off of the template argument, so the container passed in doesn't have to be of the same type.

If applicable, the order of the items in the new container matches the original, e.g. `flatten(std::list<std::list<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}))` returns `std::list<int>({1, 2, 3, 4, 5, 6, 7, 8, 9})`.

Examples:

    flatten(QList< QVector<int> >({{1, 2}, {3, 4, 5}}))
    // returns QVector<int>({1, 2, 3, 4, 5})
    
    flatten<std::set>(std::list< std::vector<int> >({{1, 2}, {3, 4}, {5, 6}}))
    // returns std::set<int>({1, 2, 3, 4, 5, 6})
    
    flatten(std::vector< std::vector<int> >())
    // returns std::vector<int>()

## Future work and contributing

Part of the point of this project was to familiarize myself with some of the more esoteric aspects of C++11. (And boy howdy does it get esoteric.) If there's a more efficient or elegant way to implement any of these functions, I'd love to see it! Also, while I tried to keep performance in mind when writing these, I'm not 100% sure I got it right, particularly when it comes to C++11's automatic move semantics and such. So if there's an error there I'd be happy to see it corrected.

And obviously, I'll be adding more functions to this bit by bit.
