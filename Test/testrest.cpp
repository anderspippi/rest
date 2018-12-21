#include "gtest/gtest.h"

#include "Rest/client.h"
#include "Rest/problemdetails.h"
#include "Rest/server.h"
#include "Support/queue.h"
#include "Support/tostring.h"

#include <glog/logging.h>

#include <list>
#include <string>
#include <thread>

namespace uiiit {
namespace rest {

struct TrivialServer : public Server {
  TrivialServer(const std::string& aUri)
      : Server(aUri) {
    (*this)(web::http::methods::GET,
            "throwing",
            [this](web::http::http_request aReq) { handleThrow(aReq); });
    (*this)(web::http::methods::GET,
            "slow",
            [this](web::http::http_request aReq) { handleSlow(aReq); });
    (*this)(web::http::methods::GET,
            "resources/(.*)/",
            [this](web::http::http_request aReq) { handleRead(aReq); });
    (*this)(web::http::methods::POST,
            "resources/(.*)/",
            [this](web::http::http_request aReq) { handleCreateUpdate(aReq); });
    (*this)(web::http::methods::PUT,
            "resources/(.*)/",
            [this](web::http::http_request aReq) { handleCreateUpdate(aReq); });
    (*this)(web::http::methods::DEL,
            "resources/(.*)/",
            [this](web::http::http_request aReq) { handleDelete(aReq); });
  }

  void handleThrow(web::http::http_request) {
    throw std::runtime_error("Something bad happened");
  }

  void handleSlow(web::http::http_request aReq) {
    web::json::value myValue;
    myValue["return"] = web::json::value("success");
    LOG(INFO) << "doing something complex";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    LOG(INFO) << "finished";
    aReq.reply(web::http::status_codes::OK, myValue);
  }

  void handleRead(web::http::http_request aReq) {
    web::json::value myValue;
    const auto       it = theResources.find(aReq.relative_uri().path());
    if (it == theResources.end()) {
      myValue["return"] = web::json::value("failure");
      aReq.reply(web::http::status_codes::NotFound, myValue);
      return;
    }
    aReq.reply(web::http::status_codes::OK, it->second);
  }

  void handleCreateUpdate(web::http::http_request aReq) {
    theResources[aReq.relative_uri().path()] = aReq.extract_json().get();
    web::json::value myValue;
    myValue["return"] = web::json::value("success");
    aReq.reply(web::http::status_codes::OK, myValue);
  }

  void handleDelete(web::http::http_request aReq) {
    theResources.erase(aReq.relative_uri().path());
    web::json::value myValue;
    myValue["return"] = web::json::value("success");
    aReq.reply(web::http::status_codes::OK, myValue);
  }

  std::map<std::string, web::json::value> theResources;
};

struct AnswerServer : public Server {
  AnswerServer(const std::string& aUri)
      : Server(aUri) {
    (*this)(web::http::methods::GET,
            "(.*)",
            [this](web::http::http_request aReq) { handleAnswer(aReq); });
  }

  void handleAnswer(web::http::http_request aReq) {
    std::ignore = aReq;
    web::json::value myValue;
    myValue["response_type"] = web::json::value("in_channel");
    myValue["text"]          = web::json::value("---\n*42*\n---");
    aReq.reply(web::http::status_codes::OK, myValue);
  }
};

struct TestRest : public ::testing::Test {
  TestRest()
      : theUri("http://localhost:10000/root/") {
  }

  const std::string theUri;
};

TEST_F(TestRest, test_server_ctor) {
  Server myServer(theUri);
  ASSERT_NO_THROW(myServer.start());
}

TEST_F(TestRest, test_client_ctor) {
  ASSERT_NO_THROW((Client(theUri)));
}

TEST_F(TestRest, test_server_client_base) {
  Server myServer(theUri);
  myServer.start();
  Client     myClient(theUri);
  const auto myRet = myClient.get();
  ASSERT_EQ(web::http::status_codes::NotFound, myRet.first);
  ASSERT_TRUE(myRet.second.has_object_field("ProblemDetails"));
  ASSERT_EQ(
      "Error processing method GET at /root/",
      ProblemDetails(myRet.second.as_object().at("ProblemDetails")).detail());
}

TEST_F(TestRest, test_server_client_trivial) {
  ASSERT_NO_THROW((TrivialServer(theUri)));
  TrivialServer myServer(theUri);
  myServer.start();
  Client           myClient(theUri);
  web::json::value myValue;
  myValue["response"] = web::json::value(42);

  // unsupported method
  {
    const auto myRet = myClient.patch(myValue, "resources/foo/?x=1&y=2");
    ASSERT_EQ(web::http::status_codes::NotFound, myRet.first);
    ASSERT_TRUE(myRet.second.has_object_field("ProblemDetails"));
    ASSERT_EQ(
        "Error processing method PATCH at /root/resources/foo/?x=1&y=2",
        ProblemDetails(myRet.second.as_object().at("ProblemDetails")).detail());
  }

  // find a resource not present
  {
    const auto myRet = myClient.get("resources/foo/?x=1&y=2");
    ASSERT_EQ(web::http::status_codes::NotFound, myRet.first);
    ASSERT_TRUE(myRet.second.has_field("return"));
    ASSERT_EQ("failure", myRet.second.as_object().at("return").as_string());
  }

  // create a resource
  {
    const auto myRet = myClient.post(myValue, "resources/foo/?x=1&y=2");
    ASSERT_EQ(web::http::status_codes::OK, myRet.first);
    ASSERT_TRUE(myRet.second.has_field("return"));
    ASSERT_EQ("success", myRet.second.as_object().at("return").as_string());
  }
  {
    const auto myRet = myClient.get("resources/foo/?x=1&y=2");
    ASSERT_EQ(web::http::status_codes::OK, myRet.first);
    ASSERT_TRUE(myRet.second.has_field("response"));
    ASSERT_EQ(42, myRet.second.as_object().at("response").as_integer());
  }

  // update a resource
  myValue["response"] = web::json::value(-42);
  {
    const auto myRet = myClient.put(myValue, "resources/foo/?x=1&y=2");
    ASSERT_EQ(web::http::status_codes::OK, myRet.first);
    ASSERT_TRUE(myRet.second.has_field("return"));
    ASSERT_EQ("success", myRet.second.as_object().at("return").as_string());
  }
  {
    const auto myRet = myClient.get("resources/foo/?x=1&y=2");
    ASSERT_EQ(web::http::status_codes::OK, myRet.first);
    ASSERT_TRUE(myRet.second.has_field("response"));
    ASSERT_EQ(-42, myRet.second.as_object().at("response").as_integer());
  }

  // delete a resource
  {
    const auto myRet = myClient.del("resources/foo/?x=1&y=2");
    ASSERT_EQ(web::http::status_codes::OK, myRet);
  }
  {
    const auto myRet = myClient.get("resources/foo/?x=1&y=2");
    ASSERT_EQ(web::http::status_codes::NotFound, myRet.first);
    ASSERT_TRUE(myRet.second.has_field("return"));
    ASSERT_EQ("failure", myRet.second.as_object().at("return").as_string());
  }

  // create many resources
  const std::list<std::string> myList({
      "foo",
      "bar",
      "baz",
      "another",
      "one",
  });
  for (const auto& myName : myList) {
    myValue["response"] = web::json::value(myName);
    const auto myRet =
        myClient.put(myValue, std::string("resources/") + myName + "/");
    ASSERT_EQ(web::http::status_codes::OK, myRet.first);
    ASSERT_TRUE(myRet.second.has_field("return"));
    ASSERT_EQ("success", myRet.second.as_object().at("return").as_string());
  }

  // check many resources
  for (const auto& myName : myList) {
    const auto myRet = myClient.get(std::string("resources/") + myName + "/");
    ASSERT_EQ(web::http::status_codes::OK, myRet.first);
    ASSERT_TRUE(myRet.second.has_field("response"));
    ASSERT_EQ(myName, myRet.second.as_object().at("response").as_string());
  }

  // throwing method
  {
    const auto myRet = myClient.get("throwing");
    ASSERT_EQ(web::http::status_codes::BadRequest, myRet.first);
    ASSERT_EQ(
        "Error processing method GET at /root/throwing",
        ProblemDetails(myRet.second.as_object().at("ProblemDetails")).detail());
  }
}

TEST_F(TestRest, test_problem_details) {
  ProblemDetails myFull(
      "a", "b", web::http::status_codes::BadGateway, "d", "e");

  ASSERT_TRUE(myFull.toJson().has_field("type"));
  ASSERT_TRUE(myFull.toJson().has_field("title"));
  ASSERT_TRUE(myFull.toJson().has_field("status"));
  ASSERT_TRUE(myFull.toJson().has_field("detail"));
  ASSERT_TRUE(myFull.toJson().has_field("instance"));
  ASSERT_EQ("a", myFull.toJson().as_object().at("type").as_string());
  ASSERT_EQ("b", myFull.toJson().as_object().at("title").as_string());
  ASSERT_EQ(502, myFull.toJson().as_object().at("status").as_integer());
  ASSERT_EQ("d", myFull.toJson().as_object().at("detail").as_string());
  ASSERT_EQ("e", myFull.toJson().as_object().at("instance").as_string());

  ASSERT_EQ("type: a, title: b, status: 502, detail: d, instance: e",
            toString(myFull));

  ProblemDetails myMinimal(web::http::status_codes::BadGateway, "d");

  ASSERT_FALSE(myMinimal.toJson().has_field("type"));
  ASSERT_FALSE(myMinimal.toJson().has_field("title"));
  ASSERT_TRUE(myMinimal.toJson().has_field("status"));
  ASSERT_TRUE(myMinimal.toJson().has_field("detail"));
  ASSERT_FALSE(myMinimal.toJson().has_field("instance"));
  ASSERT_EQ(502, myMinimal.toJson().as_object().at("status").as_integer());
  ASSERT_EQ("d", myMinimal.toJson().as_object().at("detail").as_string());

  ASSERT_EQ("status: 502, detail: d", toString(myMinimal));
}

TEST_F(TestRest, test_three_servers) {
  const std::string myUri1 = "http://localhost:10000/root/";
  TrivialServer     myServer1(myUri1);
  myServer1.start();

  const std::string myUri2 = "http://localhost:10000/another_root/";
  TrivialServer     myServer2(myUri2);
  myServer2.start();

  const std::string myUri3 = "http://localhost:10001/root/";
  TrivialServer     myServer3(myUri3);
  myServer3.start();

  const std::list<std::string> myUris({myUri1, myUri2, myUri3});

  for (const auto& myUri : myUris) {
    Client           myClient(myUri);
    web::json::value myValue;
    myValue["response"] = web::json::value(myUri);

    // POST
    const auto myRet1 = myClient.post(myValue, "resources/" + myUri);
    ASSERT_EQ(web::http::status_codes::OK, myRet1.first);
    ASSERT_TRUE(myRet1.second.has_field("return"));
    ASSERT_EQ("success", myRet1.second.as_object().at("return").as_string());

    // GET (success)
    const auto myRet2 = myClient.get("resources/" + myUri);
    ASSERT_EQ(web::http::status_codes::OK, myRet2.first);
    ASSERT_TRUE(myRet2.second.has_field("response"));
    ASSERT_EQ(myUri, myRet2.second.as_object().at("response").as_string());

    // GET (fail)
    for (const auto& myAnotherUri : myUris) {
      if (myUri == myAnotherUri) {
        continue;
      }
      Client     myAnotherClient(myAnotherUri);
      const auto myRet = myAnotherClient.get("resources/" + myUri);
      ASSERT_EQ(web::http::status_codes::NotFound, myRet.first);
    }
  }
}

TEST_F(TestRest, test_server_slow_response) {
  ASSERT_NO_THROW((TrivialServer(theUri)));
  TrivialServer myServer(theUri);
  myServer.start();
  Client              myClient(theUri);
  support::Queue<int> myQueue;

  std::thread myThread([&]() {
    Client myClient(theUri);
    for (auto i = 0; i < 10; i++) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      const auto myRet = myClient.get("resources/foo");
      ASSERT_EQ(web::http::status_codes::NotFound, myRet.first);
      myQueue.push(1);
    }
  });

  const auto myRet = myClient.get("slow");
  ASSERT_EQ(web::http::status_codes::OK, myRet.first);
  myQueue.push(0);

  myThread.join();

  // check that the slow task completed while the concurrent GET operations were
  // being served
  int myPos = -1;
  int myCnt = 0;
  while (myQueue.size() > 0) {
    if (myQueue.pop() == 0) {
      myPos = myCnt;
    }
    myCnt++;
  }
  ASSERT_GE(myPos, 2);
  ASSERT_LE(myPos, 7);
}

TEST_F(TestRest, DISABLED_test_answer_server) {
  AnswerServer myServer("http://0.0.0.0:10042/");
  myServer.start();
  pause();
}

} // namespace rest
} // namespace uiiit
