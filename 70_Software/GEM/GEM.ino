/*******************************************/
/*******************************************/
//Inputs and Ouputs
//Sensors
bool bGlassDown = false;
bool bLiquidDown = false;
bool bLiquidPresent = false;
bool bGlassPresent = false;

//Motor driver 1
const int PWM11 = 11;
const int PWM12 = 12;
const int DIR11 = 9;
const int DIR12 = 10;
int iActualStep = 1;
float fPWMPbr = 40;
int idelayMs = 1;

//HMI
//Operation
bool bStart = false;
bool bStopp = false;
bool bQuit = false;

//Handmode
bool bManualLiquidUp = false;
bool bManualLiquidDown = false;
bool bManualGlassUp = false;
bool bManualGlassDown = false;

//Communication
int incomingByte = 0;  // for incoming serial data


//Mode
int iMode = 0;
/* 0: Off
 * 1: Auto
 * 2: Manual
 * 3: Inspection
 */
 
/*******************************************/
/*******************************************/
//GlobalVariableList GVL
bool bOn = false;
bool bInitPos = false;
bool bInitialised = false;
bool bError = false;

bool bMotorLiquidUp = false;
bool bMotorLiquidDown = false;
bool bMotorGlassUp = false;
bool bMotorGlassDown = false;

int iPosLiquid = 0;
int iPosGlass = 0;
int iState = 0; 
int iLastState = 0;
/* 0: INIT
 * 1: WAIT
 * 2: POURING
 * 3: PLACING
 * 9: ERROR
 */

//Constantes
int iMaxHeightLiquid = 100;
int iMaxHeightGlass = 100;

/*******************************************/
/*******************************************/
void setup() 
{
  // put your setup code here, to run once:
  Serial1.begin(9600);
  Serial1.println("Hi");
}

