#include <adri_udp.h>


adri_udp * udpServer;
adri_udpMulti * udpMultiServer;



void setup()
{
    Serial.begin(115200);

	Serial.println("");

		
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
