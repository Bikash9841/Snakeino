 #include <TimerOne.h>
int snakelen = 0;
int latchPin = 6;
int clockPin = 5;
int dataPin = 7;
int latchPinx = 9;
int clockPinx = 10;
int dataPinx = 8;

#define JOY_X_PIN A5
#define JOY_Y_PIN A4
#define JOY_SW_PIN 2

uint8_t global_counter = 0;

// converted to decimal
uint32_t X0 = 0;        // 0B000000000000000000000000;
uint32_t Y0 = 16777215; // 0B111111111111111111111111;
uint32_t X1 = 8388608;  //0B100000000000000000000000;
uint32_t Y1 = 8388607;  //0B011111111111111111111111;

/*  Relation between consecutive coordinates:
 *  Xn = X1/2^(n-1)
 *  Yn = Y(n-1) + Xn
 */
uint32_t X_cord[22] = {X0, X1, X1 / 2, X1 / 4, X1 / 8, X1 / 16, X1 / 32, X1 / 64, X1 / 128, X1 / 256, X1 / 512, X1 / 1024, X1 / 2048, X1 / 4096, X1 / 8192, X1 / 16384, X1 / 32768, X1 / 65536, X1 / 131072, X1 / 262144, X1 / 524288, X1 / 1048576};
uint32_t Y_cord[22] = {Y0, Y1, (Y1 + X_cord[2]), (Y_cord[2] + X_cord[3]), (Y_cord[3] + X_cord[4]), (Y_cord[4] + X_cord[5]), (Y_cord[5] + X_cord[6]), (Y_cord[6] + X_cord[7]), (Y_cord[7] + X_cord[8]), (Y_cord[8] + X_cord[9]), (Y_cord[9] + X_cord[10]), (Y_cord[10] + X_cord[11]), (Y_cord[11] + X_cord[12]), (Y_cord[12] + X_cord[13]), (Y_cord[13] + X_cord[14]), (Y_cord[14] + X_cord[15]), (Y_cord[15] + X_cord[16]), (Y_cord[16] + X_cord[17]), (Y_cord[17] + X_cord[18]), (Y_cord[18] + X_cord[19]), (Y_cord[19] + X_cord[20]), (Y_cord[20] + X_cord[21])};
enum snakeDir
{
  sRIGHT,
  sLEFT,
  sUP,
  sDOWN
};

enum snakeLife
{
  sDEAD,
  sALIVE
};

typedef struct snake_str
{
  uint8_t head[2];
  uint8_t tail[2];
  snakeDir dir;   //direction
  uint8_t metric; //length
  snakeLife life;
  uint8_t score;
} snake;
snake Snake;
typedef struct game_str
{
  uint8_t threshold_score;
  uint8_t level;
  int pace;
  uint8_t chances;
  uint8_t life = 1; //0->game_over
} game;
game Game;

typedef struct fruit_str
{
  uint8_t pos[2];
} fruit;
fruit Fruit;
void setup()
{

  //shift-register pins
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPinx, OUTPUT);
  pinMode(dataPinx, OUTPUT);
  pinMode(clockPinx, OUTPUT);

  //joy-stick pins

  pinMode(JOY_SW_PIN, INPUT);
  digitalWrite(JOY_SW_PIN, OUTPUT);
  pinMode(JOY_X_PIN, INPUT);
  pinMode(JOY_Y_PIN, INPUT);

  //timer interrupt

  Timer1.initialize(25000); //100 millis
  Timer1.attachInterrupt(timerOneISR);

  //snake initialize
  snakeReset();

  //fruit initialize
  Fruit.pos[0] = random(2, 20);
  Fruit.pos[1] = random(2, 20);

  //game initialize
  Snake.score = 0;
  Game.threshold_score = 2;
  Snake.head[0] = 10;
  Snake.head[1] = 10;
  Snake.tail[0] = Snake.head[0];
  Snake.tail[1] = Snake.head[1];
  Snake.life = sALIVE;
  Game.life = 1;
  Game.level = 1;
  Game.chances = 3;
  Game.pace = 10; //global_counter unit (1 sec)
}

void snakeReset()
{
  Snake.score = 0;
  Game.threshold_score = 2;
  //snakelen=0;
  Snake.head[0] = 10;
  Snake.head[1] = 10;
  Snake.life = sALIVE;
  Game.life = 1;
  Game.level = 1;
  Game.chances = 3;
  Game.pace = 10;
  Fruit.pos[0] = random(2, 20);
  Fruit.pos[1] = random(2, 20);
}

