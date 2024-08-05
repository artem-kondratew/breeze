#ifndef TOOLS_H
#define TOOLS_H


namespace tools {
    void blink(size_t cnt, size_t dt);
    void ledOn();
    void ledOff();
}


void tools::blink(size_t cnt, size_t dt) {
    for (size_t i = 0; i < cnt; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(dt);
        digitalWrite(LED_BUILTIN, LOW);
        delay(dt);
    }
}


void tools::ledOn() {
    digitalWrite(LED_BUILTIN, HIGH);
}


void tools::ledOff() {
    digitalWrite(LED_BUILTIN, LOW);
}


#endif // TOOLS_H
