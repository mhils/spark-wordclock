#include "application.h"
#include "constants.h"



UDP TimeUdp;

const char timeServer[] = "pool.ntp.org";

const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

void sendNTPpacket(const char *address);
String timeapi();


String getTime(){
    //return "16:39:12"; 
    TimeUdp.begin(123);
    String response = timeapi();
    TimeUdp.stop();
    
    log("timeapi:");
    log(response);
    return response;
}



String timeapi()
{
	sendNTPpacket(timeServer); // send an NTP packet to a time server
	// wait to see if a reply is available

    int t = 0;
    while(t<2000 && !TimeUdp.parsePacket()){
        delay(2);
        t += 2;
    }
    
    if(t == 2000)
        return NULL;
    
    log("UDP Response Time:");
    log(t);

	TimeUdp.read(packetBuffer, NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes
    unsigned long secsSince1900 = (packetBuffer[40] << 24) | 
                                  (packetBuffer[41] << 16) | 
                                  (packetBuffer[42] <<  8) | 
                                  (packetBuffer[43] <<  0);

    int hour = (secsSince1900 % 86400L) / 3600;         
    int minute = (secsSince1900 % 3600L) / 60;
    int second = (secsSince1900 % 60L);

	char timestr[9];
	sprintf(timestr, "%02d:%02d:%02d", hour, minute, second);
	return String(timestr);
}


// send an NTP request to the time server at the given address
void sendNTPpacket(const char *address)
{
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12]  = 49;
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;

	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	TimeUdp.beginPacket(address, 123);
	TimeUdp.write(packetBuffer, NTP_PACKET_SIZE); //NTP requests are to port 123
	TimeUdp.endPacket();

}