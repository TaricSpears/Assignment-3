#ifndef __MSGSERVICE__
#define __MSGSERVICE__

#include "Arduino.h"
#include "Logger.h"

class Msg {
    String content;

   public:
    Msg(String content) {
        this->content = content;
    }

    String getContent() const {
        return content;
    }
};

class Pattern {
   public:
    virtual boolean match(const Msg& m) = 0;
};

class SetmodePattern : public Pattern {
   public:
    boolean match(const Msg& m) {
        return m.getContent().startsWith("SETMODE:");
    }
};

class SetopeningPattern : public Pattern {
   public:
    boolean match(const Msg& m) {
        return m.getContent().startsWith("SETOPENING:");
    }
};

class TemperaturePattern : public Pattern {
   public:
    boolean match(const Msg& m) {
        return m.getContent().startsWith("TEMPERATURE:");
    }
};

class MsgServiceClass {
   public:
    Msg* currentMsg;
    bool msgAvailable;

    void init();

    bool isMsgAvailable();
    Msg* receiveMsg();

    bool isMsgAvailable(Pattern& pattern);
    Msg* receiveMsg(Pattern& pattern);

    void sendMsg(const String& msg);
};

extern MsgServiceClass MsgService;

#endif
