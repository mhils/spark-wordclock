// This #include statement was automatically added by the Spark IDE.
#include "constants.h"

// PINS
const int serialClearPin = D0; //OE pin
const int outputEnablePin = D1; //OE pin
const int clockPin = D3; //SRCLK pin
const int latchPin = D4; // RCLK pin
const int dataPin = D5; //SER pin
const int photoresistorPin = A7;

static unsigned long  const ONE_DAY_MILLIS = 24 * 60 * 60 * 1000;
static unsigned char  const DEFAULT_TIME_ZONE = 1;
static unsigned short const BRIGHTNESS_SMOOTH = 100; //higher value -> slower brightness change.

SYSTEM_MODE(SEMI_AUTOMATIC);

// Variables
int brightness = 0;

int configure(String command){
    log("Received command:");
    log(command);
    char opcode = command.charAt(0);
    switch(opcode){
        case 't':  //timezone offset
            Time.zone(command.substring(1).toInt());
            break;
        case 'd':  //blink
            blink();
            break;
        case 'a':  //blink
            show_all();
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
    
    Time.zone(DEFAULT_TIME_ZONE);
    Time.setDSTOffset(1.0);
    
    log("Connecting to particle cloud...");
    
    Particle.connect();
    if(waitFor(Particle.connected, 10000)) {
        log("Connected.");
        //register api calls
        Spark.function("configure", configure);
        Spark.variable("brightness", &brightness, INT);
    } else {
        log("No connection.");
        Time.setTime(946728010); // ten seconds after noon, because we have just waited ten seconds.
    }
    
    log("Initialized.");
}

void loop() {
    debug("Adjust time...");
    adjustTime();
    debug("Adjust brightness...");
    adjustBrightness();
    debug("Show time...");
    showTime();
    debug("Done! Sleeping...");
    delay(BRIGHTNESS_SMOOTH);
}

unsigned long lastSync = 0;
void adjustTime(){
    // if there is no cloud connection, we can neither
    // update the time nor do we want to account for DST changes,
    // as the user cannot set the date.
    if (!Particle.connected()) {
        return;
    }
    if (millis() - lastSync > ONE_DAY_MILLIS) {
        Particle.syncTime();
        lastSync = millis();
    }
    adjustDST();
}

void adjustDST(){
    int hour = Time.hour(),
        day = Time.day(),
        month = Time.month(),
        year = Time.year();
        
    // adapted from https://en.wikipedia.org/wiki/Summer_Time_in_Europe
    int startDstDay = 31 - ((((5 * year) / 4) + 4) % 7);  // Day in March
    int stopDstDay  = 31 - ((((5 * year) / 4) + 1) % 7);  // Day in October
    
    bool isDST = (
        (3 < month && month < 10) 
        ||
        (month == 3 && day > startDstDay)
        ||
        (month == 3 && day == startDstDay && hour >= 2)
        ||
        (month == 10 && day < stopDstDay)
        ||
        (month == 10 && day == stopDstDay && hour < (2 + (Time.isDST() ? Time.getDSTOffset() : 0 )))
    );
    
    if (isDST) {
        Time.beginDST();
    } else {
        Time.endDST();
    }
}


static unsigned short const start = 200;
static unsigned short const stop = 1000;
static unsigned char  const smooth = 1;
short brightnessCached = -1;
void adjustBrightness() {
    
    brightness = analogRead(photoresistorPin);
    log(brightness);

    short b = map(
        constrain(brightness, start, stop),
        start, stop, 
        1-smooth, 255+smooth);

    if(brightnessCached == -1){
        b = constrain(b, 1, 255);
    } else if(brightnessCached < b-smooth) {
        b = brightnessCached + 1;
    } else if(brightnessCached > b+smooth) {
        b = brightnessCached - 1;
    } else {
        return;
    }
    
    analogWrite(outputEnablePin, 255 - b);
    brightnessCached = b;
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
    
    unsigned char hour = Time.hour() % 12;
    unsigned char min = Time.minute();
    
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

void show_all(){
    digitalWrite(outputEnablePin, LOW);

    unsigned char out[] = {255,255,255,255};
    shiftOutArray(out);
    Spark.sleep(240);
    delay(240*1000);
}
