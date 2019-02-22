//Pin numbers
//Commands
const int GlassUP = 7;
const int GlassDOWN = 8;

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
int iMaxHeightLiquid = 100;
int iMaxHeightGlass = 100;
int iActualStep = 1;
int iRatioSpeed = 360;
float fPWMPbr = 40; //0.15121 * iRatioSpeed + 60;
float fDelay = 1000; //(((1000 / ((iRatioSpeed / 360) * 100)) / 2) * 1000) / 2;

/*******************************************/
/*******************************************/
void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(GlassUP, INPUT);
  pinMode(GlassDOWN, INPUT);
  
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
        }
        else if(bMotorGlassUp)
        {
          iActualStep = 7;
        }
        step1();
        rotationDelay(fDelay);
        break;

      case 2:
        if(bMotorGlassDown)
        {
          ++ iActualStep;
        }
        else if(bMotorGlassUp)
        {
          -- iActualStep;
        }
        step2();
        delayMicroseconds(fDelay);
        break;

      case 3:
        if(bMotorGlassDown)
        {
          iActualStep = iActualStep + 2;
        }
        else if(bMotorGlassUp)
        {
          iActualStep = iActualStep - 2;
        }
        step3();
        rotationDelay(fDelay);
        break;

      case 4:
        if(bMotorGlassDown)
        {
          ++ iActualStep;
        }
        else if(bMotorGlassUp)
        {
          -- iActualStep;
        }
        step4();
        delayMicroseconds(fDelay);
        break;

      case 5:
        if(bMotorGlassDown)
        {
          iActualStep = iActualStep + 2;
        }
        else if(bMotorGlassUp)
        {
          iActualStep = iActualStep - 2;
        }
        step5();
        rotationDelay(fDelay);
        break;

      case 6:
        if(bMotorGlassDown)
        {
          ++ iActualStep;
        }
        else if(bMotorGlassUp)
        {
          -- iActualStep;
        }
        step6();
        delayMicroseconds(fDelay);
        break;

      case 7:
        if(bMotorGlassDown)
        {
          iActualStep = 1;
        }
        else if(bMotorGlassUp)
        {
          iActualStep = iActualStep - 2;
        }
        step7();
        rotationDelay(fDelay);
        break;

      case 8:
        if(bMotorGlassDown)
        {
          iActualStep = 1;
        }
        else if(bMotorGlassUp)
        {
          -- iActualStep;
        }
        step8();
        delayMicroseconds(fDelay);
        break;
    }
    Serial.println(iActualStep);
  }
  else
  {
    digitalWrite(DIR11, LOW);
    digitalWrite(DIR12, LOW);
    digitalWrite(PWM11, LOW);
    digitalWrite(PWM12, LOW);
    
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
