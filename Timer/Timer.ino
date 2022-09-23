/* 
  - A sketch to work with a 4 digit 7-segment display,8-bit shift register,active buzzer and 3 push buttons.
  - Using the 3 buttons (function,plus,minus) buttons the user can set a time.
  - When the user stop setting the timer , the screen starts to count down until it reach zero time, at this point the buzzer starts beeping until function button is pushed.
  - If the function button is pushed in the process of counting down,the user is prompoted to set a new counter.
  - Author : Abdelrahman Galal

       A
      ---
   F |   | B
     | G |
      ---
   E |   | C
     |   |
      ---  .
       D  DP
*/

const byte dataPin = 2; // send serial digits to 7-segment through the shift register
const byte shiftClockPin = 4;
const byte memoryClockPin = 7;
const byte plusPin = 5; //connected to the plus button
const byte minusPin = 6; //connected to the minus button
const byte functionPin = 3; //connected to the function button
const byte alarmPin = 12 ; //connected to the active buzzer positive leg
const byte displayDigitPins[] = {8, 9, 10, 11}; //control pins for the 4 digit 7-segment , each pin allow one digit to be active at a time
unsigned long countPreviousTime ; //last time the counter digit was changed
const byte leds[] = {252, 96, 218, 242, 102, 182, 190, 224, 254, 246}; //the 7 segment form f numbers between 0 and 9
int functionState = 0; //variable to track the state of the device (alarm set,alarm not set,count is done)


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
  for (int i = 0; i <= 3; i++)
  {
    pinMode(displayDigitPins[i], OUTPUT);
  }
}

//loop function
void loop()
{
  int S1 = 0; //displayed number on digit 1 of the 7-segment
  int S2 = 0; //displayed number on digit 2 of the 7-segment
  int M1 = 0; //displayed number on digit 3 of the 7-segment
  int M2 = 0; //displayed number on digit 4 of the 7-segment
  setTimer(&S1 , &S2, &M1, &M2);
  countDown(&S1 , &S2, &M1, &M2);
  if (S1 == 0 && S2 == 0 && M1 == 0 && M2 == 0 && functionState)
  {
    fireAlarm();
  }
}

//function to prompot user to set counting time
void setTimer(int *s1 , int *s2 , int *m1, int *m2)
{
  //3 variables to calc seconds according to how many time the plus and minus buttons have been pushed
  int secondsPlusCount = 0;
  int secondsMinusCount = 0;
  int secondsTotalCount = 0;
  //3 variables to calc minutes according to how many time the plus and minus buttons have been pushed
  int minsPlusCount = 0;
  int minsMinusCount = 0;
  int minsTotalCount = 0;
  unsigned long lastButtonPush = millis();   //last time either plus or mins button has been pushed
  int buttonsState = 0; //to choose between setting minutes or seconds (defaultt 0 seconds)
  functionState = 0;   // functionState = 0 while not counting means that user set time
  // exist only when either plus or mins button have been pushed AND the user is inactive for 5 seconds
  while (!functionState || ( millis() - lastButtonPush < 5000) )
  {
    //toggle between setting mins or seconds when function button is pushed
    if (digitalRead(functionPin) == LOW)
    {
      buttonsState = !buttonsState; //toggle between mins and seconds
      if ( !buttonsState )
      {
        customDelay(leds[*s1], leds[*s2], 0, 0); //seconds are active
      }
      else
      {
        customDelay(0, 0, leds[*m1], leds[*m2]); //mins are active
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
      //to enable the user to start setting the seconds at 59
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
      //to enable the user to start setting the minutes at 23
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

void countDown(int *s1 , int *s2 , int *m1, int *m2)
{
  countPreviousTime = millis();
  for (*m2 ; *m2 >= 0 ; --*m2)
  {
    for (*m1; *m1 >= 0 ; --*m1)
    {
      for (*s2 ; *s2  >= 0 ; --*s2)
      {
        for (*s1  ; *s1 >= 0 ; --*s1)
        {
          while ((millis() - countPreviousTime) < 1000)
          {
            //exit if function button is pushed during counting down
            if (digitalRead(functionPin) == LOW)
            {
              *s1 = 0;
              *s2 = 0;
              *m1 = 0;
              *m2 = 0;
              customDelay(leds[0], leds[0], 0, 0);
              return;
            }
            oneDisplayRound(leds[*s1], leds[*s2], leds[*m1] + 1, leds[*m2]);
            //Serial.println(S1);
          }
          countPreviousTime = millis();
        }
        //reset s1 to countdwn from 9
        *s1 = 9;
      }
      //reset s2 to countdwn from 5
      *s2 = 5;
    }
    *m1 = 3;
  }
  *s1 = 0;
  *s2 = 0;
  *m1 = 0;
  *m2 = 0;
  functionState = 1;
}

//function to implement dynamic scanning
void showDigit(byte digit, byte displayIndex)
{
  outSeven(digit);
  //only one digit is active at a time
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

//function to encounter for button bouncing by introducing a delay
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


//function to activate the buzzer
void fireAlarm()
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
