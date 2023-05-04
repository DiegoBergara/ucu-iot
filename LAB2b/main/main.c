#include <stdio.h>
#include "sta-AP.h"
#include "server.h"

void app_main(void)
{
    //1 para STA o 2 para AP 
    setMode(2);
    server_init();
}