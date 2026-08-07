/* { dg-do compile } */
/* { dg-options "-fprofiles -std=c++11 --param ggc-min-expand=0 --param ggc-min-heapsize=0" } */

/* The profiles framework is in the Objective-C++ front end: the record of
   profile-enforcements and its repetition rule work, and the record is a GC
   root (the collection forced after the template's body marks it).  */

[[profiles::enforce(acme::type)]];
[[profiles::enforce(dup::diff(fortify: 3))]];	/* { dg-message "previously loaded here" } */
[[profiles::enforce(dup::diff(fortify: 4))]];	/* { dg-error "'dup::diff' already loaded with different arguments" } */

__attribute__((objc_root_class))
@interface A
{
  int i;
}
- (int) m;
@end

@implementation A
- (int) m { return i; }
@end

template <class T> T f (T t) { return t; }
