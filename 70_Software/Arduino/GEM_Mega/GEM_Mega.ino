/*******************************************/
//Librarys
//#include "Nextion.h"
/*******************************************/
//Inputs and Ouputs
//Sensors
bool bGlassDown = false;      //Sensor glass down
bool bLiquidDown = false;     //Sensor bottle down
bool bGlassPresent = false;   //Sensor glass present

//Pin numbers
//IN-OUT
const int SensorGlassDown = 2;       //Pin Number Sensor Glass Up
const int SensorLiquidDown = 3;      //Pin Number Sensor Liquid Up
const int SensorGlassPressent = 4; //Pin Number Sensor Glass Present
const int CommandGlassUp = 7;      //Pin Number Command Glass Up
const int CommandGlassDown = 8;    //Pin Number Command Glass Down
const int MaxPos = 13;             //Pin Number Singal max Positon reached

//Motor driver 1
const int PWM11 = 11;        //Pin Number PWM 1
const int PWM12 = 12;       //Pin Number PWM 2
const int DIR11 = 9;         //Pin Number DIR 1
const int DIR12 = 10;        //Pin Number DIR 2
/*******************************************/
/*******************************************/
//Operation
bool bHMIStart = false;            //HMI button start
bool bHMIStop = false;             //HMI button stop
bool bHMIInit  = false;            //HMI button init
bool bHMIQuit  = false;            //HMI button quit
bool bHMIEmergencyStop = false;    //HMI button emergency stop

//Handmode
bool bHMIManualLiquidUp = false;    //HMI button hand mode bottle up
bool bHMIManualLiquidDown = false;  //HMI button hand mode bottle down
bool bHMIManualGlassUp = false;     //HMI button hand mode glass up
bool bHMIManualGlassDown = false;   //HMI button hand mode glass down

//Mode
bool bHMIModeUp = false;    //HMI button mode up
bool bHMIModeDown = false;  //HMI button mode down
int iMode = 0;              //Current mode
/* 0: Off
 * 1: Auto
 * 2: Manual
 * 3: Inspection
 */
 
/*******************************************/
/*******************************************/
//GlobalVariableList GVL
bool bAutoOn = false;           //Automatic mode on
bool bInitPos = false;          //Initial positon active
bool bInitialised = false;      //Autoamtic mode is initialised
bool bEmergencyStop = false;    //Emergency stop is active
bool bSparkling = false;        //Bottle contains sparkling liquid
bool bBarm = false;             //Bottler conatins liquid with barn
bool bShakeCounted = false;     //Bottle shake is counted
bool bShakeDown = false;        //Bottle shaking marker

bool bMotorLiquidUp = false;    //Command motor liquid up
bool bMotorLiquidDown = false;  //Command motor liquid down
bool bMotorGlassUp = false;     //Command motor glass up
bool bMotorGlassDown = false;   //Command motor glas down

int iPosLiquid = 0;   //Positon of the bottle
int iPosGlass = 0;    //Position of the glass
int iShakes = 0;      //Times the bottle was shaked        
int iState = 0;       //Current state/step
int iLastState = 0;   //Last state/step
/* 0: INIT
 * 1: WAIT
 * 2: POURING
 * 3: PLACING
 * 9: ERROR
 */


//Communication
bool bComGlasDown = false;      //State of the HMI object: glas down
bool bComGlasPresent = false;   //State of the HMI object: glas present
bool bComLiquidDown = false;    //State of the HMI object: liquid down

int iComMode = 0;               //State of the HMI object: Mode     
int iComState = 0;              //State of the HMI object: State
int incomingBytes[7];           //Communication Bytes to read
String str = "";                //string variable


//Values
int iPosRatio = 1;               //Adjustment variable for positioning
int iCurrentStep = 1;            //current step
int iRatioSpeed = 180;           //speed value
float fPWMPbr = 0.15121 * iRatioSpeed + 50; //PWM signal based on speed value
float fDelay = (((1000 / ((iRatioSpeed / 360) * 100)) / 2) * 1000) / 2; //Delay based on speed value


