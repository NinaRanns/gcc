// check that contract assertion on a non empty statement doesn't cause an ICE
// { dg-do compile }
// { dg-options "-std=c++2a -fcontracts " }

void f();
int main ()
{
  int i = 3;
  [[assert: true]] f(); // { dg-error "assertions must be followed by" }
  [[assert: true]] [[assert: i >5]] [[assert: true]] ;
}
