#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HID-Project.h>
#include <Fonts/Org_01.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int mode = 0;
boolean play = false;
int lastButtonState = HIGH;
enum Screen { SCREEN_MAIN, SCREEN_PONG };
Screen displayScreen = SCREEN_MAIN;
boolean isPlaying = false;

int pinA = 9;
int pinB = 8;
int pinC = 7;
int pinD = 6;
int pinE = 5;
int pinF = 4;

// Game settings (editable)
int paddleHeight = 10; // Height of the paddles
int paddleWidth = 3;   // Width of the paddles
int paddleSpeed = 2;   // Speed at which paddles move
int ballSize = 3;      // Size of the ball
float ballSpeedX = 1;  // Horizontal speed of the ball
float ballSpeedY = 0.5; // Vertical speed of the ball
int cpuDifficulty = 1; // 1 = hardest, higher is easier (CPU reacts slower)

// Game state enum to represent different modes
enum Mode { MENU, GAME };
Mode gameMode = MENU; // Default game mode is MENU

// Paddle and ball positions
int p1Y = SCREEN_HEIGHT / 2 - paddleHeight / 2;
int p2Y = SCREEN_HEIGHT / 2 - paddleHeight / 2;
float ballX = SCREEN_WIDTH / 2;
float ballY = SCREEN_HEIGHT / 2;
float vx = ballSpeedX;
float vy = ballSpeedY;

//Logo
static const unsigned char PROGMEM image_network_www_bits[] = {0x03,0xc0,0x0d,0xb0,0x32,0x4c,0x24,0x24,0x44,0x22,0x7f,0xfe,0x88,0x11,0x88,0x11,0x88,0x11,0x88,0x11,0x7f,0xfe,0x44,0x22,0x24,0x24,0x32,0x4c,0x0d,0xb0,0x03,0xc0};

//Mode 0 (Main Screen)
static const unsigned char PROGMEM image_Button_bits[] = {0x1f,0xff,0x00,0x60,0x00,0x80,0xc0,0x00,0x40,0xc0,0x00,0x40,0xc0,0x00,0x40,0xc0,0x00,0x40,0xc0,0x00,0x40,0xc0,0x00,0x40,0xc0,0x00,0x40,0xc0,0x00,0x40,0xc0,0x00,0x40,0xc0,0x00,0x40,0xc0,0x00,0x40,0xc0,0x00,0x40,0xc0,0x00,0x40,0xe0,0x00,0xc0,0x7f,0xff,0x80,0x3f,0xff,0x00};

static const unsigned char PROGMEM image_FaceNormal_bits[] = {0x00,0x00,0x00,0x00,0x3c,0x00,0x01,0xe0,0x7a,0x00,0x03,0xd0,0x7e,0x00,0x03,0xf0,0x7e,0x00,0x03,0xf0,0x7e,0x00,0x03,0xf0,0x3c,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x40,0x00,0x00,0x10,0x40,0x00,0x00,0x10,0x40,0x00,0x00,0x08,0x80,0x00,0x00,0x07,0x00,0x00};

static const unsigned char PROGMEM image_Attention_bits[] = {0x70,0x50,0x50,0x50,0x70,0x20,0x00,0x70};

//Mode 1
static const unsigned char PROGMEM image_Layer_11_bits[] = {0x80,0x00,0x40,0x00,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0x00,0x80,0x00,0x40};

static const unsigned char PROGMEM image_Layer_14_bits[] = {0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0};

static const unsigned char PROGMEM image_Layer_17_bits[] = {0x66,0x32,0x40,0x89,0x4a,0x40,0x89,0x49,0xc0,0x89,0x70,0x40,0x66,0x41,0x80};

static const unsigned char PROGMEM image_Layer_17_copy_bits[] = {0x73,0x31,0x99,0x80,0x84,0x4a,0x22,0x40,0xe4,0x4b,0xba,0x40,0x14,0x72,0x22,0x40,0xe3,0x4b,0xba,0x40};

static const unsigned char PROGMEM image_Layer_18_bits[] = {0x63,0x1d,0xf3,0x94,0xa0,0x44,0x94,0xb8,0x47,0xe7,0x84,0x44,0x84,0xb8,0x47};

