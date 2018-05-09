// MIT License
// 
// Copyright (c) 2018 Francisco Ruiz (francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef _coffee_basis_Semaphore_h
#define _coffee_basis_Semaphore_h

#include <mutex>
#include <condition_variable>

namespace coffee {
namespace basis {

/**
 * A useful way to think of a semaphore as used in the real-world systems is as a record of how many units of
 * a particular resource are available, coupled with operations to adjust that record safely (i.e. to avoid race conditions)
 * as units are required or become free, and, if necessary, wait until a unit of the resource becomes available.
 *
 * \see https://en.wikipedia.org/wiki/Semaphore_(programming)
 */
class Semaphore {
public:
   /**
    * Constructor
    * \param _counter initial number of resources.
    */
   explicit Semaphore(const int _counter) : counter(_counter) {;}

   /**
    * If the value of semaphore variable is not negative, decrement it by 1. If the semaphore variable is now negative,
    * the process executing wait is blocked (i.e., added to the semaphore's queue) until the value is greater or equal to 1.
    * Otherwise, the process continues execution, having used a unit of the resource.
    */
   void wait() noexcept;

   /**
    * Increments the value of semaphore variable by 1. After the increment, if the pre-increment value was negative
    * (meaning there are processes waiting for a resource), it transfers a blocked process from the semaphore's
    * waiting queue to the ready queue.
    */
   void signal() noexcept;

private:
   std::mutex mutex;
   std::condition_variable condition;
   int counter;
};

} /* namespace basis */
} /* namespace coffee */

#endif /* INCLUDE_COFFEE_ADT_SEMAPHORE_HPP_ */
