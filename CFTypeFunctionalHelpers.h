#ifndef CFTYPEFUNCTIONALHELPERS_H
#define CFTYPEFUNCTIONALHELPERS_H

#include <iterator>
#include <CoreServices/CoreServices.h>

class CFArray_iterator : public std::iterator<std::random_access_iterator_tag,
                                              CFTypeRef,
                                              int,
                                              CFTypeRef *,
                                              CFTypeRef &>
{
    public:
        CFArray_iterator(CFArrayRef inArray) : array(inArray), index(0) {};
        CFArray_iterator & operator++() {
            index++;
            index = std::min(index, CFArrayGetCount(array));
            return *this;
        };
        CFArray_iterator operator++(int) {
            CFArray_iterator result = *this;
            ++(*this);
            return result;
        };
        CFTypeRef operator*() { return CFArrayGetValueAtIndex(array, index); };
        bool operator==(const CFArray_iterator & rhs) { return array == rhs.array && index == rhs.index; };
        bool operator!=(const CFArray_iterator & rhs) { return !(*this == rhs); };
    private:
        CFArrayRef array;
        CFIndex index;
        friend CFArray_iterator end(CFArrayRef array);
};

inline CFArray_iterator begin(CFArrayRef array)
{
    return CFArray_iterator(array);
}

inline CFArray_iterator end(CFArrayRef array)
{
    CFArray_iterator result(array);
    result.index = CFArrayGetCount(array);
    return result;
}

#endif // CFTYPEFUNCTIONALHELPERS_H
