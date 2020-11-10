#include <qtFunctionalHelpers.h>
#include <QDebug>
#include <QItemSelection>

#ifdef Q_OS_MAC
#include <CFTypeFunctionalHelpers.h>
#endif

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

class CallableClass {
public:
    CallableClass(int a=0) : value(a) {};
    int operator()(const CallableClass &thing) const { return -thing.value; };
    bool operator==(const CallableClass &other) const { return value == other.value; };
    bool operator<(const CallableClass &other) const { return value < other.value; };
    int value;
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
const set<Foo> setFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const QList<Foo> QListFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const QList<Foo *> QListFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};
const QVector<Foo> QVectorFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const QVector<Foo *> QVectorFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};
const DerivedList derivedListFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const DerivedQList derivedQListFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const DerivedPtrList derivedListFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};
const DerivedQPtrList derivedQListFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};

const QStringList stringList = {"aaaaa", "bbbb", "ccc", "dd", "e"};

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
    
    TEST(::map<list>(listFoos, [] (const Foo &foo) { return foo.fooTimesTwo(); }), listExpected);
    TEST(::map<list>(listFoos, &Foo::fooTimesTwo), listExpected);
    TEST(::map<list>(listFooPtrs, &Foo::fooTimesTwo), listExpected);
    TEST(::map<QList>(QListFoos, &Foo::fooTimesTwo), QListExpected);
    TEST(::map<QList>(QListFooPtrs, &Foo::fooTimesTwo), QListExpected);
    TEST(::map<list>(listFoos, &Foo::baseFooTimesTwo), listExpected);
    TEST(::map<list>(listFooPtrs, &Foo::baseFooTimesTwo), listExpected);
    TEST(::map<QList>(QListFoos, &Foo::baseFooTimesTwo), QListExpected);
    TEST(::map<QList>(QListFooPtrs, &Foo::baseFooTimesTwo), QListExpected);
    TEST(::map<list>(derivedListFoos, &Foo::baseFooTimesTwo), listExpected);
    TEST(::map<QList>(derivedQListFoos, &Foo::baseFooTimesTwo), QListExpected);
    
    TEST(::map(vectorNumbers, [] (int x) { return x*2; }), vectorExpected);
    TEST(::map(setNumbers, [] (int x) { return x*2; }), setExpected);
    TEST(::map(QListNumbers, [] (int x) { return x*2; }), QListExpected);
    TEST(::map(QVectorNumbers, [] (int x) { return x*2; }), QVectorExpected);
    TEST(::map(QSetNumbers, [] (int x) { return x*2; }), QSetExpected);
    TEST(::map(QLinkedListNumbers, [] (int x) { return x*2; }), QLinkedListExpected);
    TEST(::map<vector>(listNumbers, [] (int x) { return x*2; }), vectorExpected);
    TEST(::map<set>(listNumbers, [] (int x) { return x*2; }), setExpected);
    TEST(::map<QList>(listNumbers, [] (int x) { return x*2; }), QListExpected);
    TEST(::map<QVector>(listNumbers, [] (int x) { return x*2; }), QVectorExpected);
    TEST(::map<QSet>(listNumbers, [] (int x) { return x*2; }), QSetExpected);
    TEST(::map<QLinkedList>(listNumbers, [] (int x) { return x*2; }), QLinkedListExpected);
    
    // toupper takes an integer and returns an integer, so in order to use it with ::map we have
    // to cast it to the right kind of function. Probably not the best way to go but it works.
    TEST(::map(string("abcde"), (char(*)(char))(toupper)), string("ABCDE"));
    TEST(::map(string("abcde"), [] (char c) { return char(toupper(c)); }), string("ABCDE"));
    TEST(::map<string>(string("abcde"), toupper), string("ABCDE"));
    TEST(::map<QString>(string("abcde"), [] (char c) { return QChar(c).toUpper();}), QString("ABCDE"));
    TEST(::map<QString>(QString("abcde"), [] (QChar c) { return c.toUpper();}), QString("ABCDE"));
    TEST(::map(stringList, [] (const QString &a) { return a.toUpper(); }),
         QStringList({"AAAAA", "BBBB", "CCC", "DD", "E"}));
    TEST(::map(stringList, &QString::toUpper), QStringList({"AAAAA", "BBBB", "CCC", "DD", "E"}));
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
    
    TEST(compr<list>(listFoos, &Foo::fooTimesTwo, &Foo::isEven), listExpected);
    TEST(compr<list>(listFoos, &Foo::fooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), listExpected);
    TEST(compr<list>(listFoos, [] (const Foo &a) { return a.fooTimesTwo(); }, &Foo::isEven), listExpected);
    TEST(compr<list>(listFooPtrs, &Foo::fooTimesTwo, &Foo::isEven), listExpected);
    TEST(compr<list>(listFooPtrs, &Foo::fooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), listExpected);
    TEST(compr<list>(listFooPtrs, [] (const Foo *a) { return a->fooTimesTwo(); }, &Foo::isEven), listExpected);
    TEST(compr<QList>(QListFoos, &Foo::fooTimesTwo, &Foo::isEven), QListExpected);
    TEST(compr<QList>(QListFoos, &Foo::fooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), QListExpected);
    TEST(compr<QList>(QListFoos, [] (const Foo &a) { return a.fooTimesTwo(); }, &Foo::isEven), QListExpected);
    TEST(compr<QList>(QListFooPtrs, &Foo::fooTimesTwo, &Foo::isEven), QListExpected);
    TEST(compr<QList>(QListFooPtrs, &Foo::fooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), QListExpected);
    TEST(compr<QList>(QListFooPtrs, [] (const Foo *a) { return a->fooTimesTwo(); }, &Foo::isEven), QListExpected);
    TEST(compr<list>(derivedListFoos, &Foo::fooTimesTwo, &Foo::isEven), listExpected);
    TEST(compr<QList>(derivedQListFoos, &Foo::fooTimesTwo, &Foo::isEven), QListExpected);
    
    TEST(compr<list>(listFoos, &Foo::baseFooTimesTwo, &Foo::baseIsEven), listExpected);
    TEST(compr<list>(listFoos, &Foo::baseFooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), listExpected);
    TEST(compr<list>(listFoos, [] (const Foo &a) { return a.baseFooTimesTwo(); }, &Foo::baseIsEven), listExpected);
    TEST(compr<list>(listFooPtrs, &Foo::baseFooTimesTwo, &Foo::baseIsEven), listExpected);
    TEST(compr<list>(listFooPtrs, &Foo::baseFooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), listExpected);
    TEST(compr<list>(listFooPtrs, [] (const Foo *a) { return a->baseFooTimesTwo(); }, &Foo::baseIsEven), listExpected);
    TEST(compr<QList>(QListFoos, &Foo::baseFooTimesTwo, &Foo::baseIsEven), QListExpected);
    TEST(compr<QList>(QListFoos, &Foo::baseFooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), QListExpected);
    TEST(compr<QList>(QListFoos, [] (const Foo &a) { return a.baseFooTimesTwo(); }, &Foo::baseIsEven), QListExpected);
    TEST(compr<QList>(QListFooPtrs, &Foo::baseFooTimesTwo, &Foo::baseIsEven), QListExpected);
    TEST(compr<QList>(QListFooPtrs, &Foo::baseFooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), QListExpected);
    TEST(compr<QList>(QListFooPtrs, [] (const Foo *a) { return a->baseFooTimesTwo(); }, &Foo::baseIsEven), QListExpected);
    
    TEST(compr(vectorNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), vectorExpected);
    TEST(compr(setNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), setExpected);
    TEST(compr(QListNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QListExpected);
    TEST(compr(QVectorNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QVectorExpected);
    TEST(compr(QSetNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QSetExpected);
    TEST(compr(QLinkedListNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QLinkedListExpected);
    
    TEST(compr<vector>(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), vectorExpected);
    TEST(compr<set>(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), setExpected);
    TEST(compr<QList>(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QListExpected);
    TEST(compr<QVector>(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QVectorExpected);
    TEST(compr<QSet>(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QSetExpected);
    TEST(compr<QLinkedList>(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}),
         QLinkedListExpected);
    
    TEST(compr(std::string("aBcDeFgH"), (char(*)(char))(tolower), isupper), std::string("bdfh"));
    TEST(compr<string>(std::string("aBcDeFgH"), tolower, isupper), std::string("bdfh"));
    TEST(compr<QString>(QString("aBcDeFgH"), [] (QChar c) { return c.toLower(); },
         [] (QChar c) { return c.isUpper(); }), QString("bdfh"));
    TEST(compr(stringList, &QString::toUpper, &QString::isEmpty), QStringList());
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
    
    TEST(filter<list>(listNumbers, [] (int x) { return (x%2) == 0; }), listExpected);
    TEST(filter<list>(listNumbers, &isEven), listExpected);
    TEST(filter<list>(listNumbers, bind(isMultiple, placeholders::_1, 2)), listExpected);
    TEST(filter<list>(listFoos, &Foo::isEven), list<Foo>({fooB, fooD}));
    TEST(filter<list>(listFooPtrs, &Foo::isEven), list<Foo *>({&fooB, &fooD}));
    TEST(filter<list>(derivedListFoos, &Foo::isEven), list<Foo>({fooB, fooD}));
    
    TEST(filter<list>(listFoos, &Foo::baseIsEven), list<Foo>({fooB, fooD}));
    TEST(filter<list>(listFooPtrs, &Foo::baseIsEven), list<Foo *>({&fooB, &fooD}));
    TEST(filter<QList>(QListFoos, &Foo::baseIsEven), QList<Foo>({fooB, fooD}));
    TEST(filter<QList>(QListFooPtrs, &Foo::baseIsEven), QList<Foo *>({&fooB, &fooD}));
    TEST(filter<QList>(QListFoos, &Foo::baseIsEven), QList<Foo>({fooB, fooD}));
    TEST(filter<QList>(derivedQListFoos, &Foo::baseIsEven), QList<Foo>({fooB, fooD}));
    
    TEST(filter(vectorNumbers, [] (int x) { return (x%2) == 0; }), vectorExpected);
    TEST(filter(setNumbers, [] (int x) { return (x%2) == 0; }), setExpected);
    TEST(filter(QListNumbers, [] (int x) { return (x%2) == 0; }), QListExpected);
    TEST(filter(QVectorNumbers, [] (int x) { return (x%2) == 0; }), QVectorExpected);
    TEST(filter(QSetNumbers, [] (int x) { return (x%2) == 0; }), QSetExpected);
    TEST(filter(QLinkedListNumbers, [] (int x) { return (x%2) == 0; }), QLinkedListExpected);
    
    TEST(filter<vector>(listNumbers, [] (int x) { return (x%2) == 0; }), vectorExpected);
    TEST(filter<set>(listNumbers, [] (int x) { return (x%2) == 0; }), setExpected);
    TEST(filter<QList>(listNumbers, [] (int x) { return (x%2) == 0; }), QListExpected);
    TEST(filter<QVector>(listNumbers, [] (int x) { return (x%2) == 0; }), QVectorExpected);
    TEST(filter<QSet>(listNumbers, [] (int x) { return (x%2) == 0; }), QSetExpected);
    TEST(filter<QLinkedList>(listNumbers, [] (int x) { return (x%2) == 0; }), QLinkedListExpected);
    
    TEST(filter(std::string("aBcDeFgH"), isupper), std::string("BDFH"));
    TEST(filter(stringList, &QString::isEmpty), QStringList());
    TEST(filter(stringList, [] (const QString &a) { return !a.isEmpty(); }), stringList);
}

void testReject()
{
    const list<int> listExpected = {1, 3, 5};
    const vector<int> vectorExpected = {1, 3, 5};
    const set<int> setExpected = {1, 3, 5};
    const QList<int> QListExpected = {1, 3, 5};
    const QVector<int> QVectorExpected = {1, 3, 5};
    const QSet<int> QSetExpected = {1, 3, 5};
    const QLinkedList<int> QLinkedListExpected = {1, 3, 5};
    
    TEST(reject(vectorNumbers, [] (int x) { return (x%2) == 0; }), vectorExpected);
    TEST(reject(setNumbers, [] (int x) { return (x%2) == 0; }), setExpected);
    TEST(reject(QListNumbers, [] (int x) { return (x%2) == 0; }), QListExpected);
    TEST(reject(QVectorNumbers, [] (int x) { return (x%2) == 0; }), QVectorExpected);
    TEST(reject(QSetNumbers, [] (int x) { return (x%2) == 0; }), QSetExpected);
    TEST(reject(QLinkedListNumbers, [] (int x) { return (x%2) == 0; }), QLinkedListExpected);
    
    TEST(reject<list>(listNumbers, [] (int x) { return (x%2) == 0; }), listExpected);
    TEST(reject<list>(listNumbers, &isEven), listExpected);
    TEST(reject<list>(listNumbers, bind(isMultiple, placeholders::_1, 2)), listExpected);
    TEST(reject<list>(listFoos, &Foo::isEven), list<Foo>({fooA, fooC, fooE}));
    TEST(reject<list>(listFooPtrs, &Foo::isEven), list<Foo *>({&fooA, &fooC, &fooE}));
    TEST(reject<list>(derivedListFoos, &Foo::isEven), list<Foo>({fooA, fooC, fooE}));
    
    TEST(reject<list>(listFoos, &Foo::baseIsEven), list<Foo>({fooA, fooC, fooE}));
    TEST(reject<list>(listFooPtrs, &Foo::baseIsEven), list<Foo *>({&fooA, &fooC, &fooE}));
    TEST(reject<QList>(QListFoos, &Foo::baseIsEven), QList<Foo>({fooA, fooC, fooE}));
    TEST(reject<QList>(QListFooPtrs, &Foo::baseIsEven), QList<Foo *>({&fooA, &fooC, &fooE}));
    TEST(reject<QList>(QListFoos, &Foo::baseIsEven), QList<Foo>({fooA, fooC, fooE}));
    TEST(reject<QList>(derivedQListFoos, &Foo::baseIsEven), QList<Foo>({fooA, fooC, fooE}));
    
    TEST(reject<vector>(listNumbers, [] (int x) { return (x%2) == 0; }), vectorExpected);
    TEST(reject<set>(listNumbers, [] (int x) { return (x%2) == 0; }), setExpected);
    TEST(reject<QList>(listNumbers, [] (int x) { return (x%2) == 0; }), QListExpected);
    TEST(reject<QVector>(listNumbers, [] (int x) { return (x%2) == 0; }), QVectorExpected);
    TEST(reject<QSet>(listNumbers, [] (int x) { return (x%2) == 0; }), QSetExpected);
    TEST(reject<QLinkedList>(listNumbers, [] (int x) { return (x%2) == 0; }), QLinkedListExpected);
    
    TEST(reject(std::string("aBcDeFgH"), isupper), std::string("aceg"));
    TEST(reject(stringList, &QString::isEmpty), stringList);
    TEST(reject(stringList, [] (const QString &a) { return !a.isEmpty(); }), QStringList());
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
    TEST(allOf(stringList, &QString::isEmpty), false);
    TEST(allOf(stringList, [] (const QString &a) { return !a.isEmpty(); }), true);
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
    TEST(anyOf(stringList, &QString::isEmpty), false);
    TEST(anyOf(stringList, [] (const QString &a) { return !a.isEmpty(); }), true);
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
    
    TEST(extremum(list<int>(), [] (int x, int y) { return x < y; }), 0);
    TEST(extremum(list<int>(), [] (int x, int y) { return x < y; }, 123), 123);
    
    TEST(extremum(stringList, [] (const QString &a, const QString &b) { return a < b; }), QString("aaaaa"));
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
    
    TEST(min(list<int>()), 0);
    TEST(min(list<int>(), 123), 123);
    TEST(min(list<Foo>(), Foo(123)), Foo(123));
    
    // Example of gotcha where the second argument to min is callable with the value type of the passed in container:
    TEST(min(list<CallableClass>(), CallableClass(123)), CallableClass(0));
    TEST(min(list<CallableClass>({CallableClass(1), CallableClass(2)}), CallableClass(123)), CallableClass(2));
    
    TEST(min(stringList), QString("aaaaa"));
    TEST(min(stringList, &QString::length), QString("e"));
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
    
    TEST(max(list<int>()), 0);
    TEST(max(list<int>(), 123), 123);
    TEST(max(list<Foo>(), Foo(123)), Foo(123));
    
    // Example of gotcha where the second argument to max is callable with the value type of the passed in container:
    TEST(max(list<CallableClass>(), CallableClass(123)), CallableClass(0));
    TEST(max(list<CallableClass>({CallableClass(1), CallableClass(2)}), CallableClass(123)), CallableClass(1));
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
    TEST(contains(stringList, "bbbb"), true);
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
    
    TEST(omit<list>(vectorNumbers, 1), list<int>({2, 3, 4, 5}));
    TEST(omit<list>(vectorNumbers, list<int>({1, 2, 3})), list<int>({4, 5}));
    TEST(omit<vector>(vectorNumbers, 1), vector<int>({2, 3, 4, 5}));
    TEST(omit<vector>(vectorNumbers, list<int>({1, 2, 3})), vector<int>({4, 5}));
    TEST(omit<set>(vectorNumbers, 1), set<int>({2, 3, 4, 5}));
    TEST(omit<set>(vectorNumbers, list<int>({1, 2, 3})), set<int>({4, 5}));
    TEST(omit<QList>(vectorNumbers, 1), QList<int>({2, 3, 4, 5}));
    TEST(omit<QList>(vectorNumbers, list<int>({1, 2, 3})), QList<int>({4, 5}));
    TEST(omit<QVector>(vectorNumbers, 1), QVector<int>({2, 3, 4, 5}));
    TEST(omit<QVector>(vectorNumbers, list<int>({1, 2, 3})), QVector<int>({4, 5}));
    TEST(omit<QSet>(vectorNumbers, 1), QSet<int>({2, 3, 4, 5}));
    TEST(omit<QSet>(vectorNumbers, list<int>({1, 2, 3})), QSet<int>({4, 5}));
    TEST(omit<QLinkedList>(vectorNumbers, 1), QLinkedList<int>({2, 3, 4, 5}));
    TEST(omit<QLinkedList>(vectorNumbers, list<int>({1, 2, 3})), QLinkedList<int>({4, 5}));
    TEST(omit(string("abcde"), 'a'), string("bcde"));
    TEST(omit(string("abcde"), string("ace")), string("bd"));
}

void testReversed()
{
    TEST(reversed(listNumbers), list<int>({5, 4, 3, 2, 1}));
    TEST(reversed(vectorNumbers), vector<int>({5, 4, 3, 2, 1}));
    TEST(reversed(listFoos), list<Foo>({fooE, fooD, fooC, fooB, fooA}));
    TEST(reversed(QListNumbers), QList<int>({5, 4, 3, 2, 1}));
    TEST(reversed(QVectorNumbers), QVector<int>({5, 4, 3, 2, 1}));
    TEST(reversed(QLinkedListNumbers), QLinkedList<int>({5, 4, 3, 2, 1}));
}

void testFirst()
{
    TEST(first(listNumbers), 1);
    TEST(first(listNumbers, 0), 1);
    TEST(first(std::list<int>()), 0);
    TEST(first(std::list<int>(), 12345), 12345);
    TEST(first(listFoos), fooA);
    TEST(first(std::list<Foo>(), fooC), fooC);
    TEST(first(std::list<Foo>()), Foo());
    TEST(first(vectorNumbers), 1);
    TEST(first(QListNumbers), 1);
    TEST(first(QVectorNumbers), 1);
    TEST(first(QLinkedListNumbers), 1);
    TEST(first(QStringList({"one", "two", "three"})), QString("one"));
}

void testLast()
{
    TEST(last(listNumbers), 5);
    TEST(last(listNumbers, 0), 5);
    TEST(last(std::list<int>()), 0);
    TEST(last(std::list<int>(), 12345), 12345);
    TEST(last(listFoos), fooE);
    TEST(last(std::list<Foo>(), fooC), fooC);
    TEST(last(std::list<Foo>()), Foo());
    TEST(last(vectorNumbers), 5);
    TEST(last(QListNumbers), 5);
    TEST(last(QVectorNumbers), 5);
    TEST(last(QLinkedListNumbers), 5);
    TEST(last(QStringList({"one", "two", "three"})), QString("three"));
    TEST(last(QItemSelection()), QItemSelectionRange());
    TEST(last(QStringList(), QString("default")), QString("default"));
}

void testRange()
{
    TEST(range<list>(1, 6, 1), listNumbers);
    TEST(range<list>(5, 0, -1), reversed(listNumbers));
    TEST(range<list>(-1, -6, -1), list<int>({-1, -2, -3, -4, -5}));
    TEST(range<list>(1, 6, 2), list<int>({1, 3, 5}));
    TEST(range<list>(1, 6), listNumbers);
    TEST(range<list>(5), list<int>({0, 1, 2, 3, 4}));
    
    TEST(range<vector>(1, 6, 1), vectorNumbers);
    TEST(range<vector>(5, 0, -1), reversed(vectorNumbers));
    TEST(range<vector>(-1, -6, -1), vector<int>({-1, -2, -3, -4, -5}));
    TEST(range<vector>(1, 6, 2), vector<int>({1, 3, 5}));
    TEST(range<vector>(1, 6), vectorNumbers);
    TEST(range<vector>(5), vector<int>({0, 1, 2, 3, 4}));
    
    TEST(range<set>(1, 6, 1), setNumbers);
    TEST(range<set>(5, 0, -1), reversed(setNumbers));
    TEST(range<set>(-1, -6, -1), set<int>({-1, -2, -3, -4, -5}));
    TEST(range<set>(1, 6, 2), set<int>({1, 3, 5}));
    TEST(range<set>(1, 6), setNumbers);
    TEST(range<set>(5), set<int>({0, 1, 2, 3, 4}));
    
    TEST(range<QList>(1, 6, 1), QListNumbers);
    TEST(range<QList>(5, 0, -1), reversed(QListNumbers));
    TEST(range<QList>(-1, -6, -1), QList<int>({-1, -2, -3, -4, -5}));
    TEST(range<QList>(1, 6, 2), QList<int>({1, 3, 5}));
    TEST(range<QList>(1, 6), QListNumbers);
    TEST(range<QList>(5), QList<int>({0, 1, 2, 3, 4}));
    
    TEST(range<QVector>(1, 6, 1), QVectorNumbers);
    TEST(range<QVector>(5, 0, -1), reversed(QVectorNumbers));
    TEST(range<QVector>(-1, -6, -1), QVector<int>({-1, -2, -3, -4, -5}));
    TEST(range<QVector>(1, 6, 2), QVector<int>({1, 3, 5}));
    TEST(range<QVector>(1, 6), QVectorNumbers);
    TEST(range<QVector>(5), QVector<int>({0, 1, 2, 3, 4}));
    
    TEST(range<QSet>(1, 6, 1), QSetNumbers);
    TEST(range<QSet>(5, 0, -1), QSet<int>({5, 4, 3, 2, 1}));
    TEST(range<QSet>(-1, -6, -1), QSet<int>({-1, -2, -3, -4, -5}));
    TEST(range<QSet>(1, 6, 2), QSet<int>({1, 3, 5}));
    TEST(range<QSet>(1, 6), QSetNumbers);
    TEST(range<QSet>(5), QSet<int>({0, 1, 2, 3, 4}));
    
    TEST(range<QLinkedList>(1, 6, 1), QLinkedListNumbers);
    TEST(range<QLinkedList>(5, 0, -1), reversed(QLinkedListNumbers));
    TEST(range<QLinkedList>(-1, -6, -1), QLinkedList<int>({-1, -2, -3, -4, -5}));
    TEST(range<QLinkedList>(1, 6, 2), QLinkedList<int>({1, 3, 5}));
    TEST(range<QLinkedList>(1, 6), QLinkedListNumbers);
    TEST(range<QLinkedList>(5), QLinkedList<int>({0, 1, 2, 3, 4}));
}

void testMapRange()
{
    TEST(mapRange<list>(1, 6, 1, [] (int x) { return x; }), listNumbers);
    TEST(mapRange<list>(1, 6, 1, [] (int x) { return Foo(x); }), listFoos);
    TEST(mapRange<list>(5, 0, -1, [] (int x) { return x; }), reversed(listNumbers));
    TEST(mapRange<list>(5, 0, -1, [] (int x) { return Foo(x); }), reversed(listFoos));
    TEST(mapRange<list>(1, 6, [] (int x) { return x; }), listNumbers);
    TEST(mapRange<list>(1, 6, [] (int x) { return Foo(x); }), listFoos);
    TEST(mapRange<list>(5, [] (int x) { return x+1; }), listNumbers);
    TEST(mapRange<list>(5, [] (int x) { return Foo(x+1); }), listFoos);
    TEST(mapRange<list>(1, 6, 1, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), list<int>({2, 4}));
    TEST(mapRange<list>(1, 6, 1, [] (int x) { return Foo(x); }, [] (int x) { return (x%2) == 0; }), list<Foo>({fooB, fooD}));
    TEST(mapRange<list>(1, 6, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), list<int>({2, 4}));
    TEST(mapRange<list>(1, 6, [] (int x) { return Foo(x); }, [] (int x) { return (x%2) == 0; }), list<Foo>({fooB, fooD}));
    TEST(mapRange<list>(5, [] (int x) { return x+1; }, [] (int x) { return (x%2) == 0; }), list<int>({1, 3, 5}));
    TEST(mapRange<list>(5, [] (int x) { return Foo(x+1); }, [] (int x) { return (x%2) == 0; }), list<Foo>({fooA, fooC, fooE}));
    
    TEST(mapRange<vector>(1, 6, 1, [] (int x) { return x; }), vectorNumbers);
    TEST(mapRange<vector>(1, 6, 1, [] (int x) { return Foo(x); }), vectorFoos);
    TEST(mapRange<vector>(5, 0, -1, [] (int x) { return x; }), reversed(vectorNumbers));
    TEST(mapRange<vector>(5, 0, -1, [] (int x) { return Foo(x); }), reversed(vectorFoos));
    TEST(mapRange<vector>(1, 6, [] (int x) { return x; }), vectorNumbers);
    TEST(mapRange<vector>(1, 6, [] (int x) { return Foo(x); }), vectorFoos);
    TEST(mapRange<vector>(5, [] (int x) { return x+1; }), vectorNumbers);
    TEST(mapRange<vector>(5, [] (int x) { return Foo(x+1); }), vectorFoos);
    TEST(mapRange<vector>(1, 6, 1, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), vector<int>({2, 4}));
    TEST(mapRange<vector>(1, 6, 1, [] (int x) { return Foo(x); }, [] (int x) { return (x%2) == 0; }), vector<Foo>({fooB, fooD}));
    TEST(mapRange<vector>(1, 6, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), vector<int>({2, 4}));
    TEST(mapRange<vector>(1, 6, [] (int x) { return Foo(x); }, [] (int x) { return (x%2) == 0; }), vector<Foo>({fooB, fooD}));
    TEST(mapRange<vector>(5, [] (int x) { return x+1; }, [] (int x) { return (x%2) == 0; }), vector<int>({1, 3, 5}));
    TEST(mapRange<vector>(5, [] (int x) { return Foo(x+1); }, [] (int x) { return (x%2) == 0; }), vector<Foo>({fooA, fooC, fooE}));
    
    TEST(mapRange<set>(1, 6, 1, [] (int x) { return x; }), setNumbers);
    TEST(mapRange<set>(1, 6, 1, [] (int x) { return Foo(x); }), setFoos);
    TEST(mapRange<set>(1, 6, [] (int x) { return x; }), setNumbers);
    TEST(mapRange<set>(1, 6, [] (int x) { return Foo(x); }), setFoos);
    TEST(mapRange<set>(5, [] (int x) { return x+1; }), setNumbers);
    TEST(mapRange<set>(5, [] (int x) { return Foo(x+1); }), setFoos);
    TEST(mapRange<set>(1, 6, 1, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), set<int>({2, 4}));
    TEST(mapRange<set>(1, 6, 1, [] (int x) { return Foo(x); }, [] (int x) { return (x%2) == 0; }), set<Foo>({fooB, fooD}));
    TEST(mapRange<set>(1, 6, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), set<int>({2, 4}));
    TEST(mapRange<set>(1, 6, [] (int x) { return Foo(x); }, [] (int x) { return (x%2) == 0; }), set<Foo>({fooB, fooD}));
    TEST(mapRange<set>(5, [] (int x) { return x+1; }, [] (int x) { return (x%2) == 0; }), set<int>({1, 3, 5}));
    TEST(mapRange<set>(5, [] (int x) { return Foo(x+1); }, [] (int x) { return (x%2) == 0; }), set<Foo>({fooA, fooC, fooE}));
    
    TEST(mapRange<QList>(1, 6, 1, [] (int x) { return x; }), QListNumbers);
    TEST(mapRange<QList>(1, 6, 1, [] (int x) { return Foo(x); }), QListFoos);
    TEST(mapRange<QList>(5, 0, -1, [] (int x) { return x; }), reversed(QListNumbers));
    TEST(mapRange<QList>(5, 0, -1, [] (int x) { return Foo(x); }), reversed(QListFoos));
    TEST(mapRange<QList>(1, 6, [] (int x) { return x; }), QListNumbers);
    TEST(mapRange<QList>(1, 6, [] (int x) { return Foo(x); }), QListFoos);
    TEST(mapRange<QList>(5, [] (int x) { return x+1; }), QListNumbers);
    TEST(mapRange<QList>(5, [] (int x) { return Foo(x+1); }), QListFoos);
    TEST(mapRange<QList>(1, 6, 1, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), QList<int>({2, 4}));
    TEST(mapRange<QList>(1, 6, 1, [] (int x) { return Foo(x); }, [] (int x) { return (x%2) == 0; }), QList<Foo>({fooB, fooD}));
    TEST(mapRange<QList>(1, 6, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), QList<int>({2, 4}));
    TEST(mapRange<QList>(1, 6, [] (int x) { return Foo(x); }, [] (int x) { return (x%2) == 0; }), QList<Foo>({fooB, fooD}));
    TEST(mapRange<QList>(5, [] (int x) { return x+1; }, [] (int x) { return (x%2) == 0; }), QList<int>({1, 3, 5}));
    TEST(mapRange<QList>(5, [] (int x) { return Foo(x+1); }, [] (int x) { return (x%2) == 0; }), QList<Foo>({fooA, fooC, fooE}));
    
    TEST(mapRange<QVector>(1, 6, 1, [] (int x) { return x; }), QVectorNumbers);
    TEST(mapRange<QVector>(1, 6, 1, [] (int x) { return Foo(x); }), QVectorFoos);
    TEST(mapRange<QVector>(5, 0, -1, [] (int x) { return x; }), reversed(QVectorNumbers));
    TEST(mapRange<QVector>(5, 0, -1, [] (int x) { return Foo(x); }), reversed(QVectorFoos));
    TEST(mapRange<QVector>(1, 6, [] (int x) { return x; }), QVectorNumbers);
    TEST(mapRange<QVector>(1, 6, [] (int x) { return Foo(x); }), QVectorFoos);
    TEST(mapRange<QVector>(5, [] (int x) { return x+1; }), QVectorNumbers);
    TEST(mapRange<QVector>(5, [] (int x) { return Foo(x+1); }), QVectorFoos);
    TEST(mapRange<QVector>(1, 6, 1, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), QVector<int>({2, 4}));
    TEST(mapRange<QVector>(1, 6, 1, [] (int x) { return Foo(x); }, [] (int x) { return (x%2) == 0; }), QVector<Foo>({fooB, fooD}));
    TEST(mapRange<QVector>(1, 6, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), QVector<int>({2, 4}));
    TEST(mapRange<QVector>(1, 6, [] (int x) { return Foo(x); }, [] (int x) { return (x%2) == 0; }), QVector<Foo>({fooB, fooD}));
    TEST(mapRange<QVector>(5, [] (int x) { return x+1; }, [] (int x) { return (x%2) == 0; }), QVector<int>({1, 3, 5}));
    TEST(mapRange<QVector>(5, [] (int x) { return Foo(x+1); }, [] (int x) { return (x%2) == 0; }), QVector<Foo>({fooA, fooC, fooE}));
    
    TEST(mapRange<QSet>(1, 6, 1, [] (int x) { return x; }), QSetNumbers);
    TEST(mapRange<QSet>(1, 6, [] (int x) { return x; }), QSetNumbers);
    TEST(mapRange<QSet>(5, [] (int x) { return x+1; }), QSetNumbers);
    TEST(mapRange<QSet>(1, 6, 1, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), QSet<int>({2, 4}));
    TEST(mapRange<QSet>(1, 6, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), QSet<int>({2, 4}));
    TEST(mapRange<QSet>(5, [] (int x) { return x+1; }, [] (int x) { return (x%2) == 0; }), QSet<int>({1, 3, 5}));
    
    TEST(mapRange<QLinkedList>(1, 6, 1, [] (int x) { return x; }), QLinkedListNumbers);
    TEST(mapRange<QLinkedList>(5, 0, -1, [] (int x) { return x; }), reversed(QLinkedListNumbers));
    TEST(mapRange<QLinkedList>(1, 6, [] (int x) { return x; }), QLinkedListNumbers);
    TEST(mapRange<QLinkedList>(5, [] (int x) { return x+1; }), QLinkedListNumbers);
    TEST(mapRange<QLinkedList>(1, 6, 1, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), QLinkedList<int>({2, 4}));
    TEST(mapRange<QLinkedList>(1, 6, [] (int x) { return x; }, [] (int x) { return (x%2) == 0; }), QLinkedList<int>({2, 4}));
    TEST(mapRange<QLinkedList>(5, [] (int x) { return x+1; }, [] (int x) { return (x%2) == 0; }), QLinkedList<int>({1, 3, 5}));
}

void testFlatten()
{
    TEST(flatten(std::list<std::list<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), std::list<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten(std::vector<std::vector<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), std::vector<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten(std::set<std::set<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), std::set<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten(QList<QList<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), QList<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten(QVector<QVector<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), QVector<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten(QSet<QSet<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), QSet<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten(QLinkedList<QLinkedList<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), QLinkedList<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten(QList<QVector<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), QVector<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten(QVector<QSet<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), QSet<int>({1,2,3,4,5,6,7,8,9}));
    
    TEST(flatten<list>(std::list<std::list<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), std::list<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten<vector>(std::list<std::list<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), std::vector<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten<set>(std::list<std::list<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), std::set<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten<QList>(QList<QList<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), QList<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten<QVector>(QVector<QVector<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), QVector<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten<QSet>(QSet<QSet<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), QSet<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten<QLinkedList>(QList<QVector<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), QLinkedList<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten<list>(std::list<std::list<Foo> >({{fooA, fooB}, {fooC, fooD}, {fooE}})), std::list<Foo>({fooA, fooB, fooC, fooD, fooE}));
    
    TEST(flatten<list>(std::vector<std::list<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), std::list<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten<list>(std::vector<std::vector<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), std::list<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten<list>(std::list<std::vector<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), std::list<int>({1,2,3,4,5,6,7,8,9}));
    TEST(flatten<set>(std::list<std::set<int> >({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}})), std::set<int>({1,2,3,4,5,6,7,8,9}));
}

#ifdef Q_OS_MAC

void testCFArray()
{
    CFMutableArrayRef array = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    
    for(int i = 1; i <= 5; ++i) {
        CFArrayAppendValue(array, CFNumberCreate(NULL, kCFNumberIntType, &i));
    }
    
    TEST(anyOf(array, [] (const CFTypeRef &val) { return CFGetTypeID(val) == CFNumberGetTypeID(); }), true);
    TEST(allOf(array, [] (const CFTypeRef &val) { return CFGetTypeID(val) == CFNumberGetTypeID(); }), true);
    TEST(allOf(array, [] (const CFTypeRef &val) { return CFGetTypeID(val) == CFStringGetTypeID(); }), false);
    
    CFRelease(array);
}

#endif

int main()
{
    testMap();
    testFilter();
    testReject();
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
    testFirst();
    testLast();
    testRange();
    testMapRange();
    testFlatten();
#ifdef Q_OS_MAC
    testCFArray();
#endif
    
    qDebug() << "Finished!" << passedTests << "/" << totalTests << "passed";
    return 0;
}
