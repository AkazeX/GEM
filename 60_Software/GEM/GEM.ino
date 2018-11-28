/*******************************************/
/*******************************************/
//Inputs and Ouputs
//Operation
bool bStart = false;
bool bStopp = false;
bool bQuit = false;

//Command
bool bMotorLiquidUp = false;
bool bMotorLiquidDown = false;
bool bMotorGlassUp = false;
bool bMotorGlassDown = false;

//Handmode
bool bManualLiquidUp = false;
bool bManualLiquidDown = false;
bool bManualGlassUp = false;
bool bManualGlassDown = false;
//Sensore
bool bGlassDown = false;
bool bLiquidDown = false;
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

}

/*******************************************/
/*******************************************/
void loop() 
{
  //Update States
  bInitPos = bGlassDown and bLiquidDown;
  if(bGlassDown)
  {
    iPosGlass = 0;
  }
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
      else
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
            else
            {
              iLastState = iState;
              iState = 3;
            }
          }        
          break;

        //POURING
        case 2:
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
            bMotorLiquidDown = true;
            bMotorGlassDown = true;
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



  
}
