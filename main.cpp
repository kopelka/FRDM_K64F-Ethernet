#include "mbed.h"
#include "EthernetInterface.h"
#include "fsl_phy_driver.h"
#include "k64f_emac_config.h"
//#include "fsl_enet_driver.h"
//#include "fsl_enet_hal.h"
 
#define MBED_DEV_IP       "192.168.0.52"
#define MBED_DEV_MASK     "255.255.255.0"
#define MBED_DEV_GW       "0.0.0.0"
#define ECHO_SERVER_PORT   5000
Serial s(USBTX, USBRX);
//enet_dev_if_t * enetIfPtr;
volatile char   c = '\0'; // Initialized to the NULL character
 uint32_t data;
void onCharReceived()
{
    c = s.getc();
}
void baud(int baudrate) {
    
    s.baud(baudrate);
}


/*! @brief Defines the PHY link speed. */
//typedef enum _enet_phy_speed
//{
//    kEnetSpeed10M = 0,   /*!< ENET PHY 10 M speed*/
//    kEnetSpeed100M = 1  /*!< ENET PHY 100 M speed*/
//} enet_phy_speed_t;

 uint32_t mii_read_data(void)
{
	//uint32_t instance, uint32_t phyAddr, uint32_t phyReg, uint32_t *dataPtr
    return enet_mii_read(BOARD_DEBUG_ENET_INSTANCE,0,kEnetPhyCt2, &data);
}
//
//bool get_link_status(void)
//{
//    return (lpc_mii_read_data() & DP8_VALID_LINK) ? true : false;
//}
// 
//int get_connection_speed()
//{
//    return (lpc_mii_read_data() & DP8_SPEED10MBPS) ? 10 : 100;
//}
 
int main (void) {
	enet_phy_speed_t status;
		baud(115200);
	printf ("Hello World! Enter task number:\n");
	onCharReceived();
	switch(c){
		case '1': 
			printf("LEts find out about RJ-45 socket diodes");
			break;
		case '2': break;
		default: break;
	}       

		
    EthernetInterface eth;
	//eth.Mode();
    eth.init(MBED_DEV_IP, MBED_DEV_MASK, MBED_DEV_GW); //Assign a device ip, mask and gateway
    eth.connect();
    printf("IP Address is %s\n", eth.getIPAddress());

    TCPSocketServer server;
    server.bind(ECHO_SERVER_PORT);
    server.listen();
		DigitalOut myled(LED1);
    
    while (true) {
			
        printf("\nWait for new connection...\n");
        TCPSocketConnection client;
        server.accept(client);
        client.set_blocking(false, 3000); // Timeout after (1.5)s
        
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