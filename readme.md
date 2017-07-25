# A sample C++/Qt calculator application

Works by converting expression to postfix notation and trivially evaluating it.

![Screenshot](http://i.imgur.com/JX0OTqc.png)

Uses [Qt](https://www.qt.io) for GUI. Written in Qt Creator Community Edition, so the code is under GPLv3.

The calculator supports following operations:
* Addition and substraction
* Division and multiplication
* Some trigonometric operations, namely `sin`, `cos` and `tan` and their inverses `asin`, `acos`, `atan`
* * Conversion to radians is done by `rads` and conversion to degrees is done by `degs`
* Square root as `sqrt`
* Exponentiation with base *e* as `exp`

Defined constants:
* `pi` = 3.141592653589793238463
* `e` = 2.718281828459045
* `sixtyfive` = 65