#include "constants.h"
#include "timeapi.h"
#include <time.h>

// PINS
const int serialClearPin = D0; //OE pin
const int outputEnablePin = D1; //OE pin
const int clockPin = D3; //SRCLK pin
const int latchPin = D4; // RCLK pin
const int dataPin = D5; //SER pin
const int photoresistorPin = A0;
const int syncIntervalMillis = 1000*60*60*4; //sync every  4 hours

// Variables
float brightness_mult = 1;
int time_offset_hour = 1;

struct tm current_time;

unsigned long lastSync = 0;
unsigned long lastTick = 0;

int configure(String command){
    log("Received command:");
    log(command);
    char opcode = command.charAt(0);
    switch(opcode){
        case 'b':  //brightness
            brightness_mult = command.substring(1).toFloat();
            break;
        case 't':  //timezone offset
            time_offset_hour = command.substring(1).toInt();
            lastSync = 0;
            break;
        case 'd':  //blink
            blink();
            break;
        default:
            return -1;
    }
    return 0;
}

void setup() {
    // Clear shift registers first
    pinMode(serialClearPin, OUTPUT);
    digitalWrite(serialClearPin, LOW);
    delay(10);
    digitalWrite(serialClearPin, HIGH);
    
    DebugSerial.begin(9600);
    log("Initializing...");
    
    //set pin modes of remaining pins
    pinMode(outputEnablePin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(photoresistorPin, INPUT_PULLDOWN);
    
    //register api call
    Spark.function("configure", configure);
    
    log("Initialized!");
}

void loop() {
    debug("Adjust time...");
    adjustTime();
    debug("Adjust brightness...");
    adjustBrightness();
    debug("Show time...");
    showTime();
    debug("Done! Sleeping...");
    delay(100);
}

void syncTime(){
    log("Sync time...");
    const char *time_details = getTime().c_str(); // FIXME
    strptime(time_details, "%H:%M:%S", &current_time);
    current_time.tm_hour = (current_time.tm_hour + time_offset_hour) % 24; //we don't care about day etc...
}


void adjustTime(){
    
    unsigned long currTick = millis() / 1000;
    unsigned short secondsToAdd = currTick - lastTick;
    while(secondsToAdd > 0){
        current_time.tm_sec++;
        if(current_time.tm_sec == 60){
            current_time.tm_sec = 0;
            current_time.tm_min++;
            if(current_time.tm_min == 60){
                current_time.tm_min = 0;
                current_time.tm_hour = (current_time.tm_hour + 1) % 24; //we don't care about day etc...
            }
        }
        secondsToAdd--;
    }
    lastTick = currTick;
    char logmsg[24];
    sprintf(logmsg, "Time adjusted: %02d:%02d:%02d", current_time.tm_hour, current_time.tm_min, current_time.tm_sec);
    debug(logmsg);
    
    if(lastSync == 0 || // 0 -> always resync
      (lastSync + syncIntervalMillis < millis()) || 
      (lastSync > millis())) { // overflow
        syncTime();
        lastSync = millis();
    }

}

short brightnessCached = -1;
void adjustBrightness() {
    unsigned char b = 255; //TODO: Calc from photoresistor
    if(0 <= brightness_mult && brightness_mult <= 1){
        b *= brightness_mult;
    } else if(-255 <= brightness_mult) {
        b = -brightness_mult;
    }
    if(brightnessCached != b) {
        analogWrite(outputEnablePin, 255 - b);
        brightnessCached = b;
    }
}

void shiftOutArray(unsigned char out[]) {
    
    /*
    for (int i = 0; i < 4; ++i) {
        for (int j = 0;j < 7; ++j) {
          DebugSerial.print((out[i] >> j) & 1);
        }
    }
    log("");
    */
    
    digitalWrite(latchPin, LOW);
    for(int i=3;i>=0;i--){
        shiftOut(dataPin, clockPin, LSBFIRST, out[i]);
    }
    digitalWrite(latchPin, HIGH);
}

void showTime(){
    unsigned char out[] = {0, 0, 0, 0};
    
    unsigned char hour = current_time.tm_hour % 12;
    unsigned char min = current_time.tm_min;
    
    ES_IST;
    
    // <5 Minutes
    switch(min % 5){
        case 1:
            MIN_1;
            break;
        case 2:
            MIN_2;
            break;
        case 3:
            MIN_3;
            break;
        case 4:
            MIN_4;
            break;
    }
    
    // Minutes - verbose but readable
    if(min < 5) {
        UHR;
    } else if(min < 10) {
        FUENF_MINUTEN;
        NACH;
    } else if(min < 15) {
        ZEHN_MINUTEN;
        NACH;
    } else if(min < 20) {
        VIERTEL;
        NACH;
    } else if(min < 25) {
        ZWANZIG;
        NACH;
    } else if(min < 30) {
        FUENF_MINUTEN;
        VOR;
        HALB;
    } else if(min < 35) {
        HALB;
    } else if(min < 40) {
        FUENF_MINUTEN;
        NACH;
        HALB;
    } else if(min < 45) {
        ZWANZIG;
        VOR;
    } else if(min < 50) {
        VIERTEL;
        VOR;
    } else if(min < 55) {
        ZEHN_MINUTEN;
        VOR;
    } else {
        FUENF_MINUTEN;
        VOR;
    }
    
    if(min >= 25) {
        hour = (hour + 1) % 12;
    }
    
    // Hour
    switch(hour){
        case 0:
            ZWOELF;
            break;
        case 1:
            if(min < 5) {
                EIN;
            } else {
                EINS;
            }
            break;
        case 2:
            ZWEI;
            break;
        case 3:
            DREI;
            break;
        case 4:
            VIER;
            break;
        case 5:
            FUENF;
            break;
        case 6:
            SECHS;
            break;
        case 7:
            SIEBEN;
            break;
        case 8:
            ACHT;
            break;
        case 9:
            NEUN;
            break;
        case 10:
            ZEHN;
            break;
        case 11:
            ELF;
            break;
    }
    
    shiftOutArray(out);
    
}

void blink(){
    digitalWrite(outputEnablePin, LOW);
    for(int i=0; i<=3; i++){
        for(int j=0; j<7; j++){
            unsigned char out[] = {0,0,0,0};
            bitSet(out[i],j);
            
            shiftOutArray(out);
            log(i);
            log(j);
            delay(250);
        }
    }
}