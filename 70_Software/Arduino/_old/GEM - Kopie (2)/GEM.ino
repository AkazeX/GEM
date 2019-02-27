/*******************************************/
//Librarys
//#include "Nextion.h"
/*******************************************/
//Inputs and Ouputs
//Sensors
bool bGlassDown = false;
bool bLiquidDown = false;
bool bLiquidPresent = false;
bool bGlassPresent = false;

//Pin numbers
//Motor driver 1
const int PWM11 = 3;
const int PWM12 = 11; //12;
const int DIR11 = 12; //9;
const int DIR12 = 13; //10;

/*******************************************/
/*******************************************/
//HMI
//Communication
int incomingBytes[7];
int sendBytes[7];  
int emptyBytes[7]; 


//Operation
bool bHMIStart = false; 
bool bHMIStop = false;
bool bHMIInit  = false;
bool bHMIQuit  = false;

//Handmode
bool bManualLiquidUp = false;
bool bManualLiquidDown = false;
bool bManualGlassUp = false;
bool bManualGlassDown = false;

//Mode
bool bModeUp = false;
bool bModeDown = false;
int iMode = 0;
/* 0: Off
 * 1: Auto
 * 2: Manual
 * 3: Inspection
 */
 
/*******************************************/
/*******************************************/
//GlobalVariableList GVL
bool bAutOn = false;
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

//Values
int iMaxHeightLiquid = 100;
int iMaxHeightGlass = 100;
int iActualStep = 1;
int iRatioSpeed = 360;
float fPWMPbr = 40; //0.15121 * iRatioSpeed + 60;
float fDelay = 1000; //(((1000 / ((iRatioSpeed / 360) * 100)) / 2) * 1000) / 2;
int idelayMs = 1; 


/*******************************************/
/*******************************************/
//Nextion Object
//NexNumber nStep = NexNumber(3, 15, "Step");


/*******************************************/
/*******************************************/
void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(DIR11, OUTPUT);
  pinMode(DIR12, OUTPUT);
  pinMode(PWM11, OUTPUT);
  pinMode(PWM12, OUTPUT);

  TCCR2B = TCCR2B & 0b11111000 | 0x03;

  iMode = 2;
}

