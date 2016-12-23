#include <qtFunctionalHelpers.h>
#include <qDebug>

// A quick set of tests for functionHelpers.h and qtFunctionalHelpers.h
// Mainly used to check for compile errors. Also checks for correct results at runtime, but
// admittedly not as thoroughly as it could.

using namespace std;

#define TEST(expr, val) \
    if (!((expr) == (val))) { \
        qDebug() << "Failure! line" << __LINE__ << #expr; \
    } else { \
        passedTests += 1; \
    } \
    totalTests += 1;

int timesTwo(int x) { return x*2; }
int timesX(int x, int y) { return x*y; }
bool isEven(int x) { return (x%2) == 0; }
bool isMultiple(int x, int y) { return (x%y) == 0; }
bool isLessThan(int x, int y) { return x < y; }

class BaseFoo {
public:
    BaseFoo(int a=0) : value(a) {};
    int baseGetValue() const { return value; };
    int baseFooTimesTwo() const { return value*2; };
    bool baseIsEven() const { return (value%2) == 0; };
    bool baseIsLessThan(const BaseFoo &other) const { return value < other.value; }
    bool baseIsLessThanPtr(const BaseFoo *other) const { return value < other->value; }
    BaseFoo baseFooTimesFoo(const BaseFoo &other) const { return BaseFoo(value*other.value); };
    int value;
};

class Foo : public BaseFoo {
public:
    Foo(int a=0) : BaseFoo(a) {};
    int getValue() const { return baseGetValue(); };
    int fooTimesTwo() const { return getValue()*2; };
    Foo fooTimesFoo(const Foo &other) const { return Foo(getValue()*other.getValue()); };
    bool isEven() const { return ::isEven(getValue()); };
    bool isLessThan(const Foo &other) const { return getValue() < other.getValue(); }
    bool isLessThanPtr(const Foo *other) const { return getValue() < other->getValue(); }
    bool operator==(const Foo &other) const { return getValue() == other.getValue(); }
    bool operator<(const Foo &other) const { return isLessThan(other); }
    bool operator>(const Foo &other) const { return getValue() > other.getValue(); }
    Foo operator+(const Foo &other) const { return Foo(getValue() + other.getValue()); }
};

class DerivedList : public list<Foo> {
public:
    DerivedList(const list<Foo> &other) : list<Foo>(other) {};
    DerivedList(initializer_list<Foo> args) : list<Foo>(args) {};
};

class DerivedQList : public QList<Foo> {
public:
    DerivedQList(initializer_list<Foo> args) : QList<Foo>(args) {};
};

class DerivedPtrList : public list<Foo *> {
public:
    DerivedPtrList(initializer_list<Foo *> args) : list<Foo *>(args) {};
};

class DerivedQPtrList : public QList<Foo *> {
public:
    DerivedQPtrList(initializer_list<Foo *> args) : QList<Foo *>(args) {};
};

const list<int> listNumbers = {1,2,3,4,5};
const vector<int> vectorNumbers = {1,2,3,4,5};
const set<int> setNumbers = {1,2,3,4,5};
const QList<int> QListNumbers = {1,2,3,4,5};
const QVector<int> QVectorNumbers = {1,2,3,4,5};
const QSet<int> QSetNumbers = {1,2,3,4,5};
const QLinkedList<int> QLinkedListNumbers = {1,2,3,4,5};

const list<int> listEvenNumbers = {2,4,6,8,10};
const list<int> listOddNumbers = {1,3,5,7,9};

Foo fooA(1), fooB(2), fooC(3), fooD(4), fooE(5);
const list<Foo> listFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const list<Foo *> listFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};
const vector<Foo> vectorFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const vector<Foo *> vectorFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};
const QList<Foo> QListFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const QList<Foo *> QListFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};
const QVector<Foo> QVectorFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const QVector<Foo *> QVectorFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};
const DerivedList derivedListFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const DerivedQList derivedQListFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const DerivedPtrList derivedListFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};
const DerivedQPtrList derivedQListFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};

int passedTests = 0;
int totalTests = 0;

void testMap()
{
    const list<int> listExpected = listEvenNumbers;
    const vector<int> vectorExpected = {2,4,6,8,10};
    const set<int> setExpected = {2,4,6,8,10};
    const QList<int> QListExpected = {2,4,6,8,10};
    const QVector<int> QVectorExpected = {2,4,6,8,10};
    const QSet<int> QSetExpected = {2,4,6,8,10};
    const QLinkedList<int> QLinkedListExpected = {2,4,6,8,10};
    
    TEST(::map(listNumbers, [] (int x) { return x*2; }), listExpected);
    TEST(::map(listNumbers, &timesTwo), listExpected);
    TEST(::map(listNumbers, bind(timesX, 2, placeholders::_1)), listExpected);
    TEST(::map(listFoos, [] (const Foo &foo) { return foo.fooTimesTwo(); }), listExpected);
    TEST(::map(listFoos, &Foo::fooTimesTwo), listExpected);
    TEST(::map(listFooPtrs, &Foo::fooTimesTwo), listExpected);
    TEST(::map(QListFoos, &Foo::fooTimesTwo), QListExpected);
    TEST(::map(QListFooPtrs, &Foo::fooTimesTwo), QListExpected);
    TEST(::map(listFoos, &Foo::baseFooTimesTwo), listExpected);
    TEST(::map(listFooPtrs, &Foo::baseFooTimesTwo), listExpected);
    TEST(::map(QListFoos, &Foo::baseFooTimesTwo), QListExpected);
    TEST(::map(QListFooPtrs, &Foo::baseFooTimesTwo), QListExpected);
    TEST(::map(derivedListFoos, &Foo::baseFooTimesTwo), listExpected);
    TEST(::map(derivedQListFoos, &Foo::baseFooTimesTwo), QListExpected);
    
    TEST(listMap(listFoos, [] (const Foo &foo) { return foo.fooTimesTwo(); }), listExpected);
    TEST(listMap(listFoos, &Foo::fooTimesTwo), listExpected);
    TEST(listMap(listFooPtrs, &Foo::fooTimesTwo), listExpected);
    TEST(QListMap(QListFoos, &Foo::fooTimesTwo), QListExpected);
    TEST(QListMap(QListFooPtrs, &Foo::fooTimesTwo), QListExpected);
    TEST(listMap(listFoos, &Foo::baseFooTimesTwo), listExpected);
    TEST(listMap(listFooPtrs, &Foo::baseFooTimesTwo), listExpected);
    TEST(QListMap(QListFoos, &Foo::baseFooTimesTwo), QListExpected);
    TEST(QListMap(QListFooPtrs, &Foo::baseFooTimesTwo), QListExpected);
    TEST(listMap(derivedListFoos, &Foo::baseFooTimesTwo), listExpected);
    TEST(QListMap(derivedQListFoos, &Foo::baseFooTimesTwo), QListExpected);
    
    TEST(::map(vectorNumbers, [] (int x) { return x*2; }), vectorExpected);
    TEST(::map(setNumbers, [] (int x) { return x*2; }), setExpected);
    TEST(::map(QListNumbers, [] (int x) { return x*2; }), QListExpected);
    TEST(::map(QVectorNumbers, [] (int x) { return x*2; }), QVectorExpected);
    TEST(::map(QSetNumbers, [] (int x) { return x*2; }), QSetExpected);
    TEST(::map(QLinkedListNumbers, [] (int x) { return x*2; }), QLinkedListExpected);
    TEST(vectorMap(listNumbers, [] (int x) { return x*2; }), vectorExpected);
    TEST(setMap(listNumbers, [] (int x) { return x*2; }), setExpected);
    TEST(QListMap(listNumbers, [] (int x) { return x*2; }), QListExpected);
    TEST(QVectorMap(listNumbers, [] (int x) { return x*2; }), QVectorExpected);
    TEST(QSetMap(listNumbers, [] (int x) { return x*2; }), QSetExpected);
    TEST(QLinkedListMap(listNumbers, [] (int x) { return x*2; }), QLinkedListExpected);
    
    // toupper takes an integer and returns an integer, so in order to use it with ::map we have
    // to cast it to the right kind of function. Probably not the best way to go but it works.
    TEST(::map(string("abcde"), (char(*)(char))(toupper)), string("ABCDE"));
    TEST(::map(string("abcde"), [] (char c) { return char(toupper(c)); }), string("ABCDE"));
    TEST(stringMap(string("abcde"), toupper), string("ABCDE"));
    TEST(QStringMap(string("abcde"), [] (char c) { return QChar(c).toUpper();}), QString("ABCDE"));
    TEST(QStringMap(QString("abcde"), [] (QChar c) { return c.toUpper();}), QString("ABCDE"));
}

