/**
 * Arduino Cookie Clicker
 * Lucas Bubner, 2023
 */

#include <LiquidCrystal.h>

// Key value constants
#define NONE 1023
#define SELECT 640
#define LEFT 411
#define DOWN 257
#define UP 101
#define RIGHT 0
#define TOLERANCE 10

#define IN_GAME 0
#define IN_MENU 1
#define MAX_MENU_ITEMS 5 // + 1
#define PRICE_RATE 1.9
#define DOUBLE_CLICK_ADDITIONAL_RATE 1.1

#define CLICKER 1
#define GRANDMA 5
#define SILO 10
#define DOUBLE_CLICK 2
#define FACTORY 50
#define NUKE 250

// Get a key value with a tolerance of plus-minus 10 units
int getKey(int val)
{
    if (val >= NONE - TOLERANCE && val <= NONE + TOLERANCE)
        return NONE;
    if (val >= SELECT - TOLERANCE && val <= SELECT + TOLERANCE)
        return SELECT;
    if (val >= LEFT - TOLERANCE && val <= LEFT + TOLERANCE)
        return LEFT;
    if (val >= DOWN - TOLERANCE && val <= DOWN + TOLERANCE)
        return DOWN;
    if (val >= UP - TOLERANCE && val <= UP + TOLERANCE)
        return UP;
    if (val >= RIGHT - TOLERANCE && val <= RIGHT + TOLERANCE)
        return RIGHT;
    return NONE;
}

// LCD setup
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Global variables
unsigned long cookies;

int key;
int gameState;
int menuItem;

// Upgrades
unsigned long autoClick = 0;
unsigned long multiplier = 1;

// Pricing
unsigned long clicker = 10;
unsigned long grandma = 50;
unsigned long silo = 250;
unsigned long doubleClick = 300;
unsigned long factory = 1000;
unsigned long nuke = 10000;

// Propagate first-row updates to the LCD
void dispatchFirstRowUpdate()
{
    if (gameState == IN_GAME)
    {
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print("Cookies: " + (String)cookies);
    }
    else
    {
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print("Shop (" + (String)cookies + ")");
    }
}

// Propagate second-row updates to the LCD
void dispatchSecondRowUpdate()
{
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    if (gameState == IN_GAME)
    {
        if (autoClick > 0)
        {
            lcd.print("A-CPS:" + (String)autoClick + " C:x" + (String)multiplier);
        }
        else if (cookies >= 10)
        {
            lcd.print("LEFT: Open shop");
        }
        else
        {
            lcd.print("SELECT to earn!");
        }
        if (key == SELECT)
        {
            // Symbol at the end of the second line to indicate a keypress
            lcd.setCursor(15, 1);
            lcd.print("*");
        }
    }
    else
    {
        switch (menuItem)
        {
        case 0:
            lcd.print("Clicker: " + (String)clicker);
            break;
        case 1:
            lcd.print("Grandma: " + (String)grandma);
            break;
        case 2:
            lcd.print("Silo: " + (String)silo);
            break;
        case 3:
            lcd.print("x2 Clk: " + (String)doubleClick);
            break;
        case 4:
            lcd.print("Factory: " + (String)factory);
            break;
        case 5:
            lcd.print("Nuke: " + (String)nuke);
            break;
        }
    }
}

void addAutoCookies()
{
    // Only once per second
    static unsigned long lastAutoClickUpdate = 0;
    if (millis() - lastAutoClickUpdate < 1000)
    {
        return;
    }
    lastAutoClickUpdate = millis();
    cookies += autoClick;
}

void setup()
{
    gameState = IN_GAME;
    lcd.begin(20, 2);
    lcd.print("Cookie Clicker");
    lcd.setCursor(0, 1);
    lcd.print("Press SELECT");
    while (getKey(analogRead(0)) != SELECT)
    {
        // Wait for user to press SELECT
    }
    // Do not register the SELECT key press as a cookie
    cookies--;
}

void loop()
{
    static int keyLatch = NONE;

    key = getKey(analogRead(0));
    if (key != keyLatch)
    {
        // Click cookie
        if (key == SELECT && gameState == IN_GAME)
        {
            cookies += multiplier;
        }
        // Buy an upgrade
        if (key == SELECT && gameState == IN_MENU)
        {
            switch (menuItem)
            {
            case 0:
                if (cookies >= clicker)
                {
                    cookies -= clicker;
                    autoClick++;
                    clicker *= PRICE_RATE;
                }
                break;
            case 1:
                if (cookies >= grandma)
                {
                    cookies -= grandma;
                    autoClick += GRANDMA;
                    grandma *= PRICE_RATE;
                }
                break;
            case 2:
                if (cookies >= silo)
                {
                    cookies -= silo;
                    autoClick += SILO;
                    silo *= PRICE_RATE;
                }
                break;
            case 3:
                if (cookies >= doubleClick)
                {
                    cookies -= doubleClick;
                    multiplier *= DOUBLE_CLICK;
                    // Slightly increase the price of the next upgrade compared to the others
                    doubleClick *= PRICE_RATE * DOUBLE_CLICK_ADDITIONAL_RATE;
                }
                break;
            case 4:
                if (cookies >= factory)
                {
                    cookies -= factory;
                    autoClick += FACTORY;
                    factory *= PRICE_RATE;
                }
                break;
            case 5:
                if (cookies >= nuke)
                {
                    cookies -= nuke;
                    autoClick += NUKE;
                    nuke *= PRICE_RATE;
                }
                break;
            }
        }
        // Exit menu
        if (key == LEFT && gameState == IN_MENU)
        {
            gameState = IN_GAME;
        }
        else if (key == LEFT)
        {
            gameState = IN_MENU;
            menuItem = 0;
        }
        // Navigate menu up
        if (key == UP && gameState == IN_MENU)
        {
            if (menuItem == 0)
            {
                menuItem = MAX_MENU_ITEMS;
            }
            else
            {
                menuItem--;
            }
        }
        // Navigate menu down
        if (key == DOWN && gameState == IN_MENU)
        {
            if (menuItem == MAX_MENU_ITEMS)
            {
                menuItem = 0;
            }
            else
            {
                menuItem++;
            }
        }
        keyLatch = key;
    }

    addAutoCookies();
    dispatchFirstRowUpdate();
    dispatchSecondRowUpdate();
    delay(100);
}