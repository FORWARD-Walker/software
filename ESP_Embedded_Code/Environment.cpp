#include "Environment.h"

Environment::Environment(Walker *pWalker)
{
    this->pWalker = pWalker;
}

void Environment::headlightSwitch()
{
     // photoresistor
     // this is the function I want to use, found in Photoresistor.h which is found in Walker.h
     this->pWalker->pPHOTO->readValue();
     // use Photoresistor->value to reference the value from the above function

    // headlight
    if(this->pWalker->pPhotoresistor->value < 2048)
    {
        digitalWrite(LIGHT, HIGH);
        Serial.print("headlight on");
    }
    else
    {
        digitalWrite(LIGHT, LOW);
        Serial.print("headlight off");
    }
}

void Environment::stabilityInclineDecline()
{
    if(this->pWalker->pImu->pitch > 3 && this->pWalker->pImu->pitch < 10 )
  {
    //print to network
    this->pWalker->pNetworking->pushSerialData("You are now going up an incline\n");
    
  }
  if(this->pWalker->pImu->pitch > 10)
  {
    //print to network
    this->pWalker->pNetworking->pushSerialData("You are going up a steep incline; be careful\n");
  }
  if(this->pWalker->pImu->pitch < -3 && this->pWalker->pImu->pitch < -10)
  {
    // print to network
    this->pWalker->pNetworking->pushSerialData("You are now going down a decline\n");
  }
  if(this->pWalker->pImu->pitch < -10)
  {
    // print to network
    this->pWalker->pNetworking->pushSerialData("You are going down a steep decline; be careful\n");
  }
}