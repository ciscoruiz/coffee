# COFFEE

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/765adc76c848465ba47a244973ef654a)](https://app.codacy.com/app/cisco.tierra/coffee?utm_source=github.com&utm_medium=referral&utm_content=ciscoruiz/coffee&utm_campaign=badger)[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://GitHub.com/Naereen/StrapDown.js/graphs/commit-activity)
[![Ask Me Anything !](https://img.shields.io/badge/Ask%20me-anything-1abc9c.svg)](https://GitHub.com/Naereen/ama)
[![Build Status](https://travis-ci.org/ciscoruiz/coffee.svg?branch=master)](https://travis-ci.org/ciscoruiz/coffee)
[![Build Status](https://semaphoreci.com/api/v1/ciscoruiz/coffee/branches/develop/badge.svg)](https://semaphoreci.com/ciscoruiz/coffee)

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
For now it should be very simple.

### Installation

Go to the root source directory and run

```
cmake .
``` 
it will create the needed infraestructure to generate all modules by calling **make**.

You can run the next command to see the list of target you can use with **make**. 

```
make help
``` 

## License

COFFEE - COmpany eFFEEctive Platform

(c) Copyright 2018 Francisco Ruiz Rayo

https://github.com/ciscoruiz/coffee

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
   * Neither the name of Google Inc. nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Author: cisco.tierra@gmail.com

