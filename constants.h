#define DebugSerial Serial
#define log(arg) DebugSerial.println(arg)
#define debug(arg) ""

#define bitSet(value, bit) ((value) |= (1UL << (bit)))

//
//
#define ES_IST          bitSet(out[0], 0)
#define FUENF_MINUTEN   bitSet(out[0], 1)
#define ZEHN_MINUTEN    bitSet(out[0], 2)
#define ZWANZIG         bitSet(out[0], 3)
#define DREI_MIN        bitSet(out[0], 4)
#define VIERTEL         bitSet(out[0], 5)
#define NACH            bitSet(out[0], 6)

#define DREIVIERTEL     DREI_MIN;VIERTEL
//
//
#define VOR             bitSet(out[1], 0)
#define HALB            bitSet(out[1], 1)
#define ZWOELF          bitSet(out[1], 2)
#define ZW              bitSet(out[1], 3)
#define EI              bitSet(out[1], 4)
#define N               bitSet(out[1], 5)
#define S               bitSet(out[1], 6)

#define ZWEI            ZW;EI
#define EIN             EI;N
#define EINS            EIN;S
//
//
#define IEBEN           bitSet(out[2], 0)
#define DREI            bitSet(out[2], 1)
#define FUENF           bitSet(out[2], 2)
#define ELF             bitSet(out[2], 3)
#define NEUN            bitSet(out[2], 4)
#define VIER            bitSet(out[2], 5)
#define ACHT            bitSet(out[2], 6)

#define SIEBEN          S;IEBEN
//
//
#define ZEHN            bitSet(out[3], 0)
#define SECHS           bitSet(out[3], 1)
#define UHR             bitSet(out[3], 2)
#define MIN_1           bitSet(out[3], 3)
#define MIN_2           bitSet(out[3], 4)
#define MIN_3           bitSet(out[3], 5)
#define MIN_4           bitSet(out[3], 6)
//
//