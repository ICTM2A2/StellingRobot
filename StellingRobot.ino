const bool DEBUG = true;

// Arduino pins for the shift register
#define MOTORLATCH 12
#define MOTORCLK 4
#define MOTORENABLE 7
#define MOTORDATA 8

// 8-bit bus after the 74HC595 shift register
// (not Arduino pins)
// These are used to set the direction of the bridge driver.
#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR3_A 5
#define MOTOR3_B 7
#define MOTOR4_A 0
#define MOTOR4_B 6

// Arduino pins for the PWM signals.
#define MOTOR1_PWM 11
#define MOTOR2_PWM 3
#define MOTOR3_PWM 6
#define MOTOR4_PWM 5
#define SERVO1_PWM 10
#define SERVO2_PWM 9

// PotPins
const int POT_PIN_X = A0;
const int POT_PIN_Y = A1;

// Codes for the motor function.
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4

const int xSpeed = 125;
const int ySpeed = 255;
const int zSpeed = 100;

const int margin = 5;

void setup()
{
  // Open Serial
  Serial.begin(9600);

  // Reset the location to the Home point
  // Start "station"
  Serial.println("Resetting location...");
  home();
  Serial.println("Online!");
}

void loop()
{
  // //* Log X & Y position
  // Serial.print("x:");
  // Serial.println(analogRead(POT_PIN_X));
  // Serial.print("y:");
  // Serial.println(analogRead(POT_PIN_Y));
  // delay(500);

  //? =========================================================
  //? || Coordinates                                         ||
  //? =========================================================

  if (Serial.available() > 0)
  {
    String input = Serial.readString();

    int x1 = charToCoord(input[0]);
    int y1 = charToCoord(input[2]);
    int x2 = charToCoord(input[4]);
    int y2 = charToCoord(input[6]);
    int x3 = charToCoord(input[8]);
    int y3 = charToCoord(input[10]);

    int coords[3][2] = {{x1, y1}, {x2, y2}, {x3, y3}};

    // move(2, 2);
    // grab();
    // home();
    // delay(1000);
    // move(2, 2);
    // put();

    for (int i = 0; i < 3; i++)
    {
      int x = coords[i][0];
      int y = coords[i][1];

      if (x != 5 && y != 5) // For when there's not a full set
      {
        move(x, y);
        grab();
      }
    }

    drop();

    home();

    Serial.println("Done!");
  }
}

int charToCoord(char in)
{
  switch (in)
  {
  case 48:
    return 0;
  case 49:
    return 1;
  case 50:
    return 2;
  case 51:
    return 3;
  case 52:
    return 4;
  default:
    return 5;
  }
}

