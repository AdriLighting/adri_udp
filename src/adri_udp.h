#ifndef ADRI_UDP_H
      	#define ADRI_UDP_H
		#include <WiFiUdp.h>
		#include <arduino.h>
		#include <adri_timer.h>

		typedef void (*parse_f)(String s);  

		class adri_udp
		{
			// adri_timer 	* _timerSendPacket;
			adri_timer 	* _timerRecPacket;
			uint16_t 	_port 			= 9100;
			WiFiUDP 	_server;
			String 		_receive_buffer;
			parse_f 	_parse;

		public:
			adri_udp();
			~adri_udp();
			void set_port(uint16_t port);
			void remote_print();
			IPAddress remoteIP();
			bool receive() ;
			void send_toIp(String transmit_buffer, const char * ip, uint16_t port) ;
			void send_toIp(String transmit_buffer, String s_ip, String s_port);
			void send(String transmit_buffer);
			void parse();
			void parse_set_func(parse_f f);	
			void start();
			void loop();		
		};

		class adri_udpMulti
		{
			IPAddress 	_ip    		= {239, 0, 0, 57};
			uint16_t 	_port 		= 9200;
			WiFiUDP 	_server;
			String 		_receive_buffer;
			parse_f 	_parse;

		public:
			adri_udpMulti();
			~adri_udpMulti();
			bool receive(String * receive_buffer);
			void send(String transmit_buffer);
			void parse(String * udp_msg);	
			void parse_set_func(parse_f f);	
			void set_port(uint16_t port);
			void set_ip(IPAddress ip);
			void start();
			void loop();				
		};

		class udpServer  {
			public:
				IPAddress 		_ip;
				boolean 		_isConnected;
				int 			_port;
				unsigned long 	_lastCall;

				unsigned long 	_waiting;

				unsigned long 	_checkInterval;
				// int 			_checkCount;
				boolean 		_check;

				boolean 		_checkStart;
				// unsigned long 	_checkStart_last;

				udpServer(IPAddress ip, uint16_t port);  
				void compare();
				String dump();

		}  ;	
		void udpServer_checkClient();	
#endif