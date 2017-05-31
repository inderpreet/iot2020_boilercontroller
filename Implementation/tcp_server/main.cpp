/*
 * Program for TCP Socket Server for controlling TLV/Modbus devices.
 * Connect with a TCP SOcket and send Lamp ONN or Lamp OFF to see the Serial Commands transmitted.
 *
 * By: Inderpreet Singh
 *
 *
 */

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
using namespace std;
#include "mraa.hpp"
#include <signal.h>

using namespace mraa;



volatile sig_atomic_t flag = 0; //for CTRL+C
void my_handler(int sig){ // can be called asynchronously
	printf("Caught signal %d\n",sig);
	flag = 1; // set flag
}


int main(void) {
	cout<<"Starting TCP Socket Server"<<endl;
	// Server Stuff
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
//	char *hello = "Hello from server";

	//UART Stuff  here
	mraa::Uart* dev;
	try {
		dev = new mraa::Uart("/dev/ttyUSB0");
	} catch (std::exception& e) {
		std::cout << "Error while setting up raw UART, do you have a uart?" << std::endl;
		std::terminate();
	}
	if (dev->setBaudRate(9600) != mraa::SUCCESS) {
		std::cout << "Error setting parity on UART" << std::endl;
	}
	if (dev->setMode(8, mraa::UART_PARITY_NONE, 1) != mraa::SUCCESS) {
		std::cout << "Error setting parity on UART" << std::endl;
	}
	if (dev->setFlowcontrol(false, false) != mraa::SUCCESS) {
		std::cout << "Error setting flow control UART" << std::endl;
	}

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
			&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( 8081 );

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr *)&address,
			sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
//	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
//			(socklen_t*)&addrlen))<0)
//	{
//		perror("accept");
//		exit(EXIT_FAILURE);
//	}

	while(flag==0) {
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
				(socklen_t*)&addrlen))<0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}
		valread = read( new_socket , buffer, 1024);
		printf("%s",buffer );
		if(!strncmp(buffer,"Lamp ONN\n", 8)){
			dev->writeStr(":L01");
			cout<<"lamp on"<<endl;
		}else if(!strncmp(buffer,"Lamp OFF\n", 8)){
			dev->writeStr(":L00");
			cout<<"lamp off"<<endl;
		}else if(!strncmp(buffer,"Pump ONN\n", 8)){
			dev->writeStr(":P01");
		}else if(!strncmp(buffer,"Pump OFF\n", 8)){
			dev->writeStr(":P00");
		}
		strcpy(buffer,"        \n");
		close(new_socket);
		//Use below lines to reply on UART
		//send(new_socket , hello , strlen(hello) , 0 );
		//printf("Hello message sent\n");
	}
	delete dev; // casuses reset to the FTDI chip and arduinos as well.
	return 0;
}

