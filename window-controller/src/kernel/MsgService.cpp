#include "MsgService.h"

#include "Arduino.h"

String content;

MsgServiceClass MsgService;

bool MsgServiceClass::isMsgAvailable() { return msgAvailable; }

Msg *MsgServiceClass::receiveMsg() {
  if (msgAvailable) {
    Msg *msg = currentMsg;
    msgAvailable = false;
    currentMsg = NULL;
    content = "";
    return msg;
  } else {
    return NULL;
  }
}

void MsgServiceClass::init() {
  Serial.begin(115200);
  content.reserve(256);
  content = "";
  currentMsg = NULL;
  msgAvailable = false;
}

void MsgServiceClass::sendMsg(const String &msg) { Serial.println(msg); }

void serialEvent() {
  /* reading the content */
  while (Serial.available()) {
    char ch = (char)Serial.read();
    if (ch == '\n') {
      if (content.length() > 0) {
        if (MsgService.currentMsg != NULL) {
          delete MsgService.currentMsg; // Free the old message
        }
        MsgService.currentMsg = new Msg(content);
        MsgService.msgAvailable = true;
      }
    } else {
      content += ch;
    }
  }
}

bool MsgServiceClass::isMsgAvailable(Pattern &pattern) {
  return (msgAvailable && pattern.match(*currentMsg));
}

Msg *MsgServiceClass::receiveMsg(Pattern &pattern) {
  if (msgAvailable && pattern.match(*currentMsg)) {
    Msg *msg = currentMsg;
    msgAvailable = false;
    currentMsg = NULL;
    content = "";
    return msg;
  } else {
    return NULL;
  }
}
