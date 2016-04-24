#include <Adafruit_DotStar.h>
#include <avr/power.h> 

#define ISONEWAY 1


#define NUMPIXELS 60 
#define DATAPIN    4
#define CLOCKPIN   5
#define BASE 5
#define gain 20
#define numReadings 5

Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
  
uint32_t c2 = 0xFF0000;
uint32_t c1 = 0x00FF00;
uint32_t c3 = 0xFFFF00;

void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output  
  strip.show();
}

int dir = 0;
int head=0, tail=-20, tail2 =-40;
int intensity = 0;

void loop() {
  int brightness = smoothing();
  
  strip.setBrightness(smoothing()+BASE);

 if(!dir){
    forward();
  }
  else{
    backward();
  }

  strip.show();                     // Refresh strip
  delay(20);                        // Pause 20 milliseconds (~50 FPS)
  
}

void forward(){
  strip.setPixelColor(head, c1); // 'On' pixel at head
  strip.setPixelColor(tail, c2);     // 'Off' pixel at tail
  strip.setPixelColor(tail2, c3);     // 'Off' pixel at tail


  if(++head >= NUMPIXELS) {         // Increment head index.  Off end of strip?
    //head = 0;                       //  Yes, reset head index to start
    dir =1;
  }
  
  if(++tail >= NUMPIXELS) tail = 0; // Increment, reset tail index
  if(++tail2 >= NUMPIXELS) tail2 = 0; // Increment, reset tail index
}

void backward(){
  strip.setPixelColor(head, c1); // 'On' pixel at head
  strip.setPixelColor(tail, c2);     // 'Off' pixel at tail
  strip.setPixelColor(tail2, c3);     // 'Off' pixel at tail


  if(--head <= 0) {         // Increment head index.  Off end of strip?
    //head = NUMPIXELS;                       //  Yes, reset head index to start
    dir = 0;
  }
  
  if(--tail <= 0) tail = NUMPIXELS; // Increment, reset tail index
  if(--tail2 <= 0) tail2 = NUMPIXELS; // Increment, reset tail index
}


int readings[numReadings];      // the readings from the analog input

int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0; 
int maxx = 0;                   // Find minn/maxx to normalize input
int minn = 0; 
double normal = 0; 
const int NORMAL = 55;           //Gain for the normal paramater
int OPT = 0; 


int smoothing(){
  total = total - readings[readIndex];
  readings[readIndex] = analogRead(A0);

  if (maxx < readings[readIndex]){
    maxx = readings[readIndex]; 
  }
  if (minn > readings[readIndex]){
    minn = 0; 
  }
  
  total = total + readings[readIndex];
  readIndex = readIndex + 1;


  if (readIndex >= numReadings) {
    readIndex = 0;
  }

  // calculate the average and applies gain
  average = (total) / numReadings;

  normal = (double)average/((double)maxx );
   OPT = normal*gain;
  int dump = (int) OPT;
      
  return dump;    
}