static const unsigned char PROGMEM image_Layer_20_bits[] = {0x89,0xce,0x8a,0x10,0x8b,0x90,0x88,0x50,0x50,0x50,0x23,0x8e};

static const unsigned char PROGMEM image_Layer_21_bits[] = {0x71,0x8c,0xc0,0x8a,0x51,0x20,0x8a,0x5d,0x20,0x8b,0x91,0x20,0x72,0x1d,0x20};

static const unsigned char PROGMEM image_menu_settings_sliders_two_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x44,0x00,0xc7,0xfc,0x44,0x00,0x38,0x00,0x00,0x70,0x00,0x88,0xff,0x8c,0x00,0x88,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00};

//Mode 2
static const unsigned char PROGMEM image_Layer_112_bits[] = {0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60};

static const unsigned char PROGMEM image_Layer_142_bits[] = {0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0,0xff,0xf7,0xf0,0xff,0xff,0xf0,0xff,0xff,0xf0};

static const unsigned char PROGMEM image_Layer_162_bits[] = {0x80,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x40};

static const unsigned char PROGMEM image_Layer_182_bits[] = {0x07,0x00,0x09,0x00,0x11,0x00,0xe1,0x40,0x81,0x20,0x81,0x20,0x81,0x20,0xe1,0x40,0x11,0x00,0x09,0x00,0x07,0x00};

static const unsigned char PROGMEM image_Layer_182_copy_bits[] = {0x07,0x00,0x09,0x20,0x11,0x10,0xe1,0x48,0x81,0x24,0x81,0x24,0x81,0x24,0xe1,0x48,0x11,0x10,0x09,0x20,0x07,0x00};

static const unsigned char PROGMEM image_Layer_202_bits[] = {0xc0,0xa0,0x90,0x88,0x84,0x88,0x90,0xa0,0xc0};

static const unsigned char PROGMEM image_Layer_222_bits[] = {0x98,0xa0,0x60,0x10,0x10,0x10};

static const unsigned char PROGMEM image_Layer_232_bits[] = {0x0e,0x00,0x11,0x00,0x20,0x80,0x40,0x40,0xbd,0xa0,0x40,0x40,0x44,0x40,0x4a,0x40,0x4a,0x40,0x4a,0x40};

