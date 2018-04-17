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
#ifndef TEST_NETWORKING_NETWORKINGFIXTURE_HPP_
#define TEST_NETWORKING_NETWORKINGFIXTURE_HPP_

#include <coffee/app/ApplicationServiceStarter.hpp>

#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/SocketArguments.hpp>
#include <coffee/networking/AsyncSocket.hpp>
#include <coffee/networking/ServerSocket.hpp>
#include <coffee/networking/MessageHandler.hpp>

struct NetworkingFixture {
   class UpperStringHandler : public coffee::networking::MessageHandler {
   public:
      UpperStringHandler() : coffee::networking::MessageHandler("UpperStringHandler") {;}

      static std::shared_ptr<UpperStringHandler> instantiate() {
         return std::make_shared<UpperStringHandler>();
      }

   protected:
      void apply(const coffee::basis::DataBlock& message, coffee::networking::AsyncSocket& serverSocket)
         throw(coffee::basis::RuntimeException);
   };

   class LowerStringHandler : public coffee::networking::MessageHandler {
   public:
      LowerStringHandler() : coffee::networking::MessageHandler("LowerStringHandler") {;}

      static std::shared_ptr<LowerStringHandler> instantiate() {
         return std::make_shared<LowerStringHandler>();
      }

   protected:
      void apply(const coffee::basis::DataBlock& message, coffee::networking::AsyncSocket& serverSocket)
         throw(coffee::basis::RuntimeException);
   };

   class EchoHandler : public coffee::networking::MessageHandler {
   public:
      EchoHandler() : coffee::networking::MessageHandler("UpperStringHandler") {;}

      static std::shared_ptr<EchoHandler> instantiate() {
         return std::make_shared<EchoHandler>();
      }

   protected:
      void apply(const coffee::basis::DataBlock& message, coffee::networking::AsyncSocket& serverSocket)
         throw(coffee::basis::RuntimeException);
   };

   coffee::app::ApplicationServiceStarter app;
   std::shared_ptr<coffee::networking::NetworkingService> networkingService;
   std::thread thr;
   std::shared_ptr<coffee::networking::ServerSocket> upperServer;

   static const char* upperServerEndPoint;

   NetworkingFixture();
   ~NetworkingFixture();

   static void parallelRun(coffee::app::Application& app) {
      app.start();
   }
};


#endif /* TEST_NETWORKING_NETWORKINGFIXTURE_HPP_ */
