//Inputs and Ouputs
bool bStart = false;
bool bStopp = false;
bool bGlassDown = false;
bool bBottleDown = false;

//GlobalVariableList GVL
bool bInitPos = false;
bool bInitalised = false;
bool bError = false;
int iMode = 0;
/* 0: Off
 * 1: Auto
 * 2: Manual
 * 3: Inspection
 */
int iState = 0; 
int iLastState = 0;
/* 0: INIT
 * 1: WAIT
 * 2: POURING
 * 3: PLACING
 * 9: ERROR
 */



void setup() {
  // put your setup code here, to run once:

}

void loop() 
{
  //Update States
  bInitPos = bGlassDown and bBottleDown;
  //Check the Operating Mode
  switch(iMode)
  {
    //Off
    case 0:


      break;

      
     //Automatic Mode 
    case 1:
      switch(iState)
      {
        //INIT
        case 0:
          if(bError)
          {
            iLastState = iState;
            iState = 9;
          }
          else if(bInitalised)
          {
            iLastState = iState;
            iState = 1;
          }
          else
          {
            
          }
          break;

        //WAIT
        case 1:
          break;

        //POURING
        case 2:
          break;

        //PLACING
        case 3:
          break;

        //ERROR
        case 9:
          break;
        
      }
       


      break;

      
    //Manual Mode
    case 2:


      break;

    //Inspection Mode
    case 3:


      break;
  
  }




//Movement





  
}
