#include <SoftwareSerial.h>
#include <Arduino.h>

SoftwareSerial mySerial(2, 3); // RX, TX


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  mySerial.begin(9600);
  
  
}

struct Data{
  unsigned int length_frame;
  unsigned int std_1;
  unsigned int std_2_5;
  unsigned int std_10;
  unsigned int env_1;
  unsigned int env_2_5;
  unsigned int env_10;
  uint16_t par_0_3, par_0_5, par_1, par_2_5, par_5, par_10;
  uint16_t unused;
  unsigned int checksum;
};

struct Data data;


void loop() {
  // put your main code here, to run repeatedly:
  
  
  if (ReadData( &mySerial )){
    Serial.println("---------------------------------------");
    Serial.print("Standard PM 1.0: ");
    Serial.println(data.std_1);
    Serial.print("Standard PM 2.5: ");
    Serial.println(data.std_2_5);
    Serial.print("Standard PM 10: ");
    Serial.println(data.std_10);
    Serial.print("Environment PM 1.0: ");
    Serial.println(data.env_1);
    Serial.print("Environment PM 2.5: ");
    Serial.println(data.env_2_5);
    Serial.print("Environment PM 10: ");
    Serial.println(data.env_10);
    Serial.print("Particles PM 1.0: ");
    Serial.println(data.par_1);
    Serial.print("Particles PM 2.5: ");
    Serial.println(data.par_2_5);
    Serial.print("Particles PM 10: ");
    Serial.println(data.par_10);
    Serial.println("---------------------------------------");
  }  
}

boolean ReadData (Stream *s){
  if(!s->available()){
    return false;
  }

  //check the head of buffer is start-byte '0x42'
  if(s->peek()!=0x42){
    s->read();
    return false;
  }
  
  //check if the number of bits in buffer is less than 32
  if(s->available()<32){
    return false;
  }
  uint8_t buffer[32];
  uint16_t sum=0;
  s->readBytes(buffer,32);

  //get checksum ready
  for(uint8_t i=0; i<30;i++){
    sum += buffer[i];
  }

  // Endian to decimal
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
  
  memcpy((void *)&data, (void *)buffer_u16,30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}
