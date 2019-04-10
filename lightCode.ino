#include <Adafruit_NeoPixel.h>
 
#define PIN      3
#define N_LEDS 150
 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

  char junk;
  
 
  int mode=0;
  int flowit=0;
  int count=0;
  uint32_t ledcolor=strip.Color(255, 255, 255);




#define SEPERATING_CHAR 13  // ASCII-13 is '\r' carriage return character that seperates two lines



  
  
void setup() {
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  strip.begin();
  Serial.begin(38400);
  //Serial.begin(9600);
}

void loop() {
  bool pb1=digitalRead(12);
  bool pb2=digitalRead(11);
  int r=255, g=255, b=255;
  if(pb1==1){
     mode++;
     flowit=0;
     off();
     if(mode==4) mode=0;
     delay(200);
  }
   if(pb2==1){
     mode--;
     flowit=0;
     off();
     if(mode==-1) mode=3;
     delay(200);
  }
  
   
  
 char *str, *substr;
  int x,y,z,crc;
  if (Serial.available())
  {
    //Serial.print(" - ");
    str=receiveLine(Serial.read());
    if (str!=NULL) // we have received a complete line
    {
      
      substr=strtok(str,",");
      x=atoi(substr);
      
      substr=strtok(NULL,",");
      y=atoi(substr);
      substr=strtok(NULL,",");
      z=atoi(substr);
      substr=strtok(NULL,",");
      crc=atoi(substr);


      /*Serial.print("Input: ");
      Serial.print(x);Serial.print(" - ");
      Serial.print(y);Serial.print(" - ");
      Serial.print(z);Serial.print(" - ");
      if (substr!=NULL && crc==x+y+z)Serial.println("CRC OK");
      else Serial.println("CRC fail");*/
      //Serial.write("ok");
    }
  
   //Serial.print(r);
   //Serial.print(' ');
   //Serial.print(g);
   //Serial.print(' ');
   //Serial.print(b);
   //Serial.println(' ');
   
   
  ledcolor=strip.Color(x, y, z);
  //inputString = "";
  //Serial.flush();
}
switch (mode) {
  case 1:
    flow(&flowit);
    if(flowit==767){
      flowit=0;
    }
    else{
      flowit++;
    }
    break;
      case 2:
    chase(strip.Color(255, 0, 0));
    if(flowit==N_LEDS+5){
      flowit=0;
    }
    else{
      flowit++;
    }
    break;
  case 3:
    chase(ledcolor); 
    if(flowit==N_LEDS+5){
      flowit=0;
    }
    else{
      flowit++;
    }
  }

}

//________________________________________________________________________
static void off() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i  , 0); // Draw new pixel
  }
        strip.show();
}

//________________________________________________________________________
char* receiveLine(char c)
{
  static char lineBuffer[41];  // define maximum buffer length here (max string length +1)
  static byte counter=0;
  if (counter==0) memset(lineBuffer,0,sizeof(lineBuffer)); // clear buffer before using it
  if (c==SEPERATING_CHAR)
  {
    counter=0;
    return lineBuffer;
  }
  else if (c>=32 && counter<sizeof(lineBuffer)-1) 
  { // if is it a printable ASCII-character and fits into the lineBuffer
    lineBuffer[counter]=c; // insert character into lineBuffer
    counter++;
  }
  return NULL;
}
//_______________________________________________________________________
static void flow(int *j) {
  uint32_t c;
  uint32_t tmp;
 
      for(int k=0; k<256; k++) {
          int t=(*j+k)%768;
          if(t<256){
              c=strip.Color(255-t, t, 0);
            }
            else if(t<512){
              c= strip.Color(0, 511-t, t-256);
            }
            else{
              c= strip.Color(t-512, 0, 767-t);
            }
        strip.setPixelColor(k , c); // Draw new pixel

        //Serial.println(c);
    }
    strip.show();  
}
//_______________________________________________________________________
static void chase(uint32_t c) {
      uint16_t i= flowit;
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-5, 0); // Erase pixel a few steps back
      strip.show();
      delay(5);
}
//_______________________________________________________________________