void move(int x, int y)
{
  if (y == -1)
  {
    y = 0;
  }

  if (DEBUG)
  {

    Serial.print("Moving to ");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.println("");
  }

  //? =========================================================
  //? || Destinations                                        ||
  //? =========================================================

  int destX = 0;
  int destY = 0;

  switch (x)
  {
  case 0:
    destX = 854; //* value
    break;
  case 1:
    destX = 700; //* value
    break;
  case 2:
    destX = 554; //* value
    break;
  case 3:
    destX = 405; //* value
    break;
  case 4:
    destX = 258; //* value
    break;
  default:
    destX = 0; //* value
    break;
  }

  switch (y)
  {
  case 0:
    destY = 0; //* value
    break;
  case 1:
    destY = 205; //* value
    break;
  case 2:
    destY = 403; //* value
    break;
  case 3:
    destY = 610; //* value
    break;
  case 4:
    destY = 815; //* value
    break;
  case -1: // drop correctie
    destY = 135;
    break;
  case -2: // drop height
    destY = 305;
    break;
  default:
    destY = 0; //* value
    break;
  }

  //? =========================================================
  //? || X-as                                                ||
  //? =========================================================

  int currentX = analogRead(POT_PIN_X);

  if (currentX - destX > margin || currentX - destX < -margin)
  {
    if (currentX < destX)
    {
      while (analogRead(POT_PIN_X) < destX)
      {
        motor(3, BACKWARD, xSpeed);
        delay(5);
        motor(3, BRAKE, 0);
      }
    }
    else
    {
      if (x == -1) // Home
      {
        while (analogRead(POT_PIN_X) > destX)
        {
          motor(3, FORWARD, xSpeed);
          delay(5);
          motor(3, BRAKE, 0);
        }
      }
      else
      {
        while (analogRead(POT_PIN_X) > destX - 10)
        {
          motor(3, FORWARD, xSpeed);
          delay(5);
          motor(3, BRAKE, 0);
        }
        while (analogRead(POT_PIN_X) < destX)
        {
          motor(3, BACKWARD, xSpeed);
          delay(5);
          motor(3, BRAKE, 0);
        }
      }
    }
  }

  if (DEBUG)
  {
    Serial.println("Moved x");
  }

  //? =========================================================
  //? || Y-as                                                ||
  //? =========================================================

  //@ FORWARD  = UP
  //@ BACKWARD = DOWN

  int currentY = analogRead(POT_PIN_Y);

  if (currentY - destY > margin || currentY - destY < -margin)
  {
    if (currentY < destY) // go up
    {
      while (analogRead(POT_PIN_Y) < destY)
      {
        motor(1, FORWARD, ySpeed);
        delay(5);
        motor(1, BRAKE, 0);
      }
    }
    else if (currentY > destY) // go down
    {
      if (y == 0) // go Home
      {
        while (analogRead(POT_PIN_Y) > destY)
        {
          motor(1, BACKWARD, ySpeed);
          delay(5);
          motor(1, BRAKE, 0);
        }
      }
      else
      {
        while (analogRead(POT_PIN_Y) > destY - 10)
        {
          motor(1, BACKWARD, ySpeed);
          delay(5);
          motor(1, BRAKE, 0);
        }
        while (analogRead(POT_PIN_Y) < destY)
        {
          motor(1, FORWARD, ySpeed);
          delay(5);
          motor(1, BRAKE, 0);
        }
      }
    }
  }

  if (DEBUG)
  {
    Serial.println("Moved y");
  }
}

void grab()
{
  // forwards
  motor(2, FORWARD, zSpeed);
  delay(600);
  motor(2, BRAKE, 0);

  delay(100);

  // up
  motor(1, FORWARD, ySpeed);
  delay(200);
  motor(1, BRAKE, 0);

  delay(100);

  // back
  motor(2, BACKWARD, zSpeed);
  delay(600);
  motor(2, BRAKE, 0);
}

void put()
{
  motor(1, FORWARD, ySpeed);
  delay(200);
  motor(1, BRAKE, 0);

  motor(2, FORWARD, zSpeed);
  delay(400);
  motor(2, BRAKE, 0);

  delay(100);

  motor(1, BACKWARD, ySpeed);
  delay(150);
  motor(1, BRAKE, 0);

  delay(100);

  motor(2, BACKWARD, zSpeed);
  delay(600);
  motor(2, BRAKE, 0);
}

void drop()
{
  if (DEBUG)
  {
    Serial.println("Moving to Drop");
  }

  move(-1, -1);

  // Extend arm
  motor(2, FORWARD, zSpeed);
  delay(200);
  motor(2, BRAKE, 0);

  move(-1, -2);

  if (DEBUG)
  {
    Serial.println("Dropping!");
  }

  // Extend arm
  motor(2, FORWARD, zSpeed);
  delay(1000);
  motor(2, BRAKE, 0);

  move(-1, 0);

  // Retract arm
  motor(2, BACKWARD, zSpeed);
  delay(1000);
  motor(2, BRAKE, 0);
}

void home()
{
  if (DEBUG)
  {
    Serial.println("Moving to Home");
  }

  // Retract arm
  motor(2, BACKWARD, zSpeed);
  delay(1000);
  motor(2, BRAKE, 0);

  move(-1, 0);
}

