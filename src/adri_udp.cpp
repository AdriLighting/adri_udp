#include "adri_udp.h"

#include <udp.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <adri_tools.h>
#include <adri_wifiConnect.h>

// #define DEBUG_UDP

void adri_udp_parse(String s){
	// Serial.printf("\n[adri_udp_parse]\n%s\n", s.c_str());
}

adri_udp::adri_udp(){
	_parse = adri_udp_parse;
	// _timerSendPacket 	= new adri_timer(25, "", false);
	// _timerRecPacket 	= new adri_timer(10, "", false);
}

void adri_udp::set_port(uint16_t port) {
	_port = port;
}

void adri_udp::remote_print(){
	IPAddress 	ip 		= _server.remoteIP();
	uint16_t 	port 	= _server.remotePort();

	String ip_str = ip2string(ip);

	Serial.printf("\n[remote_print] ipStr: %10s port: %10d\n", ip_str.c_str(), port );
	#ifdef UDP_CHECKCLIENT
		udpServer_printAllClient();
	#endif
}
IPAddress adri_udp::remoteIP(){
	return _server.remoteIP();
}

// #define UDP_IN_MAXSIZE 8192 1472
#define UDP_IN_MAXSIZE 1024
// #define UDP_OUT_MAXSIZE 1024

char adri_udp_packetBuffer[UDP_IN_MAXSIZE + 1]; 
// char adri_udp_outPacketBuffer[UDP_OUT_MAXSIZE + 1]; 

bool adri_udp::receive() {
	// if (!_timerRecPacket->isActivate()) {
	// 	_timerRecPacket->activate();
	// } else {
	// 	if (!_timerRecPacket->loop()) return false;
	// 	else {
	// 		_timerRecPacket->activate(false);
	// 	}
	// }

	uint16_t packet_size = _server.parsePacket();

	if (!packet_size) return false;


	if (packet_size > UDP_IN_MAXSIZE || packet_size < 1) return false;

	int length = _server.read(adri_udp_packetBuffer, UDP_IN_MAXSIZE);

	adri_udp_packetBuffer[length]=0;

	#ifdef DEBUG_UDP
		fsprintf("\n[adri_udp::receive][size: %d]\n%s\n", packet_size, adri_udp_packetBuffer);
	#endif

   return true;
}

void adri_udp::send(String transmit_buffer) {

	// if (!_timerSendPacket->isActivate()) {
	// 	_timerSendPacket->activate();
	// } else {
	// 	if (!_timerSendPacket->loop()) return;
	// 	else {
	// 		_timerSendPacket->activate(false);
	// 	}
	// }

	unsigned int msg_length = transmit_buffer.length();

	if (msg_length ==0) return;

	byte* p = (byte*)malloc(msg_length);
	memcpy(p, transmit_buffer.c_str(), msg_length);
	// sprintf(adri_udp_outPacketBuffer,"%s",transmit_buffer.c_str());

	_server.beginPacket(_server.remoteIP(), _server.remotePort());

	_server.write(p, msg_length);		

	_server.endPacket();	

#ifdef DEBUG_UDP
	// char buffer[100];
	// sprintf(buffer, "\n[size] %d\n", size);
	// debugPrint(String(buffer));

	// Serial.printf("\n[adri_udp::send][size: %d]\n%s\n",  size, transmit_buffer.c_str());
	// fsprintf("\n[adri_udp::send][size: %d]\n%s\n",  msg_length, transmit_buffer.c_str());

#endif  

	free(p);
}

void adri_udp::send_toIp(String transmit_buffer, const char * ip, uint16_t port) {
	int size = transmit_buffer.length();
	if (size==0)return;
	// int start, end;
	char buf[size+1];	
	sprintf(buf,"%s",transmit_buffer.c_str());
	_server.beginPacket(ip, port);
	_server.write(buf);			
	_server.endPacket();
#ifdef DEBUG_UDP
	Serial.printf("\n[size] %d\n%s\n",  size, transmit_buffer.c_str());
#endif  
}
void adri_udp::send_toIp(String transmit_buffer, String s_ip, String s_port) {
	char buff[80];
	sprintf(buff, "%s", s_ip.c_str());
	char * ip = buff;
	uint16_t port = s_port.toInt();
	send_toIp(transmit_buffer, ip, port) ;
}

void adri_udp::parse() { 
	_parse(String(adri_udp_packetBuffer));
	yield();

}
void adri_udp::parse_set_func(parse_f f) {
	_parse = f;
}

void adri_udp::start(){
	_server.begin(_port);
}

void adri_udp::loop() {
	if (receive()) parse();
}





adri_udpMulti::adri_udpMulti(){
	_parse = adri_udp_parse;
}

void adri_udpMulti::set_port(uint16_t port) {
	_port = port;
}