//Constantes
const int cPosGlass = 240;          //Constantes Positoin 1 of the glass
const int cPosLiquid1 = 125;        //Constantes Positoin 1 of the bottle
const int cPosLiquid2 = 315;        //Constantes Positoin 2 of the bottle
const int cPosLiquid3 = 500;        //Constantes Positoin 3 of the bottle
const int cDeltaPosShake = 30;      //Constantes Positoin difference to shake
const int iMaxHeightLiquid = 500 ;   //Constantes maximum hight of the bootle
const int iMaxHeightGlass = cPosGlass;    //Constantes maximum hight of the glass
/*******************************************/
/*******************************************/
//Nextion Object
//NexNumber nStep = NexNumber(3, 15, "Step");


/*******************************************/
/*******************************************/
void setup() 
{
  //start serial communication 0
  Serial.begin(9600);
  //start serial communication 1
  Serial1.begin(9600);
  
  //Declare Pin Mode IN/OUT
  pinMode(SensorGlassDown,INPUT_PULLUP);
  pinMode(SensorLiquidDown,INPUT_PULLUP);
  pinMode(SensorGlassPressent,INPUT_PULLUP);
  pinMode(MaxPos, INPUT_PULLUP);
  pinMode(CommandGlassUp, OUTPUT);
  pinMode(CommandGlassDown, OUTPUT);
  
  //Declare Pin Mode PWM
  pinMode(DIR11, OUTPUT);
  pinMode(DIR12, OUTPUT);
  pinMode(PWM11, OUTPUT);
  pinMode(PWM12, OUTPUT);
  
//Change pin frequency of the PWM pins
  TCCR2B = TCCR2B & 0b11111000 | 0x03;

}


/*******************************************/
//Main Loop
/*******************************************/
void loop() 
{
  Serial.println(iPosLiquid);
/*******************************************/
//Read Inputs
/*******************************************/  
//Limit switch glass down
if(digitalRead(SensorGlassDown))
{
  bGlassDown = false;
}
else
{
  bGlassDown = true;
}

//Limit switch liquid down
if(digitalRead(SensorLiquidDown))
{
  bLiquidDown = false;
}
else
{
  bLiquidDown = true;
}

//Switch glass present
if(digitalRead(SensorGlassPressent))
{
  bGlassPresent = false;
}
else
{
  bGlassPresent = true;
}

//Positon glass
if(digitalRead(MaxPos))
{
  iPosGlass = cPosGlass;
}
/*******************************************/
//Debug Channel
/*******************************************/  
//if there is data in the buffer print it to serial com 0
if(Serial.available() > 0)
{
  Serial.println(String(Serial.read()));
}
  

/*******************************************/
//Communication Read
/*******************************************/  
  //if there are 7 bytes in the buffer read them
  if(Serial1.available() >= 7)
  {
    //Copy them to the incomingBytes Array
    for(int i = 0; i < 7; i++)
    {
      if(Serial1.available() > 0)
      {
        incomingBytes[i] =  Serial1.read(); 
        //Serial.println(incomingBytes[i], HEX);
                    
      }   
    }        
  }
/*******************************************/
//Communication Process
/*******************************************/
//Evaluate the Array
//incomingBytes[1] = page index  
//incomingBytes[2] = object id  
//incomingBytes[3] = state of the object
  
  if(incomingBytes[1]== 0) //Page Start
  {
    //Checking ID
    bHMIModeUp   = (incomingBytes[2]==  23);
    bHMIModeDown = (incomingBytes[2]==  24);
    bHMIQuit  = (incomingBytes[2]==  26);
    bHMIEmergencyStop = (incomingBytes[2]==  27);    
  }
  else if (incomingBytes[1]== 1) //Page Hand
  {
    //Checking ID
    bHMIManualGlassUp     = (incomingBytes[2]==  7 && incomingBytes[3]==  1);
    bHMIManualGlassDown   = (incomingBytes[2]==  8 && incomingBytes[3]==  1);
    bHMIManualLiquidUp    = (incomingBytes[2]== 11 && incomingBytes[3]==  1);
    bHMIManualLiquidDown  = (incomingBytes[2]== 12 && incomingBytes[3]==  1);
    bHMIQuit           = (incomingBytes[2]==  15);
    bHMIEmergencyStop  = (incomingBytes[2]==  16);
  }
  else if (incomingBytes[1]== 2) //Page Beverage
  {
    //Checking ID
    if(incomingBytes[2]== 16 || incomingBytes[2]== 17 || incomingBytes[2]== 19)
    {
      bSparkling = false;
      bBarm = false;
    }
    else if(incomingBytes[2]== 14 || incomingBytes[2]== 15 || incomingBytes[2]== 20 || incomingBytes[2]== 21)
    {
      bSparkling = true;
      bBarm = false;
    }
    else if(incomingBytes[2]== 18 || incomingBytes[2]== 22)
    {
      bSparkling = true;
      bBarm = true;
    }
    else if(incomingBytes[2]== 8)
    {
      bSparkling = not bSparkling;      
    }
    else if (incomingBytes[2]== 11)
    {
      bBarm = not bBarm;
    }
        
    bHMIQuit          = (incomingBytes[2]==  12);
    bHMIEmergencyStop = (incomingBytes[2]==  13);
    ;
  }
  else if (incomingBytes[1]== 3) //Page Auto
  {
    //Checking ID
    bHMIStart = (incomingBytes[2]==  7);
    bHMIStop  = (incomingBytes[2]==  8);
    bHMIInit  = (incomingBytes[2]== 11);
    bHMIQuit  = (incomingBytes[2]== 19);
    bHMIEmergencyStop = (incomingBytes[2]==  20);
  }

  //Reset received Data after the variables are set
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
  if(bHMIModeUp && iMode < 3)
  {
    ++iMode;
  }
  else if(bHMIModeDown && iMode > 0)
  {
    --iMode;
     
  }

  //Position Glas is 0 when the limit switch is reached
  if(bGlassDown)
  {
    iPosGlass = 0;
  }

  //Positon Liquid is 0 when the limit switch is reached
  if(bLiquidDown)
  {
    iPosLiquid = 0;
  }

  //Init Position when both limit switches are active
  bInitPos = bGlassDown and bLiquidDown;

  //Auto Online active when the mode value is 1 and the start button is pressed
  if(bHMIStart && iMode == 1)
  {
    bAutoOn = true;        
  }
  //if the stop button is pressed change reset the variable
  else if(bHMIStop)
  {
    bAutoOn = false;
  }
   //if the Mode is changed reset the variable
   else if(iMode != 1)
  {
    bAutoOn = false;
  }
 

          
  //if the emrgency stop button is pressed set the marker bEmergencyStop
  if(bHMIEmergencyStop)
  {
    bEmergencyStop = true;    
  }

  //If the marker bEmergencyStop is active reset the system
  if(bEmergencyStop)
  {
    iMode = 0;
    bMotorLiquidUp = false;
    bMotorLiquidDown = false;
    bMotorGlassUp = false;
    bMotorGlassDown = false;
    bInitialised = false;
  }

  //if the quit button is pressed initialise the system
  if(bHMIQuit)
  {
    iMode = 0;    
    iState = 0;
    bInitialised = false;
    bEmergencyStop = false;    
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
        //Reset Variables
        bMotorLiquidUp = false;
        bMotorLiquidDown = false;
        bMotorGlassUp = false;
        bMotorGlassDown = false;
        bAutoOn = false;
        iState = 0;
        break;

      
/*******************************************/
//Automatic Mode
/*******************************************/
      case 1:

        //Reset Motor in advance to make sure they are always turned off befor entering the switch(iSate)
        bMotorLiquidUp = false;
        bMotorLiquidDown = false;
        bMotorGlassUp = false;
        bMotorGlassDown = false;         



        switch(iState)
        {
          //INIT
          case 0:
          //Initialise the Process
            //When initialised advance
            if(bInitialised)
            {
              iLastState = iState;
              iState = 1;
            }
            //Reset the variables
            else
            {
              bAutoOn = false;
              iShakes = 0;
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
          //If the Galss is in Place, advance
            if(bGlassPresent)
            {
              //Wait 3 seconds
              delay(3000);
              iLastState = iState;
              iState = 3;
            }
            break;            

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
              if(bBarm)
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
          iRatioSpeed = 340;
            //If the Liquid reaches the Position, change Motordirection
            if(iPosLiquid <= (cPosLiquid1 + cDeltaPosShake) and not bShakeDown)
            {
              bMotorLiquidUp = true;
              bMotorLiquidDown = false;                           
            }
            //if the upper positionis reached set marker
            else if(iPosLiquid >= (cPosLiquid1 + cDeltaPosShake))
            {
              bShakeDown = true;
              bShakeCounted = false;              
            }
            
            //If the Liquid reaches the Position, change Motordirection
            if(iPosLiquid >= (cPosLiquid1 - cDeltaPosShake)and bShakeDown)
            {
              bMotorLiquidUp = false;
              bMotorLiquidDown = true;
            }
            //if the lower positionis reached set marker
            else if (iPosLiquid <= (cPosLiquid1 - cDeltaPosShake))            
            {
              bShakeDown = true;
              bShakeCounted = true;
              ++iShakes ;              
            }

             //After 5 Times advance
             if(iShakes == 5)
             {
              bShakeDown = false;
              iShakes = 0;
              iRatioSpeed = 180;
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
            if(bBarm)
            {
              delay(2000);
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
            if(bLiquidDown)
            {
              bMotorLiquidDown = false;
            }

            //If the Glass reached the Position, stop the Motor
            if(bGlassDown)
            {
              bMotorGlassDown = false;
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
            if(not bGlassPresent)
            {
              iLastState = iState;
              iState = 1; 
            }
            break; 
        }
        break;



/*******************************************/
//Manual Mode
/*******************************************/
      case 2:
        //Reset Motor in advance to make sure they are always turned off
        bMotorLiquidUp = false;
        bMotorLiquidDown = false;
        bMotorGlassUp = false;
        bMotorGlassDown = false;
      
        //Movement when pushed
        bMotorLiquidUp = bHMIManualLiquidUp and not (iPosLiquid >= iMaxHeightLiquid);
        bMotorLiquidDown = bHMIManualLiquidDown and not bLiquidDown;
        bMotorGlassUp = bHMIManualGlassUp and not (iPosGlass >= iMaxHeightGlass);
        bMotorGlassDown = bHMIManualGlassDown and not bGlassDown;
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
      else if(bHMIManualLiquidUp)
      {
        bMotorLiquidUp = true;
      }

      //Motor Liquid Down
      if(bLiquidDown)
      {
        bMotorLiquidDown = false;
      }
      else if(bHMIManualLiquidDown)
      {
        bMotorLiquidDown = true;
      }

      //Motor Glass Up
      if(iPosGlass >= iMaxHeightGlass)
      {
        bMotorGlassUp = false;
      }
      else if(bHMIManualGlassUp)
      {
        bMotorGlassUp = true;
      }

      //Motor Glass Down
      if(bGlassDown)
      {
        bMotorGlassDown = false;
      }
      else if(bHMIManualGlassDown)
      {
        bMotorGlassDown = true;
      }
      break;  
    }
     

/*******************************************/
//Movement
/*******************************************/
  //if a movement command is active execute the step, 
   //change the position value and go to the next step
  if(bMotorLiquidUp or bMotorLiquidDown)
  {
    fPWMPbr = 0.15121 * iRatioSpeed + 50;
    switch(iCurrentStep)
    {
      case 1:
        if(bMotorLiquidDown)
        {
          iCurrentStep = iCurrentStep + 2;
          iPosLiquid = iPosLiquid - iPosRatio ;
        }
        else if(bMotorLiquidUp)
        {
          iCurrentStep = 7;
          iPosLiquid = iPosLiquid + iPosRatio ;
        }
        step1();
        rotationDelay(fDelay);
        break;

      case 3:
        if(bMotorLiquidDown)
        {
          iCurrentStep = iCurrentStep + 2;
          iPosLiquid = iPosLiquid - iPosRatio ;
        }
        else if(bMotorLiquidUp)
        {
          iCurrentStep = iCurrentStep - 2;
          iPosLiquid = iPosLiquid + iPosRatio ;
        }
        step3();
        rotationDelay(fDelay);
        break;

      case 5:
        if(bMotorLiquidDown)
        {
          iCurrentStep = iCurrentStep + 2;
          iPosLiquid = iPosLiquid - iPosRatio ;
        }
        else if(bMotorLiquidUp)
        {
          iCurrentStep = iCurrentStep - 2;
          iPosLiquid = iPosLiquid + iPosRatio ;
        }
        step5();
        rotationDelay(fDelay);
        break;


      case 7:
        if(bMotorLiquidDown)
        {
          iCurrentStep = 1;
          iPosLiquid = iPosLiquid - iPosRatio ;
        }
        else if(bMotorLiquidUp)
        {
          iCurrentStep = iCurrentStep - 2;
         iPosLiquid = iPosLiquid + iPosRatio ;
        }
        step7();
        rotationDelay(fDelay);
        break;
    }
  }
  else if(iPosLiquid >= cPosLiquid3)
  {
    fPWMPbr = 30;
    //Call last currentstep again to hold it on the position
    switch(iCurrentStep)
    {
      case 1:        
        step1();
        break;

      case 3:
        step3();
        break;

      case 5:
        step5();
        break;


      case 7:
        step7();
        break;
    }    
  }
  else
  {
    digitalWrite(DIR11, LOW);
    digitalWrite(DIR12, LOW);
    digitalWrite(PWM11, LOW);
    digitalWrite(PWM12, LOW);
  }
  

/*******************************************/
//Communication Write
/*******************************************/
//When there is enough place to write a telegram
if(Serial1.availableForWrite()>=16)
{
  //Check if the plc stat and the HMI object state are different, 
  //sende a telegramm to update the object state if there is a difference
  if(bGlassDown != bComGlasDown)
  {
    if(bGlassDown)
    {
      Serial1.write("StartPict.GlasDown.bco=2016");
      bComGlasDown = true;
    }
    else
    {
      Serial1.write("StartPict.GlasDown.bco=48631");
      bComGlasDown = false;
    }
  }

  //Check if the plc stat and the HMI object state are different, 
  //sende a telegramm to update the object state if there is a difference
  if(bGlassPresent != bComGlasPresent)
  {
    if(bGlassPresent)
    {
      Serial1.write("StartPict.GlasPresent.bco=2016");
      bComGlasPresent = true;
    }
    else
    {
      Serial1.write("StartPict.GlasPresent.bco=48631");
      bComGlasPresent = false;
    }
  }

  //Check if the plc stat and the HMI object state are different, 
  //sende a telegramm to update the object state if there is a difference
  if(bLiquidDown != bComLiquidDown)
  {
    if(bLiquidDown)
    {
      Serial1.write("StartPict.BottleDown.bco=2016");
      bComLiquidDown = true;
    }
    else
    {
      Serial1.write("StartPict.BottleDown.bco=48631");
      bComLiquidDown = false;
    }
  }


  //Check if the plc stat and the HMI object state are different, 
  //sende a telegramm to update the object state if there is a difference
  if(iMode != iComMode)
  {
    //Serial1.write("StartPict.Mode.val="+String(iMode));
    iComMode = iMode;    
  }

  //Check if the plc stat and the HMI object state are different, 
  //sende a telegramm to update the object state if there is a difference
  if(iState != iComState)
  {
    //Serial1.write("AutoPict.Step.val="+String(iState));
    //Serial1.write("AutoPict.LastStep.val="+String(iLastState));
    iComState = iState;    
  }  
}


/*******************************************/
//Write Outputs
/*******************************************/ 
//write Motor Glass up
if(bMotorGlassUp)
{
  digitalWrite(CommandGlassUp, HIGH);  
}
else
{
  digitalWrite(CommandGlassUp, LOW);  
}

//write Motor Glass down
if(bMotorGlassDown)
{
  digitalWrite(CommandGlassDown, HIGH);  
}
else
{
  digitalWrite(CommandGlassDown, LOW);  
}

/*******************************************/
//Reseting
/*******************************************/ 
  //Reset HMI buttons (only one cycle active)
  bHMIModeUp         = false;
  bHMIModeDown       = false;
  bHMIStart       = false;
  bHMIStop        = false;
  bHMIInit        = false;
  bHMIQuit        = false;
  bHMIEmergencyStop = false;
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