/*******************************************/
void loop() 
/*******************************************/
{

  //nStep.setValue(1);

/*******************************************/
//Communication Read
/*******************************************/  
  //Read
  if(Serial1.available() >= 7)
  {
    for(int i = 0; i < 7; i++)
    {
      if(Serial1.available() > 0)
      {
        incomingBytes[i] =  Serial1.read(); 
        Serial.println(incomingBytes[i], HEX);               
      }   
    }     
  }

  //Process
  if(incomingBytes[1]== 0) //Page Start
  {
    bModeUp   = (incomingBytes[2]==  23);
    bModeDown = (incomingBytes[2]==  24);
  }
  else if (incomingBytes[1]== 1) //Page Hand
  {
    //Checking ID
    bManualGlassUp     = (incomingBytes[2]==  7 && incomingBytes[3]==  1);
    bManualGlassDown   = (incomingBytes[2]==  8 && incomingBytes[3]==  1);
    bManualLiquidUp    = (incomingBytes[2]== 14 && incomingBytes[3]==  1);
    bManualLiquidDown  = (incomingBytes[2]== 15 && incomingBytes[3]==  1);
  }
  else if (incomingBytes[1]== 2) //Page Visu
  {
    ;
  }
  else if (incomingBytes[1]== 3) //Page Auto
  {
    bHMIStart = (incomingBytes[2]==  7);
    bHMIStop   = (incomingBytes[2]==  8);
    bHMIInit  = (incomingBytes[2]== 11);
  }

  //Reset received Data
  incomingBytes[0] = 0;
  incomingBytes[1] = 0;
  incomingBytes[2] = 0;
  incomingBytes[3] = 0;
  incomingBytes[4] = 0;
  incomingBytes[5] = 0;
  incomingBytes[6] = 0;


/*******************************************/
//Update States
/*******************************************/  
  //Mode
  if(bModeUp && iMode < 3)
  {
    ++iMode;
  }
  else if(bModeDown && iMode < 0)
  {
    --iMode;
  }

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

  //Init Position
  bInitPos = bGlassDown and bLiquidDown;


/*******************************************/
//Main Part
/*******************************************/    
    switch(iMode)
    {
/*******************************************/
//Off
/*******************************************/   
      case 0:
        //Reset Motor
        bMotorLiquidUp = false;
        bMotorLiquidDown = false;
        bMotorGlassUp = false;
        bMotorGlassDown = false;
        break;

      
/*******************************************/
//Automatic Mode
/*******************************************/
      case 1:

        //Reset Motor
        bMotorLiquidUp = false;
        bMotorLiquidDown = false;
        bMotorGlassUp = false;
        bMotorGlassDown = false;

          if(bHMIStart)
          {
            bAutOn = true;        
          }
          else if(bHMIStop)
          {
            bAutOn = false;
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
              bAutOn = false;
              bInitialised = true;
            }
            break;

          //WAIT
          case 1:
            if(bAutOn)
            {
              if(not bInitPos && bHMIInit)
              {
                iLastState = iState;
                iState = 3;
              }
              else if(bInitPos && bLiquidPresent && bGlassPresent)
              {
                iLastState = iState;
                iState = 2;
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



/*******************************************/
//Manual Mode
/*******************************************/
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


/*******************************************/
//Inspection Mode
/*******************************************/
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
     

/*******************************************/
//Movement
/*******************************************/
  bMotorLiquidUp = HIGH;  
  if(bMotorLiquidUp || bMotorLiquidDown)
  {
    switch(iActualStep)
    {
      case 1:
        if(bMotorLiquidUp)
        {
           ++iActualStep;
        }
        else if(bMotorLiquidDown)
        {
          iActualStep = 8;
        }
        step1();
        rotationDelay(fDelay);
        break;

      case 2:
        if(bMotorLiquidUp)
        {
           ++iActualStep;;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step2();
        rotationDelay(fDelay);
        break;

      case 3:
        if(bMotorLiquidUp)
        {
           ++iActualStep;;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step3();
        rotationDelay(fDelay);
        break;

      case 4:
        if(bMotorLiquidUp)
        {
           ++iActualStep;;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step4();
        rotationDelay(fDelay);
        break;

      case 5:
        if(bMotorLiquidUp)
        {
           ++iActualStep;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step5();
        rotationDelay(fDelay);
        break;

      case 6:
        if(bMotorLiquidUp)
        {
           ++iActualStep;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step6();
        rotationDelay(fDelay);
        break;

      case 7:
        if(bMotorLiquidUp)
        {
           ++iActualStep;
        }
        else if(bMotorLiquidDown)
        {
          -- iActualStep;
        }
        step7();
        rotationDelay(fDelay);
        break;

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
        rotationDelay(fDelay);
        break;
    }       
  }
  Serial.println(iActualStep); 
//  else
//  {
//    digitalWrite(DIR11, LOW);
//    digitalWrite(DIR12, LOW);
//    analogWrite(PWM11, LOW);
//    analogWrite(PWM12, LOW);
//  }

/*******************************************/
//Communication Write
/*******************************************/
  //Reset sending Data
  sendBytes[0] = 0x65;
  sendBytes[1] = 0x00;
  sendBytes[2] = 0x17;
  sendBytes[3] = 0x01;
  sendBytes[4] = 0xFF;
  sendBytes[5] = 0xFF;
  sendBytes[6] = 0xFF;
/*  
if(Serial1.availableForWrite()>=7)
{
  if(bManualGlassUp)
  {
    Serial.write(sendBytes[0]);
    Serial.write(sendBytes[1]);
    Serial.write(sendBytes[2]);
    Serial.write(sendBytes[3]);
    Serial.write(sendBytes[4]);
    Serial.write(sendBytes[5]);
    Serial.write(sendBytes[6]); 
  }
}

*/


/*******************************************/
//Reseting
/*******************************************/ 
  //Reset Motor
  bMotorLiquidUp = true;
  bMotorLiquidDown = false;
  
  //Reset HMI
  bModeUp         = false;
  bModeDown       = false;
//  bManualGlassUp      = false;
//  bManualGlassDown    = false;
//  bManualLiquidUp     = false;
//  bManualLiquidDown   = false;
  bHMIStart       = false;
  bHMIStop        = false;
  bHMIInit        = false;
}


/*******************************************/
//Funktions
/*******************************************/ 

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

void rotationDelay(float rDelay)
{
//  if(rDelay >5000)
//  {
//    delay(rDelay/1000);
//  }
//  else
//  {
//    delayMicroseconds(rDelay);
//  } 
  delay(idelayMs); 
}
