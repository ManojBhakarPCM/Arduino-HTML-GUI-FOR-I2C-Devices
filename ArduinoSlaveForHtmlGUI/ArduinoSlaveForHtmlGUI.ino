#include <Wire.h>
//#include <QN8027Radio.h>

//QN8027Radio radio = QN8027Radio();
uint8_t st=0;
uint8_t fsmStatus;
uint8_t incomingByte;
uint8_t counter=0;
uint8_t waiting=0;
uint8_t alldata[5];
String stats[] = {"Resetting","Recalibrating","Idle","TxReady","PACalib","Transmiting","PA Off"};
void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Welcome to Arduino via COM");
  //-------Circuit Crystal--------------------------------------
  //radio.setClockSource(0);
  //radio.setCrystalCurrent(90);
  //radio.setCrystalFreq(12);
  
  //--------Reset And Antenna ReCalibration---------------------
  //radio.reset();
  //radio.reCalibrate();//importnt
  
  //--------Transmitter And RF Power Amplifier------------------
  //Serial.print("after reseting and recalib regValue:");
  //getV(0x2C,0x00,0xFF);
  //radio.setPreEmphTime50(ON);        //No effect.
  //radio.setFrequency(88.05);
  //radio.setTxFreqDeviation(150);   //Total Broadcast channel Bandwidth
  //radio.radioNoAudioAutoOFF(OFF);
  //radio.setTxPower(75); //default is already maximum.
  //radio.Switch(ON);
  
  //--------RDS--------------------------------------------------
  //radio.RDS(ON);
  //radio.setRDSFreqDeviation(10); //RDS channel Bandwidth at 57KHz on MPX Spectrum

  //--------Audio------------------------------------------------
  //radio.MonoAudio(ON);
  //radio.mute(ON);
  //radio.scrambleAudio(ON);
  //radio.setTxPilotFreqDeviation(10); //Pilot tone Bandwidth at 19KHz in MPX Spectrum
  //radio.setAudioInpImp(20);      //Audio Input Impedence, lower value boosts volume, 5,10,20,40 KOhm
  //radio.setTxInputBufferGain(4); //volume boost. max 5
  //radio.setTxDigitalGain(2);     //volume boost  max 2
  
  //delay(2000);
  //Serial.println("Radio Started");
  
}
uint8_t read1Byte(uint8_t devID ,uint8_t regAddr)
{
  int8_t errorCode = 4;
  Wire.beginTransmission(devID);
  Wire.write(regAddr);
  Wire.endTransmission();
  Wire.requestFrom(devID, 1);
  errorCode = Wire.endTransmission();   //ACK read
  uint8_t readData = Wire.read();
  return readData;
}
void write1Byte(uint8_t devID, uint8_t regAddr,uint8_t comData)
{
  int8_t errorCode = 4;
  Wire.beginTransmission(devID);
  Wire.write(regAddr);
  Wire.write(comData);
  errorCode = Wire.endTransmission();   //ACK read
}
void setV(uint8_t devID,uint8_t regID,uint8_t mask,uint8_t sValue){
    uint8_t currentValue = read1Byte(devID,regID);
    Serial.print("DEV:0x");
    Serial.print(devID,HEX);
    Serial.print(" REG:0x");
    Serial.print(regID,HEX);
    Serial.print(" MASK:0b");
    Serial.print(mask,BIN);
    Serial.print(" VAL:0b");
    Serial.print(sValue,BIN);
    Serial.print("  currValue:");
    Serial.print(currentValue,BIN);
    Serial.print("  newValue:");
    
    currentValue = currentValue & (~mask); //clear those bits
    //shift  bits of "value" to cleared position.
    while(!(mask & 1)){ //do not provide 0x00 mask. loop will stuck forever.
      mask  = mask  >> 1;
      sValue = sValue << 1;
    }
    currentValue = currentValue | sValue;   //fit value directly on those bits
    write1Byte(devID,regID,currentValue);
    
    Serial.println(currentValue,BIN);
}
void getV(uint8_t devID,uint8_t regID,uint8_t mask){
    uint8_t value = read1Byte(devID,regID);
    value = value & mask;
    while(!(mask & 1)){
      mask  = mask  >> 1;
      value = value >> 1;
    }
    Serial.print(":RTN:");
    Serial.println(value);
}
void tB(uint8_t devID,uint8_t regID,uint8_t mask){
  uint8_t currentValue = read1Byte(devID,regID);
  currentValue = currentValue & mask;
  if(currentValue>0){
    setV(devID,regID,mask,0);
  }else{
    setV(devID,regID,mask,1);
  }
}
void executeCommands(){
  uint8_t devID  = alldata[1];
  uint8_t regID  = alldata[2];
  uint8_t mask   = alldata[3];
  uint8_t sValue = alldata[4];
  switch(alldata[0]){
    case 1:     // set value.
          setV(devID,regID,mask,sValue);
          break;    
    case 2:     // get value.
          getV(devID,regID,mask);
          break;
    case 3:     // toggle bit without knowing its value.
          tB(devID,regID,mask);
          break;
  }
}
void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    if(waiting>0){ //older parsing pending.
      alldata[counter] = 64*(waiting-194+2) + incomingByte - 128;
      //Serial.print("cleared waiting:");
      //Serial.println(alldata[counter],DEC);
      counter++;
      waiting = 0;
    }else if(incomingByte<194){
      alldata[counter] = incomingByte;
      //Serial.print("Received Direct Byte:");
      //Serial.println(alldata[counter],DEC);
      counter++;
    }else if(incomingByte>193){
      waiting = incomingByte;
      //Serial.print("put in waiting:");
      //Serial.println(waiting,DEC);
    }
    if(counter>4){
      counter=0;
      //Serial.println("------------");
      executeCommands();
    }
  }
  /*
  if(radio.getFSMStatus()!=fsmStatus){
    fsmStatus = radio.getFSMStatus();
    Serial.print("FSM Mode Changed:");
    Serial.println(stats[fsmStatus]);
  }
  radio.sendStationName("QN8027FM");
  Serial.println("StationName Sent!");
  delay(5000);
  radio.sendRadioText("-:Word 'Teacher' is made from 'Teach', Not Knowledge");
  Serial.println("RadioText Sent!");
  delay(5000);*/
}
