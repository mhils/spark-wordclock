#define DebugSerial Serial
#define log(arg) DebugSerial.println(arg)
#define debug(arg) ""


#define bitSet(value, bit) ((value) |= (1UL << (bit)))


#define SHIFTOUT_A 1
#define SHIFTOUT_B 2
#define SHIFTOUT_C 3
#define SHIFTOUT_D 0

//
//
#define ES_IST          bitSet(out[SHIFTOUT_A], 0)
#define FUENF_MINUTEN   bitSet(out[SHIFTOUT_A], 1)
#define ZEHN_MINUTEN    bitSet(out[SHIFTOUT_A], 2)
#define ZWANZIG         bitSet(out[SHIFTOUT_A], 3)
#define DREI_MIN        bitSet(out[SHIFTOUT_A], 4)
#define VIERTEL         bitSet(out[SHIFTOUT_A], 5)
#define NACH            bitSet(out[SHIFTOUT_A], 6)

#define DREIVIERTEL     DREI_MIN;VIERTEL
//
//
#define VOR             bitSet(out[SHIFTOUT_B], 0)
#define HALB            bitSet(out[SHIFTOUT_B], 1)
#define ZWOELF          bitSet(out[SHIFTOUT_B], 2)
#define ZW              bitSet(out[SHIFTOUT_B], 3)
#define EI              bitSet(out[SHIFTOUT_B], 4)
#define N               bitSet(out[SHIFTOUT_B], 5)
#define S               bitSet(out[SHIFTOUT_B], 6)

#define ZWEI            ZW;EI
#define EIN             EI;N
#define EINS            EIN;S
//
//
#define IEBEN           bitSet(out[SHIFTOUT_C], 0)
#define DREI            bitSet(out[SHIFTOUT_C], 1)
#define FUENF           bitSet(out[SHIFTOUT_C], 2)
#define ELF             bitSet(out[SHIFTOUT_C], 3)
#define NEUN            bitSet(out[SHIFTOUT_C], 4)
#define VIER            bitSet(out[SHIFTOUT_C], 5)
#define ACHT            bitSet(out[SHIFTOUT_C], 6)

#define SIEBEN          S;IEBEN
//
//
#define ZEHN            bitSet(out[SHIFTOUT_D], 0)
#define SECHS           bitSet(out[SHIFTOUT_D], 1)
#define UHR             bitSet(out[SHIFTOUT_D], 2)
#define MIN_1           bitSet(out[SHIFTOUT_D], 3)
#define MIN_2           MIN_1;bitSet(out[SHIFTOUT_D], 4)
#define MIN_3           MIN_2;bitSet(out[SHIFTOUT_D], 5)
#define MIN_4           MIN_3;bitSet(out[SHIFTOUT_D], 6)
//
//