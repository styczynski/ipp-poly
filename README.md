# ipp-bin-poly

See [Doxygen documentation](http://styczynski.ml/ipp-poly)


# Exercise

This project is based on exercise we had to do on **Warsaw University, Poland**
on the **Individual Programistic Project** subject.

### Task

The exercise was to design and implement:
* Provide dynamically-allocated lists implementation - *pre-exercise*

* Polynomial handling library in pure C99 - *part no. 1*

* Interactive calculator for that library - *part no. 2*

* Implement additional functionality and provide basic unit tests - *part no. 3*

### Project structure

The project provides:
* library `src/poly.h` for handling polynomials

* additional helper libraries like dynamically allocated arrays etc.

* interactive calculator `src/calc_poly.c`

* unit tests in `tests/`

# Building

To build do the following:

* Create release folder - `mkdir release`

* Go to that folder - `cd release`

* Generate makefiles - `cmake ..`

* Build everything - `make`

* Optionally run tests - `make test`

* Optionally generate documentation (needs Doxygen to be installed) - `make doc`

* Optionally clean everything - `make clean`

# Polynomial library

All the documentation is provided in header files.

There you've got example usage of the library:

```c
  #include "poly.h"

  int main(void) {

    Poly p = PolyC(4);
    // Create constant polynomial
    // p(x) = 4

    Poly q = PolyP( PolyC(1), 2 );
    // Create polynomial that consists of
    // variable square multiplied by
    // coefficient 1
    // q(x) = 1*x^2

    // As you may see the coefficients that are placed
    // next to variables may be other polynomials
    // (polynomials from other variable)

    Poly r = PolyP( PolyP( PolyC(1), 2 ), 3 );
    // In this example
    // r(x,y) = (1*y^2)*(x^3)

    // Now you can do arithemtic operations on them
    // for example addition:
    Poly sum = PolyAdd(&p, &q);
    // sum(x) = 4 + x^2
    // If you are not sure what would happen if we sum
    // q and r then we say that the result is
    //
    // q(x) + r(x,y) = 1*x^2 + (1*y^2)*(x^3)
    // that is:
    //   x^2 + (y^2)(x^3)
    //

    // Or evalue at specific point:
    Poly rAtTwo = PolyAt(&r, 2);
    // Now we assume the first variable is equal to 2
    // So we calculate r(x,y) for x->2
    // rAtTwo = r(2,y) = (2^3)*y^2 = 8*y^2

    // You can print them nicely:
    PolyPrint(&rAtTwo);
    // ^ Do it yourself and see what the output is

    // After everything we must free them all
    PolyDestroy(&p);
    PolyDestroy(&q);
    PolyDestroy(&r);
    PolyDestroy(&sum);
    PolyDestroy(&rAtTwo);

  }
```

# Polynomial calculator

The calulator resides in `src/calc_poly.c`.

  It reads data from stdin line by line and:

1. If the line starts with lower/upper case letter then it's assumed

  to be an input command.

2. Otherwise the input is parsed as a polynomial

  The format of the polynomial is

  `(COEFF_1,EXP_1)+(COEFF_2,EXP_2)+...+(COEFF_N,EXP_N)`

  Where pair `(COEFF_N,EXP_N)` represents single monomial `(COEFF_N)*x^(EXP_N)`

  The coefficients can be numbers:

  `(1,3)+(2,4)` -> x^3 + 2x^4

  Or polynomials (in this case they depend on other variable):

  `((1,3)+(2,4),2)+(1,3)` -> (x^3 + 2x^4)*y^2 + y^3

  can also contain monomials with the same exponent:

  `(1,2)+(1,2)` -> 2x^2

  Or can be just numbers:

  `42` -> constant polynomial 42


All the polynomials are parsed and placed on top of the stack.
Then you can call one or more of the given operations

to modify the stack contents:

1. `ZERO` - pushes zero const polynomial onto stack

2. `IS_COEFF` - check if the stack top is a constant polynomial?

3. `IS_ZERO` - check if the stack top is a polynomial equal to 0?

4. `CLONE` - Copy the polynomial on top of the stack and place it on the top.

5. `ADD` - Adds two polynomials from the top of the stack.

  Then puts the result on the stack top.

6. `MUL` - Multiplies two polynomials from the top of the stack.

  Then puts the result on the stack top.

7. `NEG` - Changes the sign of the polynomial on the top of the stack.

8. `SUB` - Substracts two polynomials from the top of the stack.

  Then puts the result on the stack top.

9. `IS_EQ` - Checks if two top-most polynomials are equal.

10. `DEG` - Checks the degree of the top-most polynomial.

  Degree of the polynomial is a highest exponent of variable

  encountered in the polynomial (assuming all variables are the

  same one).

  E.g. degree(x^2 + yx^2) = 3

11. `DEG_BY [index]` - Checks the degree of the top-most polynomial

  with respect to the given variable. Variables are indexed from 0

  (0 means the main variable of polynomial).

  E.g.

  DegBy( ((1,2),3), 0 ) = DegBy( x^2*y^3, 0 ) = DegByY( x^2*y^3 )  = 3

  DegBy( ((1,2),3), 1 ) = DegBy( x^2*y^3, 1 ) = DegByX( x^2*y^3 )  = 2


12. `AT [value]` Evaluate the top-most polynomial in specified point

  and put it into stack.

  The polynomial is evalued for `MAIN_VARIABLE=[value]` where

  `MAIN_VARIABLE` is variable with index 0.

  E.g.

  At( ((1,2),3), 0 ) = At( x^2*y^3, 0 ) = AtY( x^2*y^3, 0 ) = 0

  At( (1,2), 2 ) = At( x^2, 2 ) = AtX( x^2, 2 ) = 4


  The result of this command is always a polynomial of degree one smaller

  than the polynomial given.

13. `PRINT` - Print the top-most polynomial.

14. `POP` - Pop the top-most polynomial from the stack.

15. `POW [exp]` - Calculate the top-most polynomial power and push into the

  stack. Obviously `[exp]` can be only a number!

16. `COMPOSE [count]` - take top-most polynomail from the stack.

  (We will call it P)

  Then take `[count]` polynomials from the stack (let's call them Q1, Q2 ...).

  Then we know that `P = C_1*x_1^E_1 + C_2*x_2^E_2 + ...`

  so we substitute

  `x_1 -> Q1`

  `x_2 -> Q2`

  etc.

  if the `x_n` has got no matching `QN` then we assume `x_n -> 0`
  Then we put result of such substitution onto the stack.

17. `DUMP` - Print the stack contents.

18. `CLEAN` - Clear the stack entinerely.

19. `EXIT` - Force exit calculator.