/*******************************************/
/*******************************************/
void loop() 
{
  //Communication
  // reply only when you receive data:
  if (Serial1.available() > 0) 
  {
    // read the incoming byte:
    incomingByte = Serial1.read();

    // say what you got:
    Serial1.print("I received: ");
    Serial1.println(incomingByte, DEC);
  }



  
  //Update States
  //Init Position
  bInitPos = bGlassDown and bLiquidDown;

  //Position Glas
  if(bGlassDown)
  {
    iPosGlass = 0;
  }

  //Positon Liquid
  if(bLiquidDown)
  {
    iPosLiquid = 0;
  }


    //Check the Operating Mode
    switch(iMode)
    {
      //Off
      case 0:
        //Reset Motor
        bMotorLiquidUp = false;
        bMotorLiquidDown = false;
        bMotorGlassUp = false;
        bMotorGlassDown = false;
        break;

      
      //Automatic Mode 
      case 1:

        //Reset Motor
        bMotorLiquidUp = false;
        bMotorLiquidDown = false;
        bMotorGlassUp = false;
        bMotorGlassDown = false;

          if(bStart)
          {
            bOn = true;        
          }
          else if(bStopp)
          {
            bOn = false;
          }



        switch(iState)
        {
          //INIT
          case 0:
            if(bInitialised)
            {
              iLastState = iState;
              iState = 1;
            }
            else
            {
              bOn = false;
              bInitialised = true;
            }
            break;

          //WAIT
          case 1:
            if(bOn)
            {
              if(bInitPos)
              {
                iLastState = iState;
                iState = 2;
              }
              else if(bLiquidPresent and bGlassPresent)
              {
                iLastState = iState;
                iState = 3;
              }
            }        
            break;

          //POURING
          case 2:
            if(iPosGlass >= 30)
            {
              bMotorGlassUp = false;
            }
            else
            {
              bMotorGlassUp = true;
            }

            if(iPosLiquid >= iMaxHeightLiquid)
            {
              bMotorLiquidUp = false;
            }
            else if(iPosGlass >= 30 or iPosLiquid<= 40)
            {
              bMotorLiquidUp = true;
            }
            break;

          //PLACING
          case 3:
            if(bInitPos)
            {
              iLastState = iState;
              iState = 1;
            }
            else
            {
              if(not bLiquidDown)
              {
                bMotorLiquidDown = true;
              }
              if(not bGlassDown)
              {
                bMotorGlassDown = true;
              }         
            }
            break;

          //ERROR
          case 9:
            break;
        
        }
        break;

      
      //Manual Mode
      case 2:
        //Reset Motor
        bMotorLiquidUp = false;
        bMotorLiquidDown = false;
        bMotorGlassUp = false;
        bMotorGlassDown = false;
      
        //Movement when pushed
        bMotorLiquidUp = bManualLiquidUp and not (iPosLiquid >= iMaxHeightLiquid);
        bMotorLiquidDown = bManualLiquidDown and not bLiquidDown;
        bMotorGlassUp = bManualGlassUp and not (iPosGlass >= iMaxHeightGlass);
        bMotorGlassDown = bManualGlassDown and not bGlassDown;
        break;

      //Inspection Mode
      case 3:
      //Motor Liquid Up
      if(iPosLiquid >= iMaxHeightLiquid)
      {
        bMotorLiquidUp = false;
      }
      else if(bManualLiquidUp)
      {
        bMotorLiquidUp = true;
      }

      //Motor Liquid Down
      if(bLiquidDown)
      {
        bMotorLiquidDown = false;
      }
      else if(bManualLiquidDown)
      {
        bMotorLiquidDown = true;
      }

      //Motor Glass Up
      if(iPosGlass >= iMaxHeightGlass)
      {
        bMotorGlassUp = false;
      }
      else if(bManualGlassUp)
      {
        bMotorGlassUp = true;
      }

      //Motor Glass Down
      if(bGlassDown)
      {
        bMotorGlassDown = false;
      }
      else if(bManualGlassDown)
      {
        bMotorGlassDown = true;
      }
      break;  
    }



//Movement
  if(bMotorLiquidUp or bMotorLiquidDown)
  {
    switch(iActualStep)
    {
      case 1:
        if(bMotorLiquidUp)
        {
          ++ iActualStep;
        }
        else if(bMotorLiquidDown)
        {
          iActualStep = 8;
        }
        step1();
        delay(idelayMs);

      case 2:
        if(bMotorLiquidUp)
        {
          ++ iActualStep;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step2();
        delay(idelayMs);

      case 3:
        if(bMotorLiquidUp)
        {
          ++ iActualStep;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step3();
        delay(idelayMs);

      case 4:
        if(bMotorLiquidUp)
        {
          ++ iActualStep;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step4();
        delay(idelayMs);

      case 5:
        if(bMotorLiquidUp)
        {
          ++ iActualStep;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step5();
        delay(idelayMs);

      case 6:
        if(bMotorLiquidUp)
        {
          ++ iActualStep;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step6();
        delay(idelayMs);

      case 7:
        if(bMotorLiquidUp)
        {
          ++ iActualStep;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step7();
        delay(idelayMs);

      case 8:
        if(bMotorLiquidUp)
        {
          iActualStep = 1;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step8();
        delay(idelayMs);
    }
  }
/*
  if (iActualStep == 1) {
    if (bMotorLiquidUp) {
      ++ iActualStep;
    } else {
      iActualStep = 8;
    }
    step1();
    delay(idelayMs);
  } else if (iActualStep == 2) {
    if (bMotorLiquidUp) {
      ++ iActualStep;
    } else {
      -- iActualStep;
    }
    step2();
    delay(idelayMs);
  } else if (iActualStep == 3) {
    if (bMotorLiquidUp) {
      ++ iActualStep;
    } else {
      -- iActualStep;
    }
    step3();
    delay(idelayMs);
  } else if (iActualStep == 4) {
    if (bMotorLiquidUp) {
      ++ iActualStep;
    } else {
      -- iActualStep;
    }
    step4();
    delay(idelayMs);
  } else if (iActualStep == 5) {
    if (bMotorLiquidUp) {
      ++ iActualStep;
    } else {
      -- iActualStep;
    }
    step5();
    delay(idelayMs);
  } else if (iActualStep == 6) {
    if (bMotorLiquidUp) {
      ++ iActualStep;
    } else {
      -- iActualStep;
    }
    step6();
    delay(idelayMs);
  } else if (iActualStep == 7) {
    if (bMotorLiquidUp) {
      ++ iActualStep;
    } else {
      -- iActualStep;
    }
    step7();
    delay(idelayMs);
  } else if (iActualStep == 8) {
    if (bMotorLiquidUp) {
      iActualStep = 1;
    } else {
      -- iActualStep;
    }
    step8();
    delay(idelayMs);
  }
*/
}

void step1()  {
  digitalWrite(DIR11, HIGH);
  digitalWrite(DIR12, HIGH);
  analogWrite(PWM11, fPWMPbr);
  analogWrite(PWM12, fPWMPbr);
}

void step2()  {
  digitalWrite(DIR11, HIGH);
  digitalWrite(DIR12, LOW);
  analogWrite(PWM11, fPWMPbr);
  analogWrite(PWM12, LOW);
}

void step3() {
  digitalWrite(DIR11, HIGH);
  digitalWrite(DIR12, LOW);
  analogWrite(PWM11, fPWMPbr);
  analogWrite(PWM12, fPWMPbr);
}

void step4() {
  digitalWrite(DIR11, LOW);
  digitalWrite(DIR12, LOW);
  analogWrite(PWM11, LOW);
  analogWrite(PWM12, fPWMPbr);
}

void step5() {
  digitalWrite(DIR11, LOW);
  digitalWrite(DIR12, LOW);
  analogWrite(PWM11, fPWMPbr);
  analogWrite(PWM12, fPWMPbr);
}

void step6() {
  digitalWrite(DIR11, LOW);
  digitalWrite(DIR12, LOW);
  analogWrite(PWM11, fPWMPbr);
  analogWrite(PWM12, LOW);
}

void step7() {
  digitalWrite(DIR11, LOW);
  digitalWrite(DIR12, HIGH);
  analogWrite(PWM11, fPWMPbr);
  analogWrite(PWM12, fPWMPbr);
}

void step8() {
  digitalWrite(DIR11, LOW);
  digitalWrite(DIR12, HIGH);
  analogWrite(PWM11, LOW);
  analogWrite(PWM12, fPWMPbr);
}
