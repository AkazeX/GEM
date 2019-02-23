//Pin numbers
//Commands
const int GlassUP = 7;
const int GlassDOWN = 8;
const int MaxPos = 13;

//Motor driver 1
const int PWM11 = 3;
const int PWM12 = 11;
const int DIR11 = 9;
const int DIR12 = 10;


/*******************************************/
/*******************************************/
//GlobalVariableList GVL
bool bMotorGlassUp = false;
bool bMotorGlassDown = false;
int iPosGlass = 0;

//Values
int iPosRatio = 1;
int iActualStep = 1;
int iRatioSpeed = 360;
float fPWMPbr = 0.15121 * iRatioSpeed + 60;
float fDelay = (((1000 / ((iRatioSpeed / 360) * 100)) / 2) * 1000) / 2;

//Constantes
const int iMaxHeightGlass = 120;

/*******************************************/
/*******************************************/
void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(GlassUP, INPUT);
  pinMode(GlassDOWN, INPUT);
  pinMode(MaxPos, OUTPUT);
  
  pinMode(DIR11, OUTPUT);
  pinMode(DIR12, OUTPUT);
  pinMode(PWM11, OUTPUT);
  pinMode(PWM12, OUTPUT);

  TCCR2B = TCCR2B & 0b11111000 | 0x03;

}

/*******************************************/
void loop() 
/*******************************************/
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
 
/*******************************************/
//Movement
/*******************************************/
   
  if(bMotorGlassUp or bMotorGlassDown)
  {
    switch(iActualStep)
    {
      case 1:
        if(bMotorGlassDown)
        {
          iActualStep = iActualStep + 2;
          iPosGlass = iPosGlass - iPosRatio ;
        }
        else if(bMotorGlassUp)
        {
          iActualStep = 7;
          ++iPosGlass;
        }
        step1();
        rotationDelay(fDelay);
        break;

      case 3:
        if(bMotorGlassDown)
        {
          iActualStep = iActualStep + 2;
          --iPosGlass;
        }
        else if(bMotorGlassUp)
        {
          iActualStep = iActualStep - 2;
          ++iPosGlass;
        }
        step3();
        rotationDelay(fDelay);
        break;

      case 5:
        if(bMotorGlassDown)
        {
          iActualStep = iActualStep + 2;
          --iPosGlass;
        }
        else if(bMotorGlassUp)
        {
          iActualStep = iActualStep - 2;
          ++iPosGlass;
        }
        step5();
        rotationDelay(fDelay);
        break;


      case 7:
        if(bMotorGlassDown)
        {
          iActualStep = 1;
          --iPosGlass;
        }
        else if(bMotorGlassUp)
        {
          iActualStep = iActualStep - 2;
          ++iPosGlass;
        }
        step7();
        rotationDelay(fDelay);
        break;

    }
    Serial.println(iPosGlass);
  }
  else
  {
//    digitalWrite(DIR11, LOW);
//    digitalWrite(DIR12, LOW);
//    digitalWrite(PWM11, LOW);
//    digitalWrite(PWM12, LOW);
    
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
