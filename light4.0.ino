#include <Adafruit_NeoPixel.h>
 
#define PIN      3
#define N_LEDS 150
 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

  char junk;
  
 
  int mode=0;
  int flowit=0;
  int count=0;
  int lspeed=0;
  int contspeed = 0;
  int x =0;
  int y =0;
  int z =0;
  uint32_t ledcolor=strip.Color(255, 255, 255);




#define SEPERATING_CHAR 110  // ASCII-13 is '\r' carriage return character that seperates two lines



  
  
void setup() {
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  strip.begin();
  //Serial.begin(38400);
  randomSeed(analogRead(A3));
  Serial.begin(9600);
}

void loop() {
  bool pb1=digitalRead(12);
  bool pb2=digitalRead(11);
  int r=255, g=255, b=255;

   
  
 char *str, *substr;

  
    str=receiveLine(Serial.read());
    
    if (str!=NULL) // we have received a complete line
    { 
      while(Serial.available()){
        Serial.read();
      }
      Serial.print(str);
      if(str[0]=='m'){
        mode=(int)str[1]-48;
      }
      if(str[0]=='s'){
        lspeed=(int)str[1]-48;
      }
      if(str[0]=='r'){
        x=((int)str[1]-48)*100+((int)str[2]-48)*9+ ((int)str[3]-48)*1;
      }
      if(str[0]=='g'){
        y=((int)str[1]-48)*100+((int)str[2]-48)*8+ ((int)str[3]-48)*1;
      }
      if(str[0]=='b'){
        z=((int)str[1]-48)*100+((int)str[2]-48)*8+ ((int)str[3]-48)*1;
      }
      ledcolor=strip.Color(x, y, z);
    }
 // Serial.println(contspeed);
  contspeed+=(1+lspeed);
  if(contspeed >= 10000){
    contspeed =0;
  switch (mode) {
    case 0:
    off();
    break;
    case 1:
    solid(ledcolor); 
   break;
      case 2:
      contspeed =9000;
    fade(flowit);
    if(flowit>=767){
      flowit=0;
    }
    else{
      flowit+=4;
    }
    break;
      case 3:
      contspeed =9000;
    flow(flowit);
    if(flowit>=767){
      flowit=0;
    }
    else{
      flowit+=4;
    }
    break;
  case 4:

    contspeed =9000;
    chase(ledcolor); 
    
    if(flowit==N_LEDS+5){
      flowit=0;
    }
    else{
      flowit++;
    }

    break;
  case 5:
    crazy(x);
    break;
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
static void solid(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
  }
        strip.show();
}

//_______________________________________________________________________
static void fade(int j) {
  uint32_t c;
  uint32_t tmp;

          int t=(j)%768;
          if(t<256){
              c=strip.Color(255-t, t, 0);
            }
            else if(t<512){
              c= strip.Color(0, 511-t, t-256);
            }
            else{
              c= strip.Color(t-512, 0, 767-t);
            }
            solid(c);  
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
static void flow(int j) {
  uint32_t c;
  uint32_t tmp;
 
      for(int k=0; k<256; k++) {
          int t=(j+k)%768;
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
      strip.setPixelColor(i-10, 0); // Erase pixel a few steps back
      strip.show();
      Serial.print("hi");


}
//_______________________________________________________________________
static void crazy(int t) {
       for(int i=0; i<N_LEDS/2;i++){
         strip.setPixelColor(random(N_LEDS) , 0); 
      }
      for(int i=0; i<t+1;i++){
         strip.setPixelColor(random(N_LEDS) , strip.Color(random(150), random(150), random(150))); 
      }

      // Draw new pixel
      strip.show();

}
//_______________________________________________________________________
