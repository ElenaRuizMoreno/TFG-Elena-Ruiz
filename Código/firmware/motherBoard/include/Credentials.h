#ifndef _CREDENTIALS_
#define _CREDENTIALS_

#define THINGSBOARD_SERVER "mon.medicalopenworld.org"
#define THINGSBOARD_PORT 1883

#define FACTORY_SERVER 0
#define DEMO_SERVER 1

#define THINGSBOARD_PROVISION_SERVER FACTORY_SERVER

#if(THINGSBOARD_PROVISION_SERVER == DEMO_SERVER)
#define PROVISION_DEVICE_KEY "bztump0738iuc2ggreix"
#define PROVISION_DEVICE_SECRET "0znp47gkyh9hbljq1opm"
#elif (THINGSBOARD_PROVISION_SERVER == FACTORY_SERVER)
#define PROVISION_DEVICE_KEY "0i2tui2pi1pd36l09bnt"
#define PROVISION_DEVICE_SECRET "ttzau2156pstkt43w822"
#endif

#define ssid "in3wifi"
#define wifiPassword "12345678"
#endif // _CREDENTIALS_
