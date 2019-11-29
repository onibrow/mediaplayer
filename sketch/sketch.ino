
// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

#define BREAKOUT_RESET  7 // 9      // VS1053 reset pin (output)                   // PB1 DNE -> GPIO 5 = Pin 7
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)             // PB2 Ex = SS       = Pin 10
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)     // PB0 Ex = GPIO 6   = Pin 8
//#define SHIELD_CS     7      // VS1053 chip select pin (output)
//#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 6 // 4     // Card chip select pin                                  // PD4 DNE -> GPIO 4 = Pin 6
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 5 // 3       // VS1053 Data request, ideally an Interrupt pin         // PD3 DNE -> GPIO 3 = Pin 5

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
int start_sig = 0;

void setup() {
  Serial.begin(9600);
  pinMode(14, INPUT);


  if (! musicPlayer.begin()) {
    while (1);
  }

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  // list files
  SD.open("/");

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(10, 10);

  // Timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  //  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

  // Play one file, don't return until complete
  //  Serial.println(F("Playing FLAC"));
  //  musicPlayer.playFullFile("track1~1.fla");
  // Play another file in the background, REQUIRES interrupts!
  //  Serial.println(F("Playing MP3"));

}

void loop() {
  // File is playing in the background
  start_sig = digitalRead(14);
  if (musicPlayer.stopped() && start_sig == 1) {
    musicPlayer.playFullFile("sugar.mp3");
  }
  delay(100);
  //  if (musicPlayer.stopped()) {
  //    Serial.println("Done playing music");
  //    while (1) {
  //      delay(10);  // we're done! do nothing...
  //    }
  //  }

  //  if (Serial.available()) {
  //    char c = Serial.read();
  //
  //    // if we get an 's' on the serial console, stop!
  //    if (c == 's') {
  //      musicPlayer.stopPlaying();
  //    }
  //
  //    // if we get an 'p' on the serial console, pause/unpause!
  //    if (c == 'p') {
  //      if (! musicPlayer.paused()) {
  //        Serial.println("Paused");
  //        musicPlayer.pausePlaying(true);
  //      } else {
  //        Serial.println("Resumed");
  //        musicPlayer.pausePlaying(false);
  //      }
  //    }
  //  }

  delay(10);
}


/// File listing helper
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      //Serial.println("**nomorefiles**");
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
