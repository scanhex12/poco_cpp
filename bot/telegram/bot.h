#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>
#include <iostream>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <vector>
#include <optional>
#include <stdexcept>
#include <string>

class TelegramBot {
public:
    TelegramBot(std::string token = "5971517780:AAH-EB_kaQsUA0wC_e0S6pOkP8cLxPuycW0")
        : token_(token) {
    }

    void GetMe(const std::string& api_endpoint) {
        std::string uri_adress = api_endpoint + "bot" + token_ + "/getMe";
        Poco::URI uri(uri_adress);
        std::string path(uri.getPathAndQuery());
        if (path.empty()) {
            path = "/";
        }
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path);
        Poco::Net::HTTPResponse response;
        session.sendRequest(request);

        std::istream& s = session.receiveResponse(response);
        if (response.getStatus() != 200) {
            throw std::runtime_error("Invalid request");
        }
        std::string json;
        char c;
        while (s.get(c)) {
            json.push_back(c);
        }
    }

    std::optional<std::string> GetUpdates(const std::string& api_endpoint,
                                          std::optional<int> offset = std::nullopt,
                                          std::optional<int> timeout = std::nullopt) {
        std::string uri_adress = api_endpoint + "bot" + token_ + "/getUpdates";
        if (offset) {
            uri_adress += "?";
            uri_adress += "offset=";
            uri_adress += std::to_string(*offset);
        }
        if (timeout) {
            if (offset) {
                uri_adress += "&";
            } else {
                uri_adress += "?";
            }

            uri_adress += "timeout=";
            uri_adress += std::to_string(*timeout);
        }

        Poco::URI uri(uri_adress);
        std::string path(uri.getPathAndQuery());
        if (path.empty()) {
            path = "/";
        }
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path);
        Poco::Net::HTTPResponse response;
        session.sendRequest(request);

        std::istream& s = session.receiveResponse(response);
        std::string json;
        char c;
        while (s.get(c)) {
            json.push_back(c);
        }

        ExtractId(json);
        UpdateMaxId(json);
        return ExtractMessage(json);
    }

    void SendMessage(const std::string& api_endpoint, const std::string& message,
                     std::optional<int> reply_message) {
        std::string uri_adress = api_endpoint + "bot" + token_ + "/sendMessage";
        Poco::URI uri(uri_adress);
        std::string path(uri.getPathAndQuery());
        if (path.empty()) {
            path = "/";
        }
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, path);

        Poco::JSON::Object json;
        json.set("text", message);
        json.set("chat_id", id_conversation_);
        if (reply_message) {
            json.set("reply_to_message_id", *reply_message);
        }
        std::stringstream ss;
        json.stringify(ss);
        request.setContentLength(ss.str().size());
        request.setContentType("application/json");

        std::ostream& os = session.sendRequest(request);
        json.stringify(os);
        Poco::Net::HTTPResponse response;
        std::stringstream sss;
        std::istream& is = session.receiveResponse(response);
        Poco::StreamCopier::copyStream(is, sss);
    }

    int GetMaxUpdateId() const {
        return max_message_id_;
    }

    int GetConversationId() const {
        return id_conversation_;
    }

private:
    std::string token_;
    int id_conversation_ = -1;
    int max_message_id_ = 0;

    std::optional<std::string> ExtractMessage(const std::string& json_from_update) {
        Poco::JSON::Parser parser;
        auto result = parser.parse(json_from_update);

        Poco::JSON::Object::Ptr sub_object = result.extract<Poco::JSON::Object::Ptr>();
        auto messages = sub_object->get("result");

        Poco::JSON::Array::Ptr all_messages = messages.extract<Poco::JSON::Array::Ptr>();
        size_t total_length = all_messages->size();
        if (total_length == 0) {
            return std::nullopt;
        }
        auto last_message = all_messages->get(total_length - 1);

        std::string text_message = ExtractField(last_message.toString(), {"message", "text"});
        return text_message;
    }

    void ExtractId(const std::string& json_from_update) {
        if (id_conversation_ != -1) {
            return;
        }
        Poco::JSON::Parser parser;
        auto result = parser.parse(json_from_update);

        Poco::JSON::Object::Ptr sub_object = result.extract<Poco::JSON::Object::Ptr>();
        auto messages = sub_object->get("result");

        Poco::JSON::Array::Ptr all_messages = messages.extract<Poco::JSON::Array::Ptr>();
        auto last_message = all_messages->get(0);

        std::string str_id_conversation =
            ExtractField(last_message.toString(), {"message", "chat", "id"});
        id_conversation_ = std::stoi(str_id_conversation);
    }

    void UpdateMaxId(const std::string& json_from_update) {
        Poco::JSON::Parser parser;
        auto result = parser.parse(json_from_update);

        Poco::JSON::Object::Ptr sub_object = result.extract<Poco::JSON::Object::Ptr>();
        auto messages = sub_object->get("result");

        Poco::JSON::Array::Ptr all_messages = messages.extract<Poco::JSON::Array::Ptr>();
        size_t array_length = all_messages->size();
        if (array_length == 0) {
            return;
        }
        auto last_message = all_messages->get(array_length - 1);

        std::string str_id_conversation = ExtractField(last_message.toString(), {"update_id"});
        max_message_id_ = std::max(max_message_id_, std::stoi(str_id_conversation));
    }

    std::string ExtractField(std::string json, std::vector<std::string> path) {
        Poco::JSON::Parser parser;
        auto result = parser.parse(json);

        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
        std::string test = object->get(path[0]);
        if (path.size() == 1) {
            return test;
        } else {
            std::reverse(path.begin(), path.end());
            path.pop_back();
            std::reverse(path.begin(), path.end());
            return ExtractField(test, path);
        }
    }
};
