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
const int PWM11 = 11;
const int PWM12 = 12;
const int DIR11 = 9;
const int DIR12 = 10;

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
bool bAutoOn = false;
bool bInitPos = false;
bool bInitialised = false;
bool bError = false;
bool bBarn = false;

bool bMotorLiquidUp = false;
bool bMotorLiquidDown = false;
bool bMotorGlassUp = false;
bool bMotorGlassDown = false;

int iPosLiquid = 0;
int iPosGlass = 0;
int iShakes = 0;
int iState = 0; 
int iLastState = 0;
/* 0: INIT
 * 1: WAIT
 * 2: POURING
 * 3: PLACING
 * 9: ERROR
 */

//Values
int iPosRatio = 1;
int iActualStep = 1;
int iRatioSpeed = 360;
float fPWMPbr = 0.15121 * iRatioSpeed + 60;
float fDelay = (((1000 / ((iRatioSpeed / 360) * 100)) / 2) * 1000) / 2;

//Constantes
const int cPosGlass = 250;
const int cPosLiquid1 = 240;
const int cPosLiquid2 = 280;
const int cPosLiquid3 = 580;
const int cDeltaPosShake = 20;
const int iMaxHeightLiquid = 620;
const int iMaxHeightGlass = 120;
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

  //iMode = 2;
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

  //Auto Online
  if(bHMIStart && iMode == 1)
          {
            bAutoOn = true;        
          }
          else
          {
            bAutoOn = false;
          }
          
          if(bHMIStop && iMode == 1)
          {
            bAutoOn = false;
          }
          else
          {
            bAutoOn = false;
          }


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



        switch(iState)
        {
          //INIT
          case 0:
          //Initialise the Process
            if(bInitialised)
            {
              iLastState = iState;
              iState = 1;
            }
            else
            {
              bAutoOn = false;
              bInitialised = true;
            }
            break;

          //StandBy
          case 1:
          //Automation Mode is On and Started
            if(bAutoOn)
            {
              //Drives to Init Position on Command
              if(not bInitPos && bHMIInit)
              {
                iLastState = iState;
                iState = 9;
              }
              //Goes to Next Step if it is on the Init Position
              else if(bInitPos)
              {
                iLastState = iState;
                iState = 2;
              }
            }        
            break;

          //Wait for Placing
          case 2:
          //If the Galss and Liquid is in Place, advance
            if(bGlassDown && bLiquidDown)
            {
              iLastState = iState;
              iState = 3;
            }            

          //Lift to Start Position
          case 3:
          //Drive Glass and Liquid to Positon
            bMotorLiquidUp = true;
            bMotorGlassUp = true;

           //If the Glass reached the Position, stop the Motor
            if(iPosGlass >= cPosGlass)
            {
              bMotorGlassUp = false;
            }

            //If the Liquid reached the Position, stop the Motor
            if(iPosLiquid >= cPosLiquid2)
            {
              bMotorLiquidUp = false;
            }

            //If the Glass and Liquid reached the Positions, advance
            if(iPosGlass >= cPosGlass && iPosLiquid >= cPosLiquid2)
            {
              iLastState = iState;
              iState = 4;
            }            
            break;

          //Start Pouring
          case 4:
          //Drive Glass and Liquid to Positon
            bMotorLiquidUp = true;

            //If the Liquid reached the Position, stop the Motor
            if(iPosLiquid >= cPosLiquid3 )
            {
              bMotorLiquidUp = false;

              //Considering the Barn advance
              if(bBarn)
              {
                delay(4000);
                iLastState = iState;
                iState = 5;
              }
              else
              {
                iLastState = iState;
                iState = 8;
              }
            }
            break;
            
          //Lower Bottle
          case 5:
          //Drive Glass and Liquid to Positon
            bMotorLiquidDown = true;

            //If the Liquid reaches the Position, stop the Motor
            if(iPosLiquid <= cPosLiquid1)
            {
              bMotorLiquidDown = false;
              iShakes = 0;
              iLastState = iState;
              iState = 6;              
            }
            break;

          //Shake Bottle
          case 6:
            //If the Liquid reaches the Position, change Motordirection
            if(iPosLiquid <= (cPosLiquid1 - cDeltaPosShake))
            {
              bMotorLiquidUp = true;
              bMotorLiquidDown = false;                           
            }
            //If the Liquid reaches the Position, change Motordirection
            else if(iPosLiquid >= (cPosLiquid1 + cDeltaPosShake))
            {
              bMotorLiquidUp = false;
              bMotorLiquidDown = true;
              ++iShakes ;                          
            }

            //After 5 Times advance
             if(iShakes == 5)
             {
              iLastState = iState;
              iState = 7;               
             }
             break;

           //Continue Pouring
          case 7:
            //Drive GLiquid to Positon
            bMotorLiquidUp = true;

            //If the Liquid reached the Position, stop the Motor
            if(iPosLiquid >= cPosLiquid3 )
            {
              bMotorLiquidUp = false;
              iLastState = iState;
              iState = 8;  
            }
            break;

          //Empty the Bottle
          case 8:
            //Wait a certain time and advance
            if(bBarn)
            {
              delay(1000);
              bMotorLiquidUp = false;
              iLastState = iState;
              iState = 9;
            }
            else
            {
              delay(5000);
              bMotorLiquidUp = false;
              iLastState = iState;
              iState = 9;
            }             
            break;

          //Lower to Init Position
          case 9:
          //Drive Glass and Liquid to Positon
            bMotorGlassDown = true;
            bMotorLiquidDown = true;

            //If the Liquid reached the Position, stop the Motor
            if(iPosLiquid == 0 || bLiquidDown)
            {
              bMotorLiquidDown = false;
            }

            //If the Glass reached the Position, stop the Motor
            if(iPosGlass == 0 || bGlassDown)
            {
              bMotorLiquidDown = false;
            }

            //If Init Position advance
            if(bInitPos)
            {
              iLastState = iState;
              iState = 10;              
            }
            break;

          //Remove Glass and Liquid
          case 10:
          //If bothe Objects are removed advance
            if(bLiquidPresent && bGlassPresent)
            {
              iLastState = iState;
              iState = 1; 
            }
            break;     
            
          //ERROR
          case 20:
          iLastState = iState;
              iState = 1;
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
        bMotorLiquidDown = bManualLiquidDown and not (bLiquidDown || iPosLiquid <= 0);
        bMotorGlassUp = bManualGlassUp and not (iPosGlass >= iMaxHeightGlass);
        bMotorGlassDown = bManualGlassDown and not (bGlassDown || iPosGlass <= 0);
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
   
  if(bMotorLiquidUp or bMotorLiquidDown)
  {
    switch(iActualStep)
    {
      case 1:
        if(bMotorLiquidDown)
        {
          iActualStep = iActualStep + 2;
          iPosLiquid = iPosLiquid - iPosRatio ;
        }
        else if(bMotorLiquidUp)
        {
          iActualStep = 7;
          ++iPosLiquid;
        }
        step1();
        rotationDelay(fDelay);
        break;

      case 3:
        if(bMotorLiquidDown)
        {
          iActualStep = iActualStep + 2;
          --iPosLiquid;
        }
        else if(bMotorLiquidUp)
        {
          iActualStep = iActualStep - 2;
          ++iPosLiquid;
        }
        step3();
        rotationDelay(fDelay);
        break;

      case 5:
        if(bMotorLiquidDown)
        {
          iActualStep = iActualStep + 2;
          --iPosLiquid;
        }
        else if(bMotorLiquidUp)
        {
          iActualStep = iActualStep - 2;
          ++iPosLiquid;
        }
        step5();
        rotationDelay(fDelay);
        break;


      case 7:
        if(bMotorLiquidDown)
        {
          iActualStep = 1;
          --iPosLiquid;
        }
        else if(bMotorLiquidUp)
        {
          iActualStep = iActualStep - 2;
          ++iPosLiquid;
        }
        step7();
        rotationDelay(fDelay);
        break;

    }
    Serial.println(iPosLiquid);
  }
  else
  {
//    digitalWrite(DIR11, LOW);
//    digitalWrite(DIR12, LOW);
//    digitalWrite(PWM11, LOW);
//    digitalWrite(PWM12, LOW);
    
  }

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
  bMotorLiquidUp = false;
  bMotorLiquidDown = false;
  bMotorGlassUp = false;
  bMotorGlassDown = false;
  
  //Reset HMI
  bModeUp         = false;
  bModeDown       = false;
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
  if(rDelay >5000)
  {
    delay(rDelay/1000);
  }
  else
  {
    delayMicroseconds(rDelay);
  }  
}