//? ===========================================================
//? || Motor related functions                               ||
//? ===========================================================

void motor(int nMotor, int command, int speed)
{
  int motorA, motorB;

  if (nMotor >= 1 && nMotor <= 4)
  {
    switch (nMotor)
    {
    case 1:
      motorA = MOTOR1_A;
      motorB = MOTOR1_B;
      break;
    case 2:
      motorA = MOTOR2_A;
      motorB = MOTOR2_B;
      break;
    case 3:
      motorA = MOTOR3_A;
      motorB = MOTOR3_B;
      break;
    case 4:
      motorA = MOTOR4_A;
      motorB = MOTOR4_B;
      break;
    default:
      break;
    }

    switch (command)
    {
    case FORWARD:
      motor_output(motorA, HIGH, speed);
      motor_output(motorB, LOW, -1); // -1: no PWM set
      break;
    case BACKWARD:
      motor_output(motorA, LOW, speed);
      motor_output(motorB, HIGH, -1); // -1: no PWM set
      break;
    case BRAKE:
      motor_output(motorA, LOW, 255); // 255: fully on.
      motor_output(motorB, LOW, -1);  // -1: no PWM set
      break;
    case RELEASE:
      motor_output(motorA, LOW, 0);  // 0: output floating.
      motor_output(motorB, LOW, -1); // -1: no PWM set
      break;
    default:
      break;
    }
  }
}

void motor_output(int output, int high_low, int speed)
{
  int motorPWM;

  switch (output)
  {
  case MOTOR1_A:
  case MOTOR1_B:
    motorPWM = MOTOR1_PWM;
    break;
  case MOTOR2_A:
  case MOTOR2_B:
    motorPWM = MOTOR2_PWM;
    break;
  case MOTOR3_A:
  case MOTOR3_B:
    motorPWM = MOTOR3_PWM;
    break;
  case MOTOR4_A:
  case MOTOR4_B:
    motorPWM = MOTOR4_PWM;
    break;
  default:
    // Use speed as error flag, -3333 = invalid output.
    speed = -3333;
    break;
  }

  if (speed != -3333)
  {
    // Set the direction with the shift register
    // on the MotorShield, even if the speed = -1.
    // In that case the direction will be set, but
    // not the PWM.
    shiftWrite(output, high_low);

    // set PWM only if it is valid
    if (speed >= 0 && speed <= 255)
    {
      analogWrite(motorPWM, speed);
    }
  }
}

void shiftWrite(int output, int high_low)
{
  static int latch_copy;
  static int shift_register_initialized = false;

  // Do the initialization on the fly,
  // at the first time it is used.
  if (!shift_register_initialized)
  {
    // Set pins for shift register to output
    pinMode(MOTORLATCH, OUTPUT);
    pinMode(MOTORENABLE, OUTPUT);
    pinMode(MOTORDATA, OUTPUT);
    pinMode(MOTORCLK, OUTPUT);

    // Set pins for shift register to default value (low);
    digitalWrite(MOTORDATA, LOW);
    digitalWrite(MOTORLATCH, LOW);
    digitalWrite(MOTORCLK, LOW);
    // Enable the shift register, set Enable pin Low.
    digitalWrite(MOTORENABLE, LOW);

    // start with all outputs (of the shift register) low
    latch_copy = 0;

    shift_register_initialized = true;
  }

  // The defines HIGH and LOW are 1 and 0.
  // So this is valid.
  bitWrite(latch_copy, output, high_low);

  // Use the default Arduino 'shiftOut()' function to
  // shift the bits with the MOTORCLK as clock pulse.
  // The 74HC595 shiftregister wants the MSB first.
  // After that, generate a latch pulse with MOTORLATCH.
  shiftOut(MOTORDATA, MOTORCLK, MSBFIRST, latch_copy);
  delayMicroseconds(5); // For safety, not really needed.
  digitalWrite(MOTORLATCH, HIGH);
  delayMicroseconds(5); // For safety, not really needed.
  digitalWrite(MOTORLATCH, LOW);
}
