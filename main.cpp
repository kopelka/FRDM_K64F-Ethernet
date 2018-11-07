#include "mbed.h"
#include "EthernetInterface.h"
 
#define MBED_DEV_IP       "192.168.0.52"
#define MBED_DEV_MASK     "255.255.255.0"
#define MBED_DEV_GW       "0.0.0.0"
#define ECHO_SERVER_PORT   5000

 
int main (void) {
    EthernetInterface eth;
    eth.init(MBED_DEV_IP, MBED_DEV_MASK, MBED_DEV_GW); //Assign a device ip, mask and gateway
    eth.connect();
    printf("IP Address is %s\n", eth.getIPAddress());
    
    TCPSocketServer server;
    server.bind(ECHO_SERVER_PORT);
    server.listen();
		DigitalOut myled(LED1);
    
    while (true) {
			//blink to test if its working
        myled = 1;
        wait(0.2);
        myled = 0;
        wait(0.2);
			
        printf("\nWait for new connection...\n");
        TCPSocketConnection client;
        server.accept(client);
        client.set_blocking(false, 1500); // Timeout after (1.5)s
        
        printf("Connection from: %s\n", client.get_address());
        char buffer[256];
        while (true) {
            int n = client.receive(buffer, sizeof(buffer));
            if (n <= 0) break;
            
            client.send_all(buffer, n);
            if (n <= 0) break;
        }
        
        client.close();
    }
}