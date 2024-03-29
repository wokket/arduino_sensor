#include <LiquidCrystal.h>
#include "dht11.h"
#include "LcdKeypad.h"
#include "MenuData.h"
#include <TimerOne.h>

enum AppModeValues
{
  APP_NORMAL_MODE,
  APP_MENU_MODE,
  APP_PROCESS_MENU_CMD
};

byte appMode = APP_NORMAL_MODE;

MenuManager Menu1(menu_Root, menuCount(menu_Root));

char strbuf[LCD_COLS + 1]; // one line of lcd display
byte btn;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void refreshMenuDisplay(byte refreshMode);
byte getNavAction();

dht11 DHT11;

void setup()
{
  Serial.begin(9600);

  DHT11.attach(2);

  backLightOn();
  // set up the LCD's number of columns and rows:
  lcd.begin(LCD_COLS, LCD_ROWS);

  // fall in to menu mode by default.
  appMode = APP_MENU_MODE;
  refreshMenuDisplay(REFRESH_DESCEND);

  // Use soft PWM for backlight, as hardware PWM must be avoided for some LCD shields.
  Timer1.initialize();
  Timer1.attachInterrupt(lcdBacklightISR, 500);
  setBacklightBrightness(1);
}

void loop()
{
  /* int chk = DHT11.read();
 
  switch (chk)
  {
  case 0:
    Serial.println("DHT11 OK");
    Serial.print("Humidity (%): ");
    Serial.println((float)DHT11.humidity, DEC);

    Serial.print("Temperature (°C): ");
    Serial.println((float)DHT11.temperature, DEC);
    break;
  case -1:
    Serial.println("DHT11 Checksum error");
    break;
  case -2:
    //Serial.println("DHT11 Time out error");
    break;
  default:
    Serial.println("DHT11 Unknown error");
    break;
  } 
*/

  btn = getButton();

  switch (appMode)
  {
  case APP_NORMAL_MODE:
    if (btn == BUTTON_UP_LONG_PRESSED)
    {
      appMode = APP_MENU_MODE;
      refreshMenuDisplay(REFRESH_DESCEND);
    }
    break;

  case APP_MENU_MODE:
  {
    byte menuMode = Menu1.handleNavigation(getNavAction, refreshMenuDisplay);

    if (menuMode == MENU_EXIT)
    {
      lcd.clear();
      lcd.print("Hold UP for menu");
      appMode = APP_NORMAL_MODE;
    }
    else if (menuMode == MENU_INVOKE_ITEM)
    {
      appMode = APP_PROCESS_MENU_CMD;

      // Indicate selected item.
      if (Menu1.getCurrentItemCmdId())
      {
        lcd.setCursor(0, 1);
        strbuf[0] = 0b01111110; // forward arrow representing input prompt.
        strbuf[1] = 0;
        lcd.print(strbuf);
      }
    }
    break;
  }

  case APP_PROCESS_MENU_CMD:
  {
    byte processingComplete = processMenuCommand(Menu1.getCurrentItemCmdId());

    if (processingComplete)
    {
      appMode = APP_MENU_MODE;
      // clear forward arrow and old readings
      lcd.setCursor(0, 1);
      for (int i = 0; i < 15; i++)
        lcd.print(" ");
    }
    break;
  }
  }
}

//----------------------------------------------------------------------
// Addition or removal of menu items in MenuData.h will require this method
// to be modified accordingly.
byte processMenuCommand(byte cmdId)
{
  byte complete = false; // set to true when menu command processing complete.

  if (btn == BUTTON_SELECT_PRESSED)
  {
    complete = true;
  }

  switch (cmdId)
  {
  case menuCommandId::mnuCmdTemp:
  {
    double temp = DHT11.getTemp();

    lcd.setCursor(2, 1);
    lcd.print(temp, 1);
    lcd.print("C     ");

    // Serial.print("Temperature (°C): ");
    // Serial.println(temp, 1);

    break;
  }
  case menuCommandId::mnuCmdHumidity:
    lcd.setCursor(2, 1);
    lcd.print((byte)DHT11.getHumidity());
    lcd.print(" %    ");
    break;

  case menuCommandId::mnuCmdDewPoint:
    lcd.setCursor(2, 1);
    lcd.print((byte)DHT11.getDewPoint());
    lcd.print(" C     ");
    break;

  default:
    break;
  }

  return complete;
}

//----------------------------------------------------------------------
// Callback to convert button press to navigation action.
byte getNavAction()
{
  byte navAction = 0;
  byte currentItemHasChildren = Menu1.currentItemHasChildren();

  if (btn == BUTTON_UP_PRESSED || btn == BUTTON_UP_LONG_PRESSED)
  {
    navAction = MENU_ITEM_PREV;
  }
  else if (btn == BUTTON_DOWN_PRESSED || btn == BUTTON_DOWN_LONG_PRESSED)
  {
    navAction = MENU_ITEM_NEXT;
  }
  else if (btn == BUTTON_SELECT_PRESSED || (btn == BUTTON_RIGHT_PRESSED && currentItemHasChildren))
  {
    navAction = MENU_ITEM_SELECT;
  }
  //else if (btn == BUTTON_LEFT_PRESSED) navAction = MENU_BACK;
  return navAction;
}

//----------------------------------------------------------------------
const char EmptyStr[] = "";

// Callback to refresh display during menu navigation, using parameter of type enum DisplayRefreshMode.
void refreshMenuDisplay(byte refreshMode)
{
  char nameBuf[LCD_COLS + 1];

  /*
  if (refreshMode == REFRESH_DESCEND || refreshMode == REFRESH_ASCEND)
  {
    byte menuCount = Menu1.getMenuItemCount();
    
    // uncomment below code to output menus to serial monitor
    if (Menu1.currentMenuHasParent())
    {
      Serial.print("Parent menu: ");
      Serial.println(Menu1.getParentItemName(nameBuf));
    }
    else
    {
      Serial.println("Main menu:");
    }
    
    for (int i=0; i<menuCount; i++)
    {
      Serial.print(Menu1.getItemName(nameBuf, i));

      if (Menu1.itemHasChildren(i))
      {
        Serial.println("->");
      }
      else
      {
        Serial.println();
      }
    }
  }
*/

  lcd.setCursor(0, 0);
  if (Menu1.currentItemHasChildren())
  {
    rpad(strbuf, Menu1.getCurrentItemName(nameBuf));
    strbuf[LCD_COLS - 1] = 0b01111110; // Display forward arrow if this menu item has children.
    lcd.print(strbuf);
    lcd.setCursor(0, 1);
    lcd.print(rpad(strbuf, EmptyStr)); // Clear config value in display
  }
  else
  {
    byte cmdId;
    rpad(strbuf, Menu1.getCurrentItemName(nameBuf));

    if ((cmdId = Menu1.getCurrentItemCmdId()) == 0)
    {
      strbuf[LCD_COLS - 1] = 0b01111111; // Display back arrow if this menu item ascends to parent.
      lcd.print(strbuf);
      lcd.setCursor(0, 1);
      lcd.print(rpad(strbuf, EmptyStr)); // Clear config value in display.
    }
    else
    {
      lcd.print(strbuf);
      lcd.setCursor(0, 1);
      lcd.print(" ");
      // TODO Display config value.
    }
  }
}
