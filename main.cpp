#include "mbed.h"
#include "EthernetInterface.h"
//#include "fsl_phy_driver.h"
//#include "k64f_emac_config.h"
#include "fsl_enet_driver.h"
#include "fsl_enet_hal.h"
 
#define MBED_DEV_IP       "192.168.0.52"
#define MBED_DEV_MASK     "255.255.255.0"
#define MBED_DEV_GW       "0.0.0.0"
#define ECHO_SERVER_PORT   5000


Serial s(USBTX, USBRX);
volatile char   c = '\0'; // Initialized to the NULL character
 uint32_t data;
 
/*! @brief Defines the PHY register.*/
typedef enum _enet_phy_register
{
    kEnetPhyCR = 0, /*!< PHY control register */
    kEnetPhySR = 1, /*!< PHY status register*/
    kEnetPhyId1 = 2, /*!< PHY identification register 1*/
    kEnetPhyId2 = 3, /*!< PHY identification register 2*/
    kEnetPhyCt2 = 0x1e /*!< PHY control2 register*/
} enet_phy_register_t;
 /*! @brief Defines the PHY status.*/
typedef enum _enet_phy_status
{
    kEnetPhyLinkStatus = 0x4,  /*!< ENET PHY link status bit*/
    kEnetPhyAutoNegAble = 0x08, /*!< ENET PHY auto negotiation ability*/
    kEnetPhyAutoNegComplete = 0x20, /*!< ENET PHY auto negotiation complete*/
    kEnetPhySpeedDulpexMask = 0x07 /*!< ENET PHY speed mask on status register 2*/
} enet_phy_status_t;
/*! @brief Defines the control flag.*/
typedef enum _enet_phy_control
{
    kEnetPhyAutoNeg = 0x1000,/*!< ENET PHY auto negotiation control*/
    kEnetPhySpeed = 0x2000, /*! ENET PHY speed control*/
    kEnetPhyLoop = 0x4000, /*!< ENET PHY loop control*/
    kEnetPhyReset = 0x8000, /*!< ENET PHY reset control*/
    kEnetPhy10HalfDuplex = 0x01, /*!< ENET PHY 10 M half duplex*/
    kEnetPhy100HalfDuplex = 0x02,/*!< ENET PHY 100 M half duplex*/
    kEnetPhy10FullDuplex = 0x05,/*!< ENET PHY 10 M full duplex*/
    kEnetPhy100FullDuplex = 0x06/*!< ENET PHY 100 M full duplex*/
} enet_phy_control_t;

void onCharReceived()
{
    c = s.getc();
}
void baud(int baudrate) {
    
    s.baud(baudrate);
}

 uint32_t mii_read_data(void)
{
	//uint32_t instance, uint32_t phyAddr, uint32_t phyReg, uint32_t *dataPtr
    return enet_mii_read(0,0,kEnetPhyCt2, &data);
}
//
//bool get_link_status(void)
//{
//    return (lpc_mii_read_data() & DP8_VALID_LINK) ? true : false;
//}
// 
int get_connection_speed()
{
	printf("DATA %d",data);	
	mii_read_data();
	printf("DATA %d",data);	
	data &=kEnetPhySpeedDulpexMask;
	printf("DATA %d",data);	
    return (data & kEnetPhy10FullDuplex) ? 10  : 100 ;
}
 
int main (void) {
int speed;
		baud(115200);
	printf ("Hello World! Enter task number:\n");
	//onCharReceived();
	//switch(c){
	//	case '1': 
	//		printf("LEts find out about RJ-45 socket diodes");
	//		break;
	//	case '2': break;
	//	default: break;
	//}       

		
    EthernetInterface eth;
		printf("Initializing interface...\r\n");
    eth.init(MBED_DEV_IP, MBED_DEV_MASK, MBED_DEV_GW); //Assign a device ip, mask and gateway
	
		printf("Connecting to network...\r\n");
   if (0 == eth.connect()) { 
		 speed = get_connection_speed();
			printf("Connected at %d Mb/s\r\n", speed);
    //printf("IP Address is %s\n", eth.getIPAddress());
	 }

	
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

