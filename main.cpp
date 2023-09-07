#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class DBConnection {
public:
    DBConnection(std::string ip, uint16_t port) : _ip(ip), _port(port) {}
    virtual ~DBConnection() {}

    virtual bool open() = 0 {}
    virtual bool close() = 0 {}
    virtual void execQuery() = 0 {}

protected:
    std::string _ip;
    uint16_t _port;
};

class ClassThatUsesDB : public DBConnection {
public:
    ClassThatUsesDB(std::string ip, uint16_t port) : DBConnection(ip, port) {}
    ~ClassThatUsesDB() {}

    virtual bool open() override {                       //имитация подключения к БД
        if (!_ip.empty() && _port)
        {
            std::cout << "Connect: success. IP " << _ip << " PORT " << _port << std::endl;
            return true;
        }
        std::cout << "Connect: error." << std::endl;
        return false;
    }

    virtual bool close() override {                       //имитация отключения от БД
        if (!_ip.empty() && _port)
        {
            _ip.clear();
            _port = 0;
            std::cout << "Disconnect: success." << std::endl;
            return true;
        }
        else {
            std::cout << "Disconnect: invalid IP/PORT." << std::endl;
            return true;
        }
    }

    virtual void execQuery() override {                     //имитация запроса
        if (!_ip.empty() && _port)
        {
            std::cout << "Request sent to address " << _ip << ":" << _port << std::endl;
            return;
        }
        std::cout << "ERROR: request don't sent. Invalid IP/PORT." << std::endl;
        return;
    }
};

void testWork(DBConnection* test) {
    test->open();
    test->execQuery();
    test->execQuery();
    test->close();
}

class MockDBConnection : public DBConnection {
public:
    MockDBConnection(std::string ip, uint16_t port) : DBConnection(ip, port) {}
    ~MockDBConnection() {}

    MOCK_METHOD(bool, open, (), (override));
    MOCK_METHOD(void, execQuery, (), (override));
    MOCK_METHOD(bool, close, (), (override));
};

TEST(MockDBConnectionTest, test1) {
    MockDBConnection testObject1("127.0.0.1", 55555);

    EXPECT_CALL(testObject1, open).Times(::testing::AtLeast(1));
    EXPECT_CALL(testObject1, close).Times(::testing::AtLeast(1));
    EXPECT_CALL(testObject1, execQuery).Times(::testing::AtLeast(1));

    testWork(&testObject1);
}

TEST(ClassThatUsesDBTest, test2) {
    ClassThatUsesDB testObject2("127.0.0.1", 55555);

    EXPECT_EQ(testObject2.open(), true);
    testObject2.execQuery();
    EXPECT_EQ(testObject2.close(), true);
}

TEST(ClassThatUsesDBTest, test3) {
    ClassThatUsesDB testObject3("127.0.0.1", 0);

    EXPECT_EQ(testObject3.open(), false);
    testObject3.execQuery();
    EXPECT_EQ(testObject3.close(), true);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}