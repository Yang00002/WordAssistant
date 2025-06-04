#pragma once

#define DEBUG_ 0  // NOLINT(modernize-macro-to-enum)

#define ifn(condition) if(!(condition))
#define bc_set(name,func) name=func<decltype(name)>  // NOLINT(bugprone-macro-parentheses)
#define bc_setp(name,func) name=func<std::remove_pointer<decltype(name)>::type>  // NOLINT(bugprone-macro-parentheses)
#define bc_new(name) name = new std::remove_pointer<decltype(name)>::type

#define bc_disable if(0)

#define bc_enable if(1)

#define bc_defaultorder(TYPE,ELE) bool operator>(const TYPE& t){return (*this) (ELE) > t ELE;} \
bool operator>=(const TYPE& t){return (*this) (ELE) >= t ELE;} \
bool operator<(const TYPE& t){return (*this) (ELE) < t ELE;} \
bool operator<=(const TYPE& t){return (*this) (ELE) <= t ELE;} \
bool operator==(const TYPE& t){return (*this) (ELE) == t ELE;} \
bool operator!=(const TYPE& t){return (*this) (ELE) != t ELE;}
int log2ceil(int input);
