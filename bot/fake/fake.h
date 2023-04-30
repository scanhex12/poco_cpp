#pragma once

#include <string>
#include <memory>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>

namespace telegram {

class TestCase;

class FakeServer {
public:
    FakeServer(const std::string& test_case);
    ~FakeServer();
    void Start();
    std::string GetUrl();
    void Stop();
    void StopAndCheckExpectations();

private:
    std::unique_ptr<TestCase> test_case_;
    std::unique_ptr<Poco::Net::ServerSocket> socket_;
    std::unique_ptr<Poco::Net::HTTPServer> server_;
};

}  // namespace telegram
