# pyosmium

Provides Python bindings for the [libosmium](https://github.com/osmcode/libosmium) C++
library, a library for working with OpenStreetMap data in a fast and flexible
manner.

[![Build Status](https://secure.travis-ci.org/osmcode/pyosmium.png)](http://travis-ci.org/osmcode/pyosmium)

## Depends

Python >= 2.7 is supported (that includes python 3.x).

pyosmium uses [Boost.Python](http://www.boost.org/doc/libs/1_56_0/libs/python/doc/index.html)
to create the bindings. On Debian/Ubuntu install `libboost-python-dev`.

## Installation

To compile the bindings, run

    python setup.py build

To compile and install the bindings, run

    python setup.py install

libosmium is expected to reside in the same directory as pyosmium or to be
installed globally.

## Examples

The `example` directory contains small examples on how to use the library.
They are for most parts ports of the examples in Libosmium and osmium-contrib.

## Testing

There is a small test suite in the test directory. This provides regression
test for the python bindings, it is not meant to be a test suite for libosmium.

You'll need the Python `nose` module. On Debian/Ubuntu install the package
`python-nose`.

The suite can be run with:

    cd test
    python run_tests.py

## License

Pyosmium is available under the BSD 2-Clause License. See LICENSE.TXT.

## Authors

Sarah Hoffmann (lonvia@denofr.de)

