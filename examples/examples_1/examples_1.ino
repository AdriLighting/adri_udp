#include <adri_udp.h>
#include <adri_tools.h>
#include <adri_wifi.h>

#define DEVICENAME String("REVEILL")

adri_udp * udpServer;

adri_udpMulti * udpMultiServer;


void setup()
{
    Serial.begin(115200);

	SPIFFS.begin();	

	Serial.println("");

	wfifi_getID_toSPIFF("freebox_123_EXT", "phcaadax", "", ADWIFI_STATION);
	wifi_setup(DEVICENAME);
	#ifdef DEBUG
		String s;
		s = "\nwifi_print_networkInfo\n";
		wifi_print_networkInfo(&s);  
		s+= network_info(DEVICENAME);
		Serial.printf("%s\n", s.c_str());
	#endif
		
	udpServer 		= new adri_udp();
	udpServer->start();

	udpMultiServer 	= new adri_udpMulti();
	udpMultiServer->set_port(9300);
	udpMultiServer->parse_set_func(myParse);
	udpMultiServer->start();
 
}

void loop()
{
	udpServer->loop();
	udpMultiServer->loop();
}

void myParse(String s){
	Serial.printf("\n[myParse]\n%s\n", s.c_str());
}