/* This is a valid comment

  A sketch to work with a 7 segment display and 8-bit shift register and a push button.
  The 7 segment displays numbers starting countStart to countEnd with a delay equals to countDelay between each update
  When the push button is pushed for a short period , counting reset happens
  When the push button is pushed for a long period , counting order (ascending, descending) toggles.

  by : Abdelrahman Galal

*/

//const byte displayOne = 8;
//const byte displayTwo = 9;
//const byte displayThree = 10;
//const byte displayFour = 11;

const byte dataPin = 2;
const byte shiftClockPin = 4;
const byte memoryClockPin = 7;
const byte plusPin = 5;
const byte minusPin = 6;
const byte functionPin = 3;
const byte alarmPin = 12 ;
const byte displayDigitPins[] = {8, 9, 10, 11};
unsigned long countPreviousTime ; //last time the counter digit was changed
const byte leds[] = {252, 96, 218, 242, 102, 182, 190, 224, 254, 246};
int functionState = 0;
int plusState = 0;
int S1 = 0;
int S2 = 0;
int M1 = 0;
int M2 = 0;

//setup function
void setup()
{

  pinMode(dataPin, OUTPUT);
  pinMode(shiftClockPin, OUTPUT);
  pinMode(memoryClockPin, OUTPUT);
  pinMode(functionPin, INPUT);
  pinMode(plusPin, INPUT);
  pinMode(minusPin, INPUT);
  pinMode(alarmPin, OUTPUT);
  digitalWrite(alarmPin, LOW);
  Serial.begin(9600);
  for (int i = 0; i <= 3; i++)
  {
    pinMode(displayDigitPins[i], OUTPUT);
  }
}

//loop function
void loop()
{
  setTimer(&S1 , &S2, &M1, &M2);
  countDown();
  fireAlarm(0, 0, 0, 0);
}

void setTimer(int *s1 , int *s2 , int *m1, int *m2)
{
  int secondsPlusCount = 0;
  int secondsMinusCount = 0;
  int secondsTotalCount = 0;
  int minsPlusCount = 0;
  int minsMinusCount = 0;
  int minsTotalCount = 0;
  unsigned long lastButtonPush = millis();
  int buttonsState = 0; //to choose between setting minutes or seconds (defaultt 0 seconds)
  functionState = 0;
  while (!functionState || ( millis() - lastButtonPush < 5000) )
  {
    if (digitalRead(functionPin) == LOW)
    {
      buttonsState = !buttonsState;
      if ( !buttonsState )
      {
        customDelay(leds[*s1], leds[*s2], 0, 0);
      }
      else
      {
        customDelay(0, 0, leds[*m1], leds[*m2]);
      }
    }

    if (!buttonsState)
    {
      oneDisplayRound(leds[*s1], leds[*s2], 0, 0);
      //check plus button
      if (digitalRead(plusPin) == LOW)
      {
        customDelay(leds[*s1], leds[*s2], 0, 0);
        secondsPlusCount++;
        functionState = 1;
        lastButtonPush = millis();
      }
      //check minus button
      if (digitalRead(minusPin) == LOW)
      {
        customDelay(leds[*s1], leds[*s2], 0, 0);
        secondsMinusCount++;
        functionState = 1;
        lastButtonPush = millis();
      }
      if (secondsTotalCount + secondsPlusCount - secondsMinusCount < 0 )
      {
        secondsTotalCount = 60 - secondsMinusCount;
      }
      else
      {
        secondsTotalCount = secondsTotalCount + secondsPlusCount - secondsMinusCount;
      }
      secondsPlusCount = 0;
      secondsMinusCount = 0;
      char digits[3];
      //convert int to array of chr
      itoa(secondsTotalCount, digits, 10);
      if (secondsTotalCount <= 9)
      {
        *s1 = digits[0] - '0';
        *s2 = 0;

      }
      else if (secondsTotalCount <= 59)
      {
        *s1 = digits[1] - '0';
        *s2 = digits[0] - '0';
      }
      else if (secondsTotalCount > 59)
      {
        *s1 = 0;
        *s2 = 0;
        secondsTotalCount = 0;
      }

    }
    else {
      //check plus button
      oneDisplayRound(0, 0, leds[*m1], leds[*m2]);
      if (digitalRead(plusPin) == LOW)
      {
        customDelay(0, 0, leds[*m1], leds[*m2]);
        minsPlusCount++;
        functionState = 1;
        lastButtonPush = millis();
      }
      //check minus button
      if (digitalRead(minusPin) == LOW)
      {
        customDelay(0, 0, leds[*m1], leds[*m2]);
        minsMinusCount++;
        functionState = 1;
        lastButtonPush = millis();
      }
      if (minsTotalCount + minsPlusCount - minsMinusCount < 0 )
      {
        minsTotalCount = 24 - minsMinusCount;
      }
      else
      {
        minsTotalCount = minsTotalCount + minsPlusCount - minsMinusCount;
      }
      minsPlusCount = 0;
      minsMinusCount = 0;
      char digits[3];
      //convert int to array of chr
      itoa(minsTotalCount, digits, 10);
      if (minsTotalCount <= 9)
      {
        *m1 = digits[0] - '0';
        *m2 = 0;
      }
      else if (minsTotalCount <= 23)
      {
        *m1 = digits[1] - '0';
        *m2 = digits[0] - '0';
      }
      else if (minsTotalCount > 23)
      {
        *m1 = 0;
        *m2 = 0;
        minsTotalCount = 0;
      }
    }
  }
  functionState = 0;
}

void countDown()
{
  countPreviousTime = millis();
  for (M2 ; M2 >= 0 ; M2--)
  {
    for (M1; M1 >= 0 ; M1--)
    {
      for (S2 ; S2 >= 0 ; S2--)
      {
        for (S1 ; S1 >= 0 ; S1--)
        {
          while ((millis() - countPreviousTime) < 1000)
          {
            if (digitalRead(functionPin) == LOW)
            {
              S1 = 0;
              S2 = 0;
              M1 = 0;
              M2 = 0;
              customDelay(leds[0], leds[0], 0, 0);
              return;
            }
            oneDisplayRound(leds[S1], leds[S2], leds[M1] + 1, leds[M2]);
            //Serial.println(S1);
          }
          countPreviousTime = millis();
        }
        //reset s1 to countdwn from 9
        S1 = 9;
      }
      //reset s2 to countdwn from 5
      S2 = 5;
    }
    M1 = 3;
  }
  S1 = 0;
  S2 = 0;
  M1 = 0;
  M2 = 0;
  functionState = 1;
}


void showDigit(byte digit, byte displayIndex)
{
  outSeven(digit);
  for (byte i = 8; i <= 11; i++)
  { if ( i == displayIndex )
      digitalWrite(i, LOW);
    else
      digitalWrite(i, HIGH);
  }
  delay(3);
}


//function to change the shown digit of the 7 segment
void outSeven(byte led)
{
  digitalWrite(memoryClockPin, LOW);
  shiftOut(dataPin, shiftClockPin, LSBFIRST, led);
  digitalWrite(memoryClockPin, HIGH);
}

void customDelay(byte b1, byte b2, byte b3, byte b4)
{
  for (int i = 0 ; i < 30; i++)
  {
    oneDisplayRound(b1, b2, b3, b4);
  }
}

void oneDisplayRound(byte b1, byte b2, byte b3, byte b4)
{
  showDigit(b1, displayDigitPins[0]);
  showDigit(b2, displayDigitPins[1]);
  showDigit(b3, displayDigitPins[2]);
  showDigit(b4, displayDigitPins[3]);
}

void fireAlarm(int b1, int b2, int b3, int b4)
{
  if (S1 == b1 && S2 == b2 && M1 == b3 && M2 == b4 && functionState)
  {
    while (digitalRead(functionPin) == HIGH)
    {
      oneDisplayRound(leds[0], leds[0], leds[0], leds[0]);
      digitalWrite(alarmPin, HIGH);
    }
    digitalWrite(alarmPin, LOW);
    functionState = 0;
    customDelay(leds[0], leds[0], 0, 0);
  }
}