void adri_udpMulti::set_ip(IPAddress ip) {
	_ip = ip;
}

void adri_udpMulti::parse_set_func(parse_f f) {
	_parse = f;
}

char adri_udpMulti_packetBuffer[UDP_IN_MAXSIZE + 1]; 

bool adri_udpMulti::receive(String * receive_buffer) {

	uint16_t packet_size = _server.parsePacket();

	if (!packet_size) return false;

	if (packet_size > UDP_IN_MAXSIZE || packet_size < 3) return false;

	int length = _server.read(adri_udpMulti_packetBuffer, UDP_IN_MAXSIZE);

	adri_udpMulti_packetBuffer[length]=0;

	* receive_buffer = String(adri_udpMulti_packetBuffer);


   	return true;
}	

void adri_udpMulti::send(String transmit_buffer) {

    int size = transmit_buffer.length();
	char buf[size+1];
	sprintf(buf, "%s", 
		transmit_buffer.c_str()
	);	
	_server.beginPacketMulticast(_ip, _port, WiFi.localIP());
	_server.write(buf);
	_server.endPacket();	

}	

void adri_udpMulti::parse(String * udp_msg) { 
	String msg = *udp_msg;	
	_parse(msg);
}	

void adri_udpMulti::start(){
	_server.beginMulticast(WiFi.localIP(), _ip, _port);
}

void adri_udpMulti::loop() {
	if (receive(&_receive_buffer)) parse(&_receive_buffer);
}



	#define MAXCLIENT 5
	int udpServer_client_cnt = 0;
	int udpServer_client_pos = 0;
	udpServer * udpServerArray[MAXCLIENT];
	unsigned long udpServerCompar_max = 60000;

	udpServer::udpServer(IPAddress ip, uint16_t port){
		_ip 			= ip;
		_port 			= port;
		_isConnected 	= true;
		_check 			= false;
		_checkStart 	= false;
		_lastCall 		= millis();
	}

	void udpServer_updateClient(IPAddress ip) {

		if (udpServer_client_cnt <= 0) return;

		int ret = -1;
		for (int i = 0; i < udpServer_client_cnt; ++i){
			if (udpServerArray[i]->_ip == ip) {
				ret = i;
				break;
			}
		}

		if (ret < 0) return;

		udpServerArray[ret]->_check 				= false;
		udpServerArray[ret]->_checkStart 			= false;
		udpServerArray[ret]->_isConnected 			= true;
		udpServerArray[ret]->_lastCall 				= millis();

		
	}

	void udpServer::compare(){


		if ((millis()-_checkInterval) > udpServerCompar_max) {
			_checkInterval = millis();
			_checkStart = true;
		}
		if (_checkStart){
			if ((!_check) && ((millis()-_lastCall) > 5000)) {
				_check = true;
				_waiting = millis();
				String core;
				core 	= 	"{";
				core 	+= 	jsonAddStringsValue (true, 	"op", "server_request");	
				core 	+= 	"}";
				// udp_send(core);
			}
			if (_check) {
				if  ( (millis()-_waiting) > 1000)  {
					_check = false;
					_checkStart = false;
					if ((millis()-_lastCall) > 5000)  {
						_isConnected = false;
					}
				}				
			}
		}
	}

	void udpServer_checkClient() {
		// if (udpServer_client_cnt <= 0) {if (_programm_appi_upd) _programm_appi_upd = false;return;}
		// if (!_programm_appi_upd) return;
		if (udpServer_client_cnt <= 0)

		for (int i = 0; i < udpServer_client_cnt; ++i){
			udpServerArray[i]->compare();
		}
	} 

	boolean udpServer_isClient(IPAddress ip, uint16_t port) {

		if (udpServer_client_cnt <= 0) return false;

		boolean ret = false;
		for (int i = 0; i < udpServer_client_cnt; ++i){
			if (udpServerArray[i]->_ip == ip) {
				ret = true;
				break;
			}
		}
		return ret;
	}

	int udpServer_addClient(IPAddress ip, uint16_t port){

		if (udpServer_isClient(ip, port)) {
			udpServer_updateClient(ip);
			return -1;
		}

		if (udpServer_client_cnt < MAXCLIENT) {

			String ip_str = ip2string(ip);
			fsprintf("\n[udpServer_addClient] adding client nbr:%d ip:%s, port:%d\n", udpServer_client_cnt, ip_str.c_str(), port);

			udpServer_client_pos = udpServer_client_cnt;

			// if (!_programm_appi_upd) _programm_appi_upd = true;

			udpServerArray[udpServer_client_cnt] = new udpServer(ip, port);
			udpServer_client_cnt++;

			return udpServer_client_cnt-1;
		}
		return -1;	
	}