void timerOneISR()
{
  if (Game.pace < 1)
  {
    snakeReset();
  }
  else if (global_counter == Game.pace)
  {
  
  if (Snake.dir == sLEFT)
  {
    Snake.head[1]--;

  } //y--

  else if (Snake.dir == sRIGHT)
  {
    Snake.head[1]++;
    //y++
  }
  else if (Snake.dir == sUP)
  {
    Snake.head[0]++;
    //x++
  }
  else if (Snake.dir == sDOWN)
  {
    Snake.head[0]--; //x--
  }
  global_counter = 0;
}

global_counter++;
if (analogRead(JOY_Y_PIN) < 100)
{
  Snake.dir = sRIGHT;
}
else if (analogRead(JOY_Y_PIN) > 1000)
{
  Snake.dir = sLEFT;
}
else if (analogRead(JOY_X_PIN) < 100)
{
  Snake.dir = sUP;
}
else if (analogRead(JOY_X_PIN) > 1000)
{
  Snake.dir = sDOWN;
}
}
void ShiftVCC(uint32_t x)
{

  uint8_t h_data = (x >> 16) & (0xff); //1-8
  uint8_t m_data = (x >> 8) & (0xff);  //9-16
  uint8_t l_data = (x >> 0) & (0xff);  //17-24

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, l_data);
  shiftOut(dataPin, clockPin, LSBFIRST, m_data);
  shiftOut(dataPin, clockPin, LSBFIRST, h_data);
  digitalWrite(latchPin, HIGH);
}
void ShiftGND(uint32_t y)
{
  uint8_t h_data = (y >> 16) & (0xff); //1-8
  uint8_t m_data = (y >> 8) & (0xff);  //9-16
  uint8_t l_data = (y >> 0) & (0xff);  //17-24

  digitalWrite(latchPinx, LOW);
  shiftOut(dataPinx, clockPinx, LSBFIRST, l_data);
  shiftOut(dataPinx, clockPinx, LSBFIRST, m_data);
  shiftOut(dataPinx, clockPinx, LSBFIRST, h_data);
  digitalWrite(latchPinx, HIGH);
}

void reset()
{ // to turn off all leds
  ShiftVCC(X0);
  ShiftGND(Y0);
}

void coordinate(int x, int y)
{
  ShiftVCC(X_cord[x]);
  ShiftGND(Y_cord[y]);
  reset();
}

void point(int X1, int Y1, int X2, int Y2)
{
  int r = Y2 - Y1;
  int c = X2 - X1;

  if (X1 > X2)
  {
    int temp = X1;
    X1 = X2;
    X2 = temp;
  }

  if (Y1 > Y2)
  {
    int temp1 = Y1;
    Y1 = Y2;
    Y2 = temp1;
  }

  if (c == 0)
  {
    for (int i = Y1; i <= Y1 + (Y2 - Y1); i++)
    {
      coordinate(X1, i);
    }
  }

  if (r == 0)
  {
    for (int i = X1; i <= X1 + (X2 - X1); i++)
    {
      coordinate(i, Y1);
    }
  }

  if (c != 0 && r != 0)
  {
    for (int i = X1; i <= X1 + (X2 - X1); i++)
    {
      coordinate(i, Y1);
    }

    for (int i = Y1; i <= Y1 + (Y2 - Y1); i++)
    {
      coordinate(X2, i);
    }
  }
}
void Boundary()
{

  point(1, 1, 21, 1);
  point(21, 1, 21, 21);
  point(21, 21, 1, 21);
  point(1, 21, 1, 1);
}
void gameOver()
{
  point(10, 3, 18, 3);
  point(10, 7, 18, 7);
  point(10, 11, 18, 11);
  point(10, 15, 18, 15);
  point(10, 19, 18, 19);
  coordinate(8, 3);
  coordinate(8, 7);
  coordinate(8, 11);
  coordinate(8, 15);
  coordinate(8, 19);
}

void loop()
{

  //if snake is alive
  if (Game.life == 1 && Snake.life == sALIVE)
  {

    coordinate(Snake.head[0], Snake.head[1]);
    //render fruit
    coordinate(Fruit.pos[0], Fruit.pos[1]);

    if ((Snake.head[0] == Fruit.pos[0]) && (Snake.head[1] == Fruit.pos[1]))
    {

      if (Snake.score > Game.threshold_score)
      {
        Game.level++;
        Game.pace -= 1;
      }
      else
      {
        Snake.score++;
      }

      Fruit.pos[0] = random(2, 20);
      Fruit.pos[1] = random(2, 20);
    }
    if (Snake.head[0] < 2 || Snake.head[0] > 20 || Snake.head[1] < 2 || Snake.head[1] > 20)
    {
      Snake.life = sDEAD;
    }
  }
  else if (Snake.life == sDEAD)
  {
    for (int i = 1; i < 18; i++)
    {
      Boundary();
      gameOver();
    }
    snakeReset();
  }
}
