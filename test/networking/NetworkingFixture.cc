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

#include "NetworkingFixture.hpp"

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/UnlimitedTraceWriter.hpp>
#include <coffee/logger/TtyWriter.hpp>
#include <coffee/logger/TraceMethod.hpp>

#include <coffee/networking/MessageHandler.hpp>
#include <coffee/networking/ServerSocket.hpp>

using namespace coffee;

//static
const char* NetworkingFixture::upperServerEndPoint = "tcp://*:5555";

NetworkingFixture::NetworkingFixture() : app("TestAppNetworkingFixture")
{
   const char* logFileName = "test/networking/trace.log";
   unlink (logFileName);
   logger::Logger::initialize(std::make_shared<logger::UnlimitedTraceWriter>(logFileName));
//   logger::Logger::initialize(std::make_shared<logger::TtyWriter>());
   logger::Logger::setLevel(logger::Level::Debug);

   networkingService = networking::NetworkingService::instantiate(app);
   networking::SocketArguments arguments;
   arguments.setMessageHandler(UpperStringHandler::instantiate()).addEndPoint("tcp://*:5555").addEndPoint("tcp://*:5556");
   upperServer = networkingService->createServerSocket(arguments);
   thr = std::thread(parallelRun, std::ref(app));
   app.waitUntilRunning();
   networkingService->waitEffectiveRunning();
}

NetworkingFixture::~NetworkingFixture() {
   LOG_THIS_METHOD();
   app.stop();
   thr.join();
}

void NetworkingFixture::UpperStringHandler::apply(const basis::DataBlock& message, networking::AsyncSocket& serverSocket)
   throw(basis::RuntimeException)
{
   std::string str(message.data(), message.size());
   std::transform(str.begin(), str.end(), str.begin(), [](unsigned char cc) { return std::toupper(cc); });
   basis::DataBlock response(str.c_str());
   serverSocket.send(response);
}

void NetworkingFixture::LowerStringHandler::apply(const basis::DataBlock& message, networking::AsyncSocket& serverSocket)
   throw(basis::RuntimeException)
{
   std::string str(message.data(), message.size());
   std::transform(str.begin(), str.end(), str.begin(), [](unsigned char cc) { return std::tolower(cc); });
   basis::DataBlock response(str.c_str());
   serverSocket.send(response);
}

void NetworkingFixture::EchoHandler::apply(const basis::DataBlock& message, networking::AsyncSocket& serverSocket)
   throw(basis::RuntimeException)
{
   serverSocket.send(message);
}
