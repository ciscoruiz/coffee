# COFFEE

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/765adc76c848465ba47a244973ef654a)](https://app.codacy.com/app/cisco.tierra/coffee?utm_source=github.com&utm_medium=referral&utm_content=ciscoruiz/coffee&utm_campaign=badger)[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://GitHub.com/Naereen/StrapDown.js/graphs/commit-activity)
[![Ask Me Anything !](https://img.shields.io/badge/Ask%20me-anything-1abc9c.svg)](https://GitHub.com/Naereen/ama)
[![Build Status](https://travis-ci.org/ciscoruiz/coffee.svg?branch=master)](https://travis-ci.org/ciscoruiz/coffee)
[![Build Status](https://semaphoreci.com/api/v1/ciscoruiz/coffee/branches/develop/badge.svg)](https://semaphoreci.com/ciscoruiz/coffee)
[![codecov](https://codecov.io/gh/ciscoruiz/coffee/branch/master/graph/badge.svg)](https://codecov.io/gh/ciscoruiz/coffee)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

COFFEE is the acronym for "COmpany eFFEEctive platform"

## Getting started 

This project is a set of common libraries which help to improve productivity while developing from medium to huge C++ projects.

This framework contains a set of modules to boost productivity while developing C++ applications. The modules are:

| Module | Comment|
|--------|--------|
| coffee.config | Low level macros defines to support different architectures. |
| coffee.adt | Defines **A**bstract **D**ata **T**ypes commonly used, some patterns (LRU Cache, Backtracking general algorihtm, observer) and more. |
| coffee.xml | Defines and high level interface to work with XML documents. |
| coffee.logger | Best logging system you will see. If defines a fast common interfaces for lots of ways of write logs and formatters. |
| coffee.balance | Module for share load between a set of resources, it implements three different strategies and it does easy to write your own. A database connection will be a resource inherited from this kind of resource. |
| coffee.app | Define classes to make easy to run an application with lots of modules. It deal with internal dependencias between engines. And engines is a large functionality which depends on other engines. An example of engine could be one database or one communication system. |
| coffee.dbms | **D**ata**B**ase **M**anager **S**ystem. It defines a common interface for work with any database you need (MySQL, PostgreSQL and more). |
| coffee.dbms.sqlite | Module to work with SQLite3 database. |
| coffee.persistence | High Level of classes to work over a database without has to deal with SQL sentences, transactions and so on, it will work with classes, primrary keys and object that will be read/write from the DBMS. |

### Prerequisites

You should have to use a g++ with support for c++11.

And should have installed the next packages:

```bash
sudo apt-get install cmake
sudo apt-get install libxml2-dev
sudo apt-get install libboost-filesystem-dev
sudo apt-get install libboost-test-dev
sudo apt-get install libsqlite3-dev:amd64
sudo apt-get install sqlite sqlite3   
   
```

If you want to generate API doc in HTML you should install:

'''
sudo apt install doxygen
sudo apt install graphviz

'''

For now it should be very simple.

### Installation

Go to the root source directory and run

```
cmake .
``` 
it will create the needed infraestructure to generate all modules by calling **make**.

You can run the next command to compile libraries and program with Unit Tests

```
make 
``` 

And the next command will run the unit tests:

```
make test
``` 

If you have installed doxygen then you will be able to run the next command to generate API docs

```
make doc
``` 

You can see more commands in the Travis CI configuration file .travis.yml

## License

COFFEE - COmpany eFFEEctive Platform

Copyright (c) 2018 Francisco Ruiz (francisco.ruiz.rayo@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


