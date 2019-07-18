#ifndef _menu_
#define _menu_
#include "MenuManager.h"
#include <avr/pgmspace.h>

/*
Generated using LCD Menu Builder at http://lcd-menu-bulder.cohesivecomputing.co.uk/.
*/

enum menuCommandId
{
  mnuCmdBack = 0,
  mnuCmdTemp,
  mnuCmdHumidity,
  mnuCmdDewPoint
};

PROGMEM const char menu_back[] = "Back";
PROGMEM const char menu_exit[] = "Exit";

PROGMEM const char menu_1[] = "Temp";
PROGMEM const char menu_2[] = "Humidity";
PROGMEM const char menu_3[] = "Dew Point";
PROGMEM const MenuItem menu_Root[] = {{mnuCmdTemp, menu_1}, {mnuCmdHumidity, menu_2}, {mnuCmdDewPoint, menu_3}, {mnuCmdBack, menu_exit}};

/*
case mnuCmdTemp :
	break;
case mnuCmdHumidity :
	break;
case mnuCmdPh :
	break;
*/

/*
<?xml version="1.0"?>
<RootMenu xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
  <Config IdPrefix="mnuCmd" VarPrefix="menu" UseNumbering="false" IncludeNumberHierarchy="false"
          MaxNameLen="15" MenuBackFirstItem="false" BackText="Back" ExitText="Exit" AvrProgMem="true"/>
  <MenuItems>
    <Item Id="Temp" Name="Temp"/>
    <Item Id="Humidity" Name="Humidity"/>
    <Item Id="DewPoint" Name="Dew Point"/>
  </MenuItems>
</RootMenu>

*/
#endif
