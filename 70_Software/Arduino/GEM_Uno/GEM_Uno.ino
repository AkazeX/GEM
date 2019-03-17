//Pin numbers
//IN-OUT
const int GlassUP = 7;        //Pin Number Command Glass Up
const int GlassDOWN = 8;      //Pin Number Command Glass Down
const int SensorGlassDown = 12;//Pin Number Sensor Glass Up
const int MaxPos = 13;        //Pin Number Singal max Positon reached

//Motor driver 1
const int PWM11 = 3;        //Pin Number PWM 1
const int PWM12 = 11;       //Pin Number PWM 2
const int DIR11 = 9;        //Pin Number DIR 1
const int DIR12 = 10;       //Pin Number DIR 2


/*******************************************/
/*******************************************/
//GlobalVariableList GVL
bool bMotorGlassUp = false;     //Command Motor drive glass up
bool bMotorGlassDown = false;   //Command Motor drive glass down
int iPosGlass = 0;              //Variabel current position

//Values
int iPosRatio = 1;               //Adjustment variable for positioning
int iCurrentStep = 1;            //current step
int iRatioSpeed = 180;           //speed value
float fPWMPbr = 0.15121 * iRatioSpeed + 50; //PWM signal based on speed value
float fDelay = (((1000 / ((iRatioSpeed / 360) * 100)) / 2) * 1000) / 2; //Delay based on speed value

//Constantes
const int iMaxHeightGlass = 200;  //maximum hight 

/*******************************************/
/*******************************************/
void setup() 
{
  //start serial communication
  Serial.begin(9600);

  //Declare Pin Mode IN/OUT
  pinMode(GlassUP, INPUT);
  pinMode(GlassDOWN, INPUT);
  pinMode(SensorGlassDown,INPUT_PULLUP);
  pinMode(MaxPos, OUTPUT);

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
/*******************************************/
//Read Inputs
/*******************************************/
//Command Motor Up
if(digitalRead(GlassUP))
{
  bMotorGlassUp = true;
}
else
{
  bMotorGlassUp = false;
}

//Command Motor Down
if(digitalRead(GlassDOWN))
{
  bMotorGlassDown = true;
}
else
{
  bMotorGlassDown = false;
}

//Limit switch glass down
if(not digitalRead(SensorGlassDown))
{
  iPosGlass = 0;
}

/*******************************************/
//Movement
/*******************************************/
   //if a movement command is active execute the step, 
   //change the position value and go to the next step
  if(bMotorGlassUp or bMotorGlassDown)
  {
    fPWMPbr = 0.15121 * iRatioSpeed + 50;
    switch(iCurrentStep)
    {
      case 1:
        if(bMotorGlassDown)
        {
          iCurrentStep = iCurrentStep + 2;
          iPosGlass = iPosGlass - iPosRatio ;
        }
        else if(bMotorGlassUp)
        {
          iCurrentStep = 7;
          iPosGlass = iPosGlass + iPosRatio ;
        }
        step1();
        rotationDelay(fDelay);
        break;

      case 3:
        if(bMotorGlassDown)
        {
          iCurrentStep = iCurrentStep + 2;
          iPosGlass = iPosGlass - iPosRatio ;
        }
        else if(bMotorGlassUp)
        {
          iCurrentStep = iCurrentStep - 2;
          iPosGlass = iPosGlass + iPosRatio ;
        }
        step3();
        rotationDelay(fDelay);
        break;

      case 5:
        if(bMotorGlassDown)
        {
          iCurrentStep = iCurrentStep + 2;
          iPosGlass = iPosGlass - iPosRatio ;
        }
        else if(bMotorGlassUp)
        {
          iCurrentStep = iCurrentStep - 2;
          iPosGlass = iPosGlass + iPosRatio ;
        }
        step5();
        rotationDelay(fDelay);
        break;


      case 7:
        if(bMotorGlassDown)
        {
          iCurrentStep = 1;
          iPosGlass = iPosGlass - iPosRatio ;
        }
        else if(bMotorGlassUp)
        {
          iCurrentStep = iCurrentStep - 2;
          iPosGlass = iPosGlass + iPosRatio ;
        }
        step7();
        rotationDelay(fDelay);
        break;

    }
    Serial.println(iPosGlass);
  }
  else if(iPosGlass >= iMaxHeightGlass)
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
//Write Outputs
/*******************************************/ 
if(iPosGlass >= iMaxHeightGlass)
{
  digitalWrite(MaxPos, HIGH);  
}
else
{
  digitalWrite(MaxPos, LOW);  
}

/*******************************************/
//Reseting
/*******************************************/ 
  //Reset Motor
  bMotorGlassUp = false;
  bMotorGlassDown = false;
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
