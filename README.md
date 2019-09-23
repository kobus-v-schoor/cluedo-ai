[![pipeline status](https://repo.ee.up.ac.za/2019_epe321/group_1/badges/ai/pipeline.svg)](https://repo.ee.up.ac.za/2019_epe321/group_1/commits/ai)
[![coverage report](https://repo.ee.up.ac.za/2019_epe321/group_1/badges/ai/coverage.svg)](https://repo.ee.up.ac.za/2019_epe321/group_1/commits/ai)

# AI

## Building and running the unit tests

First, make sure to install the "catch" using the system package manager:`sudo 
apt install catch`. After doing this, compile the test suite using `make test`.
You can then run the test suite using `make run`.

## Using the AI in your component

To use the AI, add the `include` folder into your include search path
(`INCLUDEPATH += <path>` in Qt project) and add the `src` folder to your sources
list (`SOURCES += <path>/*.cpp` in Qt project)

## Generating documentation

First, make sure the `graphviz` package is installed (`sudo apt install
graphviz`). Then, generate the documentation with `make doc`. You can then
access the documentation at `docs/html/index.html`.