void testCompr()
{
    const list<int> listExpected = {4,8};
    const vector<int> vectorExpected = {4,8};
    const set<int> setExpected = {4,8};
    const QList<int> QListExpected = {4,8};
    const QVector<int> QVectorExpected = {4,8};
    const QSet<int> QSetExpected = {4,8};
    const QLinkedList<int> QLinkedListExpected = {4,8};
    
    TEST(compr(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), listExpected);
    TEST(compr(listNumbers, &timesTwo, &isEven), listExpected);
    TEST(compr(listNumbers, bind(timesX, 2, placeholders::_1), bind(isMultiple, placeholders::_1, 2)),
         listExpected);
    TEST(compr(listFoos, &Foo::fooTimesTwo, &Foo::isEven), listExpected);
    TEST(compr(listFoos, &Foo::fooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), listExpected);
    TEST(compr(listFoos, [] (const Foo &a) { return a.fooTimesTwo(); }, &Foo::isEven), listExpected);
    TEST(compr(listFooPtrs, &Foo::fooTimesTwo, &Foo::isEven), listExpected);
    TEST(compr(listFooPtrs, &Foo::fooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), listExpected);
    TEST(compr(listFooPtrs, [] (const Foo *a) { return a->fooTimesTwo(); }, &Foo::isEven), listExpected);
    TEST(compr(QListFoos, &Foo::fooTimesTwo, &Foo::isEven), QListExpected);
    TEST(compr(QListFoos, &Foo::fooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), QListExpected);
    TEST(compr(QListFoos, [] (const Foo &a) { return a.fooTimesTwo(); }, &Foo::isEven), QListExpected);
    TEST(compr(QListFooPtrs, &Foo::fooTimesTwo, &Foo::isEven), QListExpected);
    TEST(compr(QListFooPtrs, &Foo::fooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), QListExpected);
    TEST(compr(QListFooPtrs, [] (const Foo *a) { return a->fooTimesTwo(); }, &Foo::isEven), QListExpected);
    TEST(compr(derivedListFoos, &Foo::fooTimesTwo, &Foo::isEven), listExpected);
    TEST(compr(derivedQListFoos, &Foo::fooTimesTwo, &Foo::isEven), QListExpected);
    
    TEST(listCompr(listFoos, &Foo::fooTimesTwo, &Foo::isEven), listExpected);
    TEST(listCompr(listFoos, &Foo::fooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), listExpected);
    TEST(listCompr(listFoos, [] (const Foo &a) { return a.fooTimesTwo(); }, &Foo::isEven), listExpected);
    TEST(listCompr(listFooPtrs, &Foo::fooTimesTwo, &Foo::isEven), listExpected);
    TEST(listCompr(listFooPtrs, &Foo::fooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), listExpected);
    TEST(listCompr(listFooPtrs, [] (const Foo *a) { return a->fooTimesTwo(); }, &Foo::isEven), listExpected);
    TEST(QListCompr(QListFoos, &Foo::fooTimesTwo, &Foo::isEven), QListExpected);
    TEST(QListCompr(QListFoos, &Foo::fooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), QListExpected);
    TEST(QListCompr(QListFoos, [] (const Foo &a) { return a.fooTimesTwo(); }, &Foo::isEven), QListExpected);
    TEST(QListCompr(QListFooPtrs, &Foo::fooTimesTwo, &Foo::isEven), QListExpected);
    TEST(QListCompr(QListFooPtrs, &Foo::fooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), QListExpected);
    TEST(QListCompr(QListFooPtrs, [] (const Foo *a) { return a->fooTimesTwo(); }, &Foo::isEven), QListExpected);
    TEST(listCompr(derivedListFoos, &Foo::fooTimesTwo, &Foo::isEven), listExpected);
    TEST(QListCompr(derivedQListFoos, &Foo::fooTimesTwo, &Foo::isEven), QListExpected);
    
    TEST(listCompr(listFoos, &Foo::baseFooTimesTwo, &Foo::baseIsEven), listExpected);
    TEST(listCompr(listFoos, &Foo::baseFooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), listExpected);
    TEST(listCompr(listFoos, [] (const Foo &a) { return a.baseFooTimesTwo(); }, &Foo::baseIsEven), listExpected);
    TEST(listCompr(listFooPtrs, &Foo::baseFooTimesTwo, &Foo::baseIsEven), listExpected);
    TEST(listCompr(listFooPtrs, &Foo::baseFooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), listExpected);
    TEST(listCompr(listFooPtrs, [] (const Foo *a) { return a->baseFooTimesTwo(); }, &Foo::baseIsEven), listExpected);
    TEST(QListCompr(QListFoos, &Foo::baseFooTimesTwo, &Foo::baseIsEven), QListExpected);
    TEST(QListCompr(QListFoos, &Foo::baseFooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), QListExpected);
    TEST(QListCompr(QListFoos, [] (const Foo &a) { return a.baseFooTimesTwo(); }, &Foo::baseIsEven), QListExpected);
    TEST(QListCompr(QListFooPtrs, &Foo::baseFooTimesTwo, &Foo::baseIsEven), QListExpected);
    TEST(QListCompr(QListFooPtrs, &Foo::baseFooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), QListExpected);
    TEST(QListCompr(QListFooPtrs, [] (const Foo *a) { return a->baseFooTimesTwo(); }, &Foo::baseIsEven), QListExpected);
    
    TEST(compr(vectorNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), vectorExpected);
    TEST(compr(setNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), setExpected);
    TEST(compr(QListNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QListExpected);
    TEST(compr(QVectorNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QVectorExpected);
    TEST(compr(QSetNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QSetExpected);
    TEST(compr(QLinkedListNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QLinkedListExpected);
    
    TEST(vectorCompr(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), vectorExpected);
    TEST(setCompr(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), setExpected);
    TEST(QListCompr(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QListExpected);
    TEST(QVectorCompr(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QVectorExpected);
    TEST(QSetCompr(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QSetExpected);
    TEST(QLinkedListCompr(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}),
         QLinkedListExpected);
    
    TEST(compr(std::string("aBcDeFgH"), (char(*)(char))(tolower), isupper), std::string("bdfh"));
    TEST(stringCompr(std::string("aBcDeFgH"), tolower, isupper), std::string("bdfh"));
    TEST(QStringCompr(QString("aBcDeFgH"), [] (QChar c) { return c.toLower(); },
         [] (QChar c) { return c.isUpper(); }), QString("bdfh"));
}

void testFilter()
{
    const list<int> listExpected = {2,4};
    const vector<int> vectorExpected = {2,4};
    const set<int> setExpected = {2,4};
    const QList<int> QListExpected = {2,4};
    const QVector<int> QVectorExpected = {2,4};
    const QSet<int> QSetExpected = {2,4};
    const QLinkedList<int> QLinkedListExpected = {2,4};
    
    TEST(listFilter(listNumbers, [] (int x) { return (x%2) == 0; }), listExpected);
    TEST(listFilter(listNumbers, &isEven), listExpected);
    TEST(listFilter(listNumbers, bind(isMultiple, placeholders::_1, 2)), listExpected);
    TEST(listFilter(listFoos, &Foo::isEven), list<Foo>({fooB, fooD}));
    TEST(listFilter(listFooPtrs, &Foo::isEven), list<Foo *>({&fooB, &fooD}));
    TEST(listFilter(derivedListFoos, &Foo::isEven), list<Foo>({fooB, fooD}));
    
    TEST(listFilter(listFoos, &Foo::baseIsEven), list<Foo>({fooB, fooD}));
    TEST(listFilter(listFooPtrs, &Foo::baseIsEven), list<Foo *>({&fooB, &fooD}));
    TEST(QListFilter(QListFoos, &Foo::baseIsEven), QList<Foo>({fooB, fooD}));
    TEST(QListFilter(QListFooPtrs, &Foo::baseIsEven), QList<Foo *>({&fooB, &fooD}));
    TEST(QListFilter(QListFoos, &Foo::baseIsEven), QList<Foo>({fooB, fooD}));
    TEST(QListFilter(derivedQListFoos, &Foo::baseIsEven), QList<Foo>({fooB, fooD}));
    
    TEST(filter(vectorNumbers, [] (int x) { return (x%2) == 0; }), vectorExpected);
    TEST(filter(setNumbers, [] (int x) { return (x%2) == 0; }), setExpected);
    TEST(filter(QListNumbers, [] (int x) { return (x%2) == 0; }), QListExpected);
    TEST(filter(QVectorNumbers, [] (int x) { return (x%2) == 0; }), QVectorExpected);
    TEST(filter(QSetNumbers, [] (int x) { return (x%2) == 0; }), QSetExpected);
    TEST(filter(QLinkedListNumbers, [] (int x) { return (x%2) == 0; }), QLinkedListExpected);
    
    TEST(vectorFilter(listNumbers, [] (int x) { return (x%2) == 0; }), vectorExpected);
    TEST(setFilter(listNumbers, [] (int x) { return (x%2) == 0; }), setExpected);
    TEST(QListFilter(listNumbers, [] (int x) { return (x%2) == 0; }), QListExpected);
    TEST(QVectorFilter(listNumbers, [] (int x) { return (x%2) == 0; }), QVectorExpected);
    TEST(QSetFilter(listNumbers, [] (int x) { return (x%2) == 0; }), QSetExpected);
    TEST(QLinkedListFilter(listNumbers, [] (int x) { return (x%2) == 0; }), QLinkedListExpected);
    
    TEST(filter(std::string("aBcDeFgH"), isupper), std::string("BDFH"));
}

void testAllOf()
{
    TEST(allOf(listNumbers, [] (int x) { return (x%2) == 0; }), false);
    TEST(allOf(listNumbers, &isEven), false);
    TEST(allOf(listNumbers, bind(isMultiple, placeholders::_1, 2)), false);
    TEST(allOf(listFoos, &Foo::isEven), false);
    TEST(allOf(listFooPtrs, &Foo::isEven), false);
    TEST(allOf(QListFoos, &Foo::isEven), false);
    TEST(allOf(QListFooPtrs, &Foo::isEven), false);
    TEST(allOf(listFoos, &Foo::baseIsEven), false);
    TEST(allOf(listFooPtrs, &Foo::baseIsEven), false);
    TEST(allOf(QListFoos, &Foo::baseIsEven), false);
    TEST(allOf(QListFooPtrs, &Foo::baseIsEven), false);
    TEST(allOf(derivedListFoos, &Foo::isEven), false);
    TEST(allOf(derivedQListFoos, &Foo::isEven), false);
    
    TEST(allOf(listEvenNumbers, [] (int x) { return (x%2) == 0; }), true);
    
    TEST(allOf(vectorNumbers, [] (int x) { return (x%2) == 0; }), false);
    TEST(allOf(setNumbers, [] (int x) { return (x%2) == 0; }), false);
    TEST(allOf(QListNumbers, [] (int x) { return (x%2) == 0; }), false);
    TEST(allOf(QVectorNumbers, [] (int x) { return (x%2) == 0; }), false);
    TEST(allOf(QSetNumbers, [] (int x) { return (x%2) == 0; }), false);
    TEST(allOf(QLinkedListNumbers, [] (int x) { return (x%2) == 0; }), false);
}

void testAnyOf()
{
    TEST(anyOf(listNumbers, [] (int x) { return (x%2) == 0; }), true);
    TEST(anyOf(listNumbers, &isEven), true);
    TEST(anyOf(listNumbers, bind(isMultiple, placeholders::_1, 2)), true);
    TEST(anyOf(listFoos, &Foo::isEven), true);
    TEST(anyOf(listFooPtrs, &Foo::isEven), true);
    TEST(anyOf(QListFoos, &Foo::isEven), true);
    TEST(anyOf(QListFooPtrs, &Foo::isEven), true);
    TEST(anyOf(listFoos, &Foo::baseIsEven), true);
    TEST(anyOf(listFooPtrs, &Foo::baseIsEven), true);
    TEST(anyOf(QListFoos, &Foo::baseIsEven), true);
    TEST(anyOf(QListFooPtrs, &Foo::baseIsEven), true);
    TEST(anyOf(derivedListFoos, &Foo::isEven), true);
    TEST(anyOf(derivedQListFoos, &Foo::isEven), true);
    
    TEST(anyOf(listOddNumbers, [] (int x) { return (x%2) == 0; }), false);
    
    TEST(anyOf(vectorNumbers, [] (int x) { return (x%2) == 0; }), true);
    TEST(anyOf(setNumbers, [] (int x) { return (x%2) == 0; }), true);
    TEST(anyOf(QListNumbers, [] (int x) { return (x%2) == 0; }), true);
    TEST(anyOf(QVectorNumbers, [] (int x) { return (x%2) == 0; }), true);
    TEST(anyOf(QSetNumbers, [] (int x) { return (x%2) == 0; }), true);
    TEST(anyOf(QLinkedListNumbers, [] (int x) { return (x%2) == 0; }), true);
}

void testExtremum()
{
    TEST(extremum(listNumbers, [] (int x, int y) { return x < y; }), 1);
    TEST(extremum(listNumbers, &isLessThan), 1);
    TEST(extremum(listFoos, &Foo::isLessThan), fooA);
    TEST(extremum(listFooPtrs, &Foo::isLessThanPtr), &fooA);
    TEST(extremum(QListFoos, &Foo::isLessThan), fooA);
    TEST(extremum(QListFooPtrs, &Foo::isLessThanPtr), &fooA);
    TEST(extremum(listFoos, &Foo::baseIsLessThan), fooA);
    TEST(extremum(listFooPtrs, &Foo::baseIsLessThanPtr), &fooA);
    TEST(extremum(QListFoos, &Foo::baseIsLessThan), fooA);
    TEST(extremum(QListFooPtrs, &Foo::baseIsLessThanPtr), &fooA);
    TEST(extremum(derivedListFoos, &Foo::isLessThan), fooA);
    TEST(extremum(derivedQListFoos, &Foo::isLessThan), fooA);
    TEST(extremum(derivedListFooPtrs, &Foo::isLessThanPtr), &fooA);
    TEST(extremum(derivedQListFooPtrs, &Foo::isLessThanPtr), &fooA);
    
    TEST(extremum(vectorNumbers, [] (int x, int y) { return x < y; }), 1);
    TEST(extremum(setNumbers, [] (int x, int y) { return x < y; }), 1);
    TEST(extremum(QListNumbers, [] (int x, int y) { return x < y; }), 1);
    TEST(extremum(QVectorNumbers, [] (int x, int y) { return x < y; }), 1);
    TEST(extremum(QSetNumbers, [] (int x, int y) { return x < y; }), 1);
    TEST(extremum(QLinkedListNumbers, [] (int x, int y) { return x < y; }), 1);
}

void testMin()
{
    TEST(min(listNumbers), 1);
    TEST(min(listFoos), fooA);
    TEST(min(vectorNumbers), 1);
    TEST(min(setNumbers), 1);
    TEST(min(QListNumbers), 1);
    TEST(min(QVectorNumbers), 1);
    TEST(min(QSetNumbers), 1);
    TEST(min(QLinkedListNumbers), 1);
    
    TEST(min(listFoos, [] (const Foo &a) { return a.value; }), fooA);
    TEST(min(listFoos, &Foo::getValue), fooA);
    TEST(min(listFooPtrs, &Foo::getValue), &fooA);
    TEST(min(QListFoos, &Foo::getValue), fooA);
    TEST(min(QListFooPtrs, &Foo::getValue), &fooA);
    TEST(min(listFoos, &Foo::baseGetValue), fooA);
    TEST(min(listFooPtrs, &Foo::baseGetValue), &fooA);
    TEST(min(QListFoos, &Foo::baseGetValue), fooA);
    TEST(min(QListFooPtrs, &Foo::baseGetValue), &fooA);
    TEST(min(derivedListFoos, &Foo::getValue), fooA);
    TEST(min(derivedQListFoos, &Foo::baseGetValue), fooA);
}

void testMax()
{
    TEST(max(listNumbers), 5);
    TEST(max(listFoos), fooE);
    TEST(max(vectorNumbers), 5);
    TEST(max(setNumbers), 5);
    TEST(max(QListNumbers), 5);
    TEST(max(QVectorNumbers), 5);
    TEST(max(QSetNumbers), 5);
    TEST(max(QLinkedListNumbers), 5);
    
    TEST(max(listFoos, [] (const Foo &a) { return a.value; }), fooE);
    TEST(max(listFoos, &Foo::getValue), fooE);
    TEST(max(listFooPtrs, &Foo::getValue), &fooE);
    TEST(max(QListFoos, &Foo::getValue), fooE);
    TEST(max(QListFooPtrs, &Foo::getValue), &fooE);
    TEST(max(listFoos, &Foo::baseGetValue), fooE);
    TEST(max(listFooPtrs, &Foo::baseGetValue), &fooE);
    TEST(max(QListFoos, &Foo::baseGetValue), fooE);
    TEST(max(QListFooPtrs, &Foo::baseGetValue), &fooE);
    TEST(max(derivedListFoos, &Foo::getValue), fooE);
    TEST(max(derivedQListFoos, &Foo::baseGetValue), fooE);
}

void testReduce()
{
    TEST(reduce(listNumbers, [] (int x, int y) { return x*y; }, 1), 120);
    TEST(reduce(listNumbers, &timesX, 1), 120);
    TEST(reduce(listFoos, &Foo::fooTimesFoo, Foo(1)), Foo(120));
    TEST(reduce(QListFoos, &Foo::fooTimesFoo, Foo(1)), Foo(120));
    TEST(reduce(derivedListFoos, &Foo::fooTimesFoo, Foo(1)), Foo(120));
    TEST(reduce(derivedQListFoos, &Foo::fooTimesFoo, Foo(1)), Foo(120));
    
    TEST(reduce(vectorNumbers, [] (int x, int y) { return x*y; }, 1), 120);
    TEST(reduce(setNumbers, [] (int x, int y) { return x*y; }, 1), 120);
    TEST(reduce(QListNumbers, [] (int x, int y) { return x*y; }, 1), 120);
    TEST(reduce(QVectorNumbers, [] (int x, int y) { return x*y; }, 1), 120);
    TEST(reduce(QSetNumbers, [] (int x, int y) { return x*y; }, 1), 120);
    TEST(reduce(QLinkedListNumbers, [] (int x, int y) { return x*y; }, 1), 120);
    
    TEST(reduce(listNumbers, [] (int x, int y) { return x*y; }), 120);
    TEST(reduce(listNumbers, &timesX), 120);
    TEST(reduce(listFoos, &Foo::fooTimesFoo), Foo(120));
    TEST(reduce(list<int>(), [] (int x, int y) { return x*y; }), 0);
    TEST(reduce(list<Foo>(), &Foo::fooTimesFoo), Foo());
    
    TEST(reduce(listNumbers, [] (const QString &a, int b) { return a + QString::number(b); }, QString("X")), "X12345");
}

void testSum()
{
    TEST(sum(listNumbers, 0), 15);
    TEST(sum(listNumbers, 100), 115);
    TEST(sum(listNumbers), 15);
    TEST(sum(vectorNumbers), 15);
    TEST(sum(setNumbers), 15);
    TEST(sum(QListNumbers), 15);
    TEST(sum(QVectorNumbers), 15);
    TEST(sum(QSetNumbers), 15);
    TEST(sum(QLinkedListNumbers), 15);
    TEST(sum(listFoos), Foo(15));
    TEST(sum(listFoos, Foo(100)), Foo(115));
    TEST(sum(QListFoos), Foo(15));
    TEST(sum(derivedListFoos), Foo(15));
    TEST(sum(derivedQListFoos), Foo(15));
    
    TEST(sum(list<long long>({1,2,3,4,5})), 15);
    TEST(sum(list<float>({1.,2.,3.,4.,5.})), 15.0);
    TEST(sum(list<unsigned char>({'\1','\2','\3','\4','\5'})), (unsigned char)15);
}

void testSorted()
{
    TEST(sorted(list<int>({3,5,1,4,2})), listNumbers);
    TEST(sorted(list<int>({3,5,1,4,2}), [] (int a, int b) { return a > b; }), list<int>({5,4,3,2,1}));
    TEST(sorted(vector<int>({3,5,1,4,2})), vectorNumbers);
    TEST(sorted(QList<int>({3,5,1,4,2})), QListNumbers);
    TEST(sorted(QVector<int>({3,5,1,4,2})), QVectorNumbers);
    
    TEST(sorted(list<Foo>({fooC, fooB, fooD, fooE, fooA}), [] (const Foo &a, const Foo &b) { return a < b; }), listFoos);
    TEST(sorted(list<Foo>({fooC, fooB, fooD, fooE, fooA}), &Foo::isLessThan), listFoos);
    TEST(sorted(list<Foo>({fooC, fooB, fooD, fooE, fooA}), &Foo::baseIsLessThan), listFoos);
    TEST(sorted(list<Foo *>({&fooC, &fooB, &fooD, &fooE, &fooA}), &Foo::isLessThanPtr), listFooPtrs);
    TEST(sorted(DerivedList({fooC, fooB, fooD, fooE, fooA}), &Foo::isLessThan), DerivedList(listFoos));
    
    TEST(sorted(QList<Foo>({fooC, fooB, fooD, fooE, fooA}), [] (const Foo &a, const Foo &b) { return a < b; }), QListFoos);
    TEST(sorted(QList<Foo>({fooC, fooB, fooD, fooE, fooA}), &Foo::isLessThan), QListFoos);
    TEST(sorted(QList<Foo>({fooC, fooB, fooD, fooE, fooA}), &Foo::baseIsLessThan), QListFoos);
    TEST(sorted(QList<Foo *>({&fooC, &fooB, &fooD, &fooE, &fooA}), &Foo::isLessThanPtr), QListFooPtrs);
    TEST(sorted(DerivedQList({fooC, fooB, fooD, fooE, fooA}), &Foo::isLessThan), QListFoos);
    
    TEST(sorted(forward_list<int>({3,5,1,4,2})), forward_list<int>({1,2,3,4,5}));
    TEST(sorted(forward_list<int>({3,5,1,4,2}), [] (int a, int b) { return a > b; }), forward_list<int>({5,4,3,2,1}));
    
    QStringList items = {"CCC", "eee", "Bbb", "aaa", "DDD"};
    TEST(sorted(items), QStringList({"Bbb", "CCC", "DDD", "aaa", "eee"}));
    TEST(sorted(items, Qt::CaseInsensitive), QStringList({"aaa", "Bbb", "CCC", "DDD", "eee"}));
}

void testContains()
{
    TEST(contains(listNumbers, 1), true);
    TEST(contains(listNumbers, 100), false);
    TEST(contains(listFoos, fooA), true);
    TEST(contains(listFoos, Foo(100)), false);
    TEST(contains(vectorNumbers, 1), true);
    TEST(contains(setNumbers, 4), true);
    TEST(contains(QListNumbers, 1), true);
    TEST(contains(QVectorNumbers, 1), true);
    TEST(contains(QSetNumbers, 4), true);
}

void testOmit()
{
    TEST(omit(listNumbers, 1), list<int>({2, 3, 4, 5}));
    TEST(omit(listNumbers, list<int>({1, 2, 3})), list<int>({4, 5}));
    TEST(omit(listNumbers, set<int>({1, 2, 3})), list<int>({4, 5}));
    TEST(omit(listFoos, fooA), list<Foo>({fooB, fooC, fooD, fooE}));
    TEST(omit(listFoos, set<Foo>({fooA, fooB, fooC})), list<Foo>({fooD, fooE}));
    TEST(omit(vectorNumbers, 1), vector<int>({2, 3, 4, 5}));
    TEST(omit(vectorNumbers, list<int>({1, 2, 3})), vector<int>({4, 5}));
    TEST(omit(vectorNumbers, set<int>({1, 2, 3})), vector<int>({4, 5}));
    TEST(omit(setNumbers, 1), set<int>({2, 3, 4, 5}));
    TEST(omit(setNumbers, list<int>({1, 2, 3})), set<int>({4, 5}));
    TEST(omit(setNumbers, set<int>({1, 2, 3})), set<int>({4, 5}));
    TEST(omit(QListNumbers, 1), QList<int>({2, 3, 4, 5}));
    TEST(omit(QListNumbers, list<int>({1, 2, 3})), QList<int>({4, 5}));
    TEST(omit(QListNumbers, set<int>({1, 2, 3})), QList<int>({4, 5}));
    TEST(omit(QVectorNumbers, 1), QVector<int>({2, 3, 4, 5}));
    TEST(omit(QVectorNumbers, list<int>({1, 2, 3})), QVector<int>({4, 5}));
    TEST(omit(QVectorNumbers, set<int>({1, 2, 3})), QVector<int>({4, 5}));
    TEST(omit(QSetNumbers, 1), QSet<int>({2, 3, 4, 5}));
    TEST(omit(QSetNumbers, list<int>({1, 2, 3})), QSet<int>({4, 5}));
    TEST(omit(QSetNumbers, set<int>({1, 2, 3})), QSet<int>({4, 5}));
    
    TEST(listOmit(vectorNumbers, 1), list<int>({2, 3, 4, 5}));
    TEST(listOmit(vectorNumbers, list<int>({1, 2, 3})), list<int>({4, 5}));
    TEST(vectorOmit(vectorNumbers, 1), vector<int>({2, 3, 4, 5}));
    TEST(vectorOmit(vectorNumbers, list<int>({1, 2, 3})), vector<int>({4, 5}));
    TEST(setOmit(vectorNumbers, 1), set<int>({2, 3, 4, 5}));
    TEST(setOmit(vectorNumbers, list<int>({1, 2, 3})), set<int>({4, 5}));
    TEST(QListOmit(vectorNumbers, 1), QList<int>({2, 3, 4, 5}));
    TEST(QListOmit(vectorNumbers, list<int>({1, 2, 3})), QList<int>({4, 5}));
    TEST(QVectorOmit(vectorNumbers, 1), QVector<int>({2, 3, 4, 5}));
    TEST(QVectorOmit(vectorNumbers, list<int>({1, 2, 3})), QVector<int>({4, 5}));
    TEST(QSetOmit(vectorNumbers, 1), QSet<int>({2, 3, 4, 5}));
    TEST(QSetOmit(vectorNumbers, list<int>({1, 2, 3})), QSet<int>({4, 5}));
    TEST(QLinkedListOmit(vectorNumbers, 1), QLinkedList<int>({2, 3, 4, 5}));
    TEST(QLinkedListOmit(vectorNumbers, list<int>({1, 2, 3})), QLinkedList<int>({4, 5}));
}

void testReversed()
{
    TEST(reversed(listNumbers), list<int>({5, 4, 3, 2, 1}));
    TEST(reversed(vectorNumbers), vector<int>({5, 4, 3, 2, 1}));
    TEST(reversed(listFoos), list<Foo>({fooE, fooD, fooC, fooB, fooA}));
    TEST(reversed(QListNumbers), QList<int>({5, 4, 3, 2, 1}));
    TEST(reversed(QVectorNumbers), QVector<int>({5, 4, 3, 2, 1}));
}

int main()
{
    testMap();
    testFilter();
    testAllOf();
    testAnyOf();
    testExtremum();
    testMin();
    testMax();
    testReduce();
    testSum();
    testCompr();
    testSorted();
    testContains();
    testOmit();
    testReversed();
    
    qDebug() << "Finished!" << passedTests << "/" << totalTests << "passed";
    return 0;
}