static const unsigned char PROGMEM image_menu_settings_sliders_two_bits_flip[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x00,0x88,0xff,0x8c,0x00,0x88,0x00,0x70,0x38,0x00,0x44,0x00,0xc7,0xfc,0x44,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void displayMode() {
  if (mode == 0) {
    if (displayScreen == SCREEN_MAIN) {
      display.clearDisplay();
      display.drawLine(0, 0, 127, 0, 1);

      display.drawLine(127, 1, 127, 31, 1);

      display.drawLine(126, 31, 0, 31, 1);

      display.drawLine(0, 0, 0, 31, 1);

      display.drawBitmap(7, 9, image_FaceNormal_bits, 29, 14, 1);

      display.drawLine(43, 1, 43, 31, 1);

      display.setTextSize(1);
      display.setTextColor(1);
      display.setTextWrap(false);
      display.setCursor(1, 23);
      display.print("");

      display.setCursor(46, 3);
      display.print("Welcome");

      display.setCursor(46, 12);
      display.print("Home");

      display.setCursor(46, 21);
      display.print("Jesstie!");

      display.drawBitmap(105, 7, image_Button_bits, 18, 18, 1);

      display.drawBitmap(112, 12, image_Attention_bits, 5, 8, 1);

      display.display();
      display.setTextColor(SSD1306_WHITE);
      display.setFont(); // kembali ke font default (penting!)
    } else {
      showMenu();
    }
    
  }else if (mode == 1) {
    //Display Mode 1
    display.clearDisplay();
    display.drawLine(0, 0, 127, 0, 1);

    display.drawLine(127, 1, 127, 31, 1);

    display.drawLine(126, 31, 0, 31, 1);

    display.drawLine(0, 0, 0, 31, 1);

    display.setTextColor(1);
    display.setTextWrap(false);
    display.setCursor(1, 23);
    display.print("");

    display.drawLine(22, 0, 22, 31, 1);

    display.drawLine(21, 1, 21, 31, 1);

    display.drawLine(0, 16, 127, 16, 1);

    display.setTextSize(2);
    display.setFont(&Org_01);
    display.setCursor(6, 27);
    display.print("2");

    display.drawLine(57, -1, 57, 30, 1);

    display.drawBitmap(1, 1, image_Layer_14_bits, 20, 15, 1);

    display.setTextColor(0);
    display.setCursor(10, 11);
    display.print("1");

    display.drawBitmap(2, 2, image_Layer_11_bits, 18, 13, 0);

    display.drawLine(92, 1, 92, 32, 1);

    display.drawRect(1, 1, 20, 15, 0);

    display.drawBitmap(66, 6, image_Layer_17_bits, 18, 5, 1);

    display.drawBitmap(33, 1, image_menu_settings_sliders_two_bits, 14, 16, 1);

    display.drawBitmap(98, 6, image_Layer_18_bits, 24, 5, 1);

    display.drawBitmap(27, 22, image_Layer_17_copy_bits, 26, 5, 1);

    display.drawBitmap(68, 21, image_Layer_20_bits, 15, 6, 1);

    display.drawBitmap(100, 22, image_Layer_21_bits, 19, 5, 1);

    display.display();
    display.setTextColor(SSD1306_WHITE);
    display.setFont(); // kembali ke font default (penting!)
  } else if (mode == 2) {
    //Display Mode 2
    display.clearDisplay();
    display.drawLine(0, 0, 127, 0, 1);

    display.drawLine(127, 1, 127, 31, 1);

    display.drawLine(126, 31, 0, 31, 1);

    display.drawLine(0, 0, 0, 31, 1);

    display.setTextColor(1);
    display.setTextWrap(false);
    display.setCursor(1, 23);
    display.print("");

    display.drawLine(22, 0, 22, 31, 1);

    display.drawLine(21, 1, 21, 31, 1);

    display.drawLine(0, 16, 127, 16, 1);

    display.drawBitmap(1, 17, image_Layer_142_bits, 20, 14, 1);

    display.setTextColor(0);
    display.setTextSize(2);
    display.setFont(&Org_01);
    display.setCursor(6, 27);
    display.print("2");

    display.drawLine(57, -1, 57, 30, 1);

    display.setTextColor(1);
    display.setCursor(10, 11);
    display.print("1");

    display.drawBitmap(9, 4, image_Layer_112_bits, 3, 10, 1);

    display.drawLine(92, 1, 92, 32, 1);

    display.drawRect(1, 17, 20, 14, 0);

    display.drawBitmap(2, 18, image_Layer_162_bits, 18, 12, 0);

    display.drawBitmap(33, 1, image_menu_settings_sliders_two_bits_flip, 14, 16, 1);

    display.drawBitmap(69, 3, image_Layer_182_bits, 11, 11, 1);

    display.drawBitmap(103, 3, image_Layer_182_copy_bits, 14, 11, 1);

    display.drawBitmap(37, 20, image_Layer_202_bits, 6, 9, 1);

    display.drawCircle(74, 24, 4, 1);

    display.drawBitmap(72, 22, image_Layer_222_bits, 5, 6, 1);

    display.drawBitmap(104, 19, image_Layer_232_bits, 11, 10, 1);

    display.display();
    display.setTextColor(SSD1306_WHITE);
    display.setFont(); // kembali ke font default (penting!)
  }
}

void setup() {
  Wire.begin();
  Consumer.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  // display.setCursor(0, 15);
  // display.println("MACROPAD by Jesstie.");
  
  //Display macroPAD
  display.setTextColor(1);
  display.setTextSize(2);
  display.setTextWrap(false);
  display.setCursor(11, 9);
  display.print("MACR  PAD");

  display.drawBitmap(59, 8, image_network_www_bits, 16, 16, 1);

  display.display();

  delay(3000);

  displayMode();

  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  pinMode(pinC, INPUT_PULLUP);
  pinMode(pinD, INPUT_PULLUP);
  pinMode(pinE, INPUT_PULLUP);
  pinMode(pinF, INPUT_PULLUP);
}

void loop() {

  // Check if game is in the menu or in play mode
  if (mode == 0 && displayScreen == SCREEN_PONG) {
    if (gameMode == MENU) {
      showMenu();
      isPlaying = false;
    } else {
      playGame();
      isPlaying = true;
    }
  }

  int buttonState = digitalRead(pinA);

  // Change Mode
  if (lastButtonState == HIGH && buttonState == LOW && !isPlaying) {
    if (mode == 0 && displayScreen == SCREEN_MAIN) {
      mode = 1;
    } else if (mode == 1) {
      mode = 2;
    } else if (mode == 2) {
      mode = 1;
    }
    delay(100);
    displayMode();
    delay(100);
  }

  if (digitalRead(pinB) == LOW && !isPlaying) {
    if (mode == 1) {
      // Copy
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 15);
      display.println("COPY!!");
      display.display();
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('c');
      delay(100);
      Keyboard.releaseAll();
      delay(1000);
      displayMode();
    } else if (mode == 2) {
      // Volume --
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 15);
      display.println("Volume --");
      display.display();
      Consumer.write(MEDIA_VOLUME_DOWN);
      Consumer.releaseAll();
      delay(1000);
      displayMode();
    }
  }

  if (digitalRead(pinC) == LOW && !isPlaying) {
    if (mode == 1) {
      // Paste
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 15);
      display.println("PASTE!!");
      display.display();
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('v');
      delay(100);
      Keyboard.releaseAll();
      delay(1000);
      displayMode();
    } else if (mode == 2) {
      // Volume ++
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 15);
      display.println("Volume ++");
      display.display();
      Consumer.write(MEDIA_VOLUME_UP);
      Consumer.releaseAll();
      delay(1000);
      displayMode();
    }
  }

  if (digitalRead(pinD) == LOW && !isPlaying) {
    if (mode == 0) {
      if (displayScreen == SCREEN_MAIN) {
        displayScreen = SCREEN_PONG;
        delay(100);
      } else if (displayScreen == SCREEN_PONG) {
        displayScreen = SCREEN_MAIN;
        delay(100);
      }
      displayMode();
    } else if (mode == 1) {
      // Print Screen
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 15);
      display.println("PRINT SCREEN!!");
      display.display();
      Keyboard.press(KEY_PRINTSCREEN);
      delay(100);
      Keyboard.releaseAll();
      delay(1000);
      displayMode();
    } else if (mode == 2) {
      // Play or pause
      if (play) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 15);
        display.println("PAUSE!!");
        display.display();
        Consumer.write(MEDIA_PAUSE);
        play = false;
        Consumer.releaseAll();
      } else {
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 15);
        display.println("PLAY!!");
        display.display();
        Consumer.write(MEDIA_PLAY_PAUSE);
        play = true;
        Consumer.releaseAll();
      }
      delay(1000);
      displayMode();
    }
  }

  // Open Apps
  if (digitalRead(pinE) == LOW && !isPlaying) {
    if (mode == 1) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 15);
      display.println("OPEN VSCODE!!");
      display.display();
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('9');
      delay(200);
      Keyboard.releaseAll();
    } else if (mode == 2) {
      display.clearDisplay();
      display.setCursor(0, 15);
      display.setTextSize(1);
      display.println("OPEN CHROME!!");
      display.display();
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('5');
      delay(200);
      Keyboard.releaseAll();
    }
    delay(1000);
    displayMode();
  }

  if (digitalRead(pinF) == LOW && !isPlaying) {
    if (mode == 0) {
      if (displayScreen == SCREEN_MAIN) {
        displayScreen = SCREEN_PONG;
        delay(100);
      } else if (displayScreen == SCREEN_PONG) {
        displayScreen = SCREEN_MAIN;
        delay(100);
      }
      delay(100);
      displayMode();
    } else if (mode == 1) {
      // Open Password
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 15);
      display.println("OPEN PASSWORD!!");
      display.display();
      Keyboard.print("password");
      delay(50);
      Keyboard.press(KEY_ENTER);
      delay(100);
      Keyboard.releaseAll();
      delay(1000);
      displayMode();
    } else if (mode == 2) {
      // Back home
      mode = 0;
      displayScreen = SCREEN_MAIN;
      delay(100);
      displayMode();
      delay(100);
    }
  }

  lastButtonState = buttonState;
}
