/*-----------------------------------------------------------------------------
  Source file for interfacing the Arduino microcontroller with
  VS1053 mp3 decoder chip and a SD card

             Written By -  Kalum <kalum_slk@gmx.com>
       Modified By Carlos Durandal to load FLAC plugin from SD.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


  -----------------------------------------------------------------------------*/

#include <Bounce2.h>
#include <SD.h>
#include <SPI.h>

Sd2Card card;
SdVolume volume;
SdFile root;
SdFile file;

/** Control Chip Select Pin (for accessing SPI Control/Status registers) */
#define MP3_XCS 2
/** Data Chip Select / BSYNC Pin */
#define MP3_XDCS 7
/** SD Card Chip Select  */
#define SD_CS 5
// ** Data Request Pin: Player asks for more data */
#define MP3_DREQ 3

/** VS10xx SCI Registers */
#define SPI_MODE 0x0   /**< VS10xx register */
#define SPI_STATUS 0x1   /**< VS10xx register */
#define SPI_BASS 0x2   /**< VS10xx register */
#define SPI_CLOCKF 0x3   /**< VS10xx register */
#define SPI_DECODE_TIME 0x4   /**< VS10xx register */
#define SPI_AUDATA 0x5   /**< VS10xx register */
#define SPI_WRAM 0x6   /**< VS10xx register */
#define SPI_WRAMADDR 0x7   /**< VS10xx register */
#define SPI_HDAT0 0x8 /**< VS10xx register */
#define SPI_HDAT1 0x9 /**< VS10xx register */
#define SPI_AIADDR 0xa   /**< VS10xx register */
#define SPI_VOL 0xb   /**< VS10xx register */
#define SPI_AICTRL0 0xc   /**< VS10xx register */
#define SPI_AICTRL1 0xd   /**< VS10xx register */
#define SPI_AICTRL2 0xe   /**< VS10xx register */
#define SPI_AICTRL3 0xf   /**< VS10xx register */

#define SPI_DEFAULT_SPEED SPI_CLOCK_DIV2

const int FORWARD_PIN = A1;
const int BACK_PIN = A0;
Bounce forward_button = Bounce (FORWARD_PIN, 25);
Bounce backward_button = Bounce (BACK_PIN, 25);

// store error strings in flash to save RAM
//#define error(s) error_P(s)
//
//void
//error_P (const char *str)
//{
//  if (card.errorCode ())
//    {
//      Serial.print (card.errorCode (), HEX);
//      Serial.print (',');
//      Serial.println (card.errorData (), HEX);
//    }
//  while (1);
//}

/** Pull the VS10xx Control Chip Select line Low */
void inline __attribute__ ((always_inline)) vs1053_select_control ()
{
  fastDigitalWrite (MP3_XCS, LOW);
}

/** Pull the VS10xx Control Chip Select line High */
void inline __attribute__ ((always_inline)) vs1053_deselect_control ()
{
  fastDigitalWrite (MP3_XCS, HIGH);
}

/** Pull the VS10xx Data Chip Select line Low */
void inline __attribute__ ((always_inline)) vs1053_select_data ()
{
  fastDigitalWrite (MP3_XDCS, LOW);
}

/** Pull the VS10xx Data Chip Select line High */
void inline __attribute__ ((always_inline)) vs1053_deselect_data ()
{
  fastDigitalWrite (MP3_XDCS, HIGH);
}


void inline __attribute__ ((always_inline)) select_sd ()
{
  fastDigitalWrite (SD_CS, LOW);
}

void inline __attribute__ ((always_inline)) deselect_sd ()
{
  fastDigitalWrite (SD_CS, HIGH);
}


void
init_spi ()
{
  SPI.setBitOrder (MSBFIRST);
  SPI.setDataMode (SPI_MODE0);
  SPI.setClockDivider (SPI_CLOCK_DIV16);  // slow SPI bus speed
  SPI.transfer (0xFF);
//  Serial.println("init_spi completed");

}

void
vs1053_write_register (unsigned char addressbyte, unsigned char highbyte,
                       unsigned char lowbyte)
{
  // SPIInit ();
  init_spi ();
  // SPI.setClockDivider (SCI_WRITE_SPEED);
  deselect_sd ();
//  Serial.println("2");
  vs1053_deselect_data ();
//  Serial.println("3");
  vs1053_select_control ();
//  Serial.println("4");
  delay (1);
  SPI.transfer (0x02);    // write command
//  Serial.println("5");
  SPI.transfer (addressbyte);
//  Serial.println("6");
  SPI.transfer (highbyte);
//  Serial.println("7");
  SPI.transfer (lowbyte);
//  Serial.println("8");
  vs1053_deselect_control ();
//  Serial.println("9");

  // GETS STUCK HERE
//  while (!digitalRead (MP3_DREQ));
//  Serial.println("10");

  SPI.setClockDivider (SPI_DEFAULT_SPEED);
//  Serial.println("11");
}



/** Read the 16-bit value of a VS10xx register */
unsigned int
vs1053_read_register (unsigned char addressbyte)
{
  unsigned int resultvalue = 0;
  unsigned int aux = 0;

  init_spi ();
  deselect_sd ();
  vs1053_deselect_data ();
  vs1053_select_control ();
  SPI.transfer (0x03);    // read command
  SPI.transfer (addressbyte);

  aux = SPI.transfer (0xff);
  resultvalue = aux << 8;
  aux = SPI.transfer (0xff);
  resultvalue |= aux;

  vs1053_deselect_control ();
  SPI.setClockDivider (SPI_DEFAULT_SPEED);
  return resultvalue;
}


/** Set VS10xx Volume Register */
void
vs1053_set_volume (unsigned char leftchannel, unsigned char rightchannel)
{
  vs1053_write_register (SPI_VOL, leftchannel, rightchannel);
}


unsigned int
vs1053_get_endfillbyte ()
{
  unsigned int reg;

  vs1053_write_register (SPI_WRAMADDR, 0x1e, 0x06);
  reg = vs1053_read_register (SPI_WRAM);

  return reg;
}


unsigned int
vs1053_get_byterate ()
{

  vs1053_write_register (SPI_WRAMADDR, 0x1e, 0x05);
  return (vs1053_read_register (SPI_WRAM));

}

void
vs1053_toggle_bass_boost ()
{
  unsigned int reg = vs1053_read_register (SPI_BASS);
  //Serial.print (reg,BIN); Check default. Default is 0
  if ((reg & B11110000))
  {
    vs1053_write_register (SPI_BASS, 0, 0);
    Serial.println("Flat");
  }
  else
  {
    vs1053_write_register (SPI_BASS, 0x00, 0x36); // +3dB below 60Hz
    Serial.println ("bass_boost");
  }
}

void vs1053_load_patch (void)
{
  if (SD.exists("PLAYFLAC.BIN")) {

    Serial.println("Found FLAC plugin, loading...");
    File plugin = SD.open("PLAYFLAC.BIN");
    if (!plugin) {
      //       error ("Open plugin failed");
      Serial.println("Open plugin failed");
    }
    else {
      while (plugin.available ()) {

        unsigned short addr, n, val, aux;
        aux = plugin.read();
        addr = aux << 8;
        aux = plugin.read();
        addr |= aux;

        aux = plugin.read();
        n = aux << 8;
        aux = plugin.read();
        n |= aux;

        if (n & 0x8000U) { /* RLE run, replicate n samples */
          n &= 0x7FFF;

          aux = plugin.read();
          val = aux << 8;
          aux = plugin.read();
          val |= aux;

          while (n--) {
            vs1053_write_register (addr, val >> 8, val & 0xff);
          }
        }
        else {    /* Copy run, copy n samples */
          while (n--) {
            aux = plugin.read();
            val = aux << 8;
            aux = plugin.read();
            val |= aux;

            vs1053_write_register (addr, val >> 8, val & 0xff);
          }
        }
      }
    }
    plugin.close();
  }
}

/** Soft Reset of VS10xx with patching the code */
void
vs1053_softreset_and_patch ()
{
  unsigned int sci_status = 0;

  init_spi ();      // slow speed
  Serial.println("1");

  // software reset the VS

  vs1053_write_register (SPI_MODE, 0x08, 0x04);
//  Serial.println("12");
  // GETS STUCK AGAIN
//  while (!fastDigitalRead (MP3_DREQ));

  vs1053_load_patch ();


  vs1053_write_register (SPI_CLOCKF, 0xc0, 0x00); // VS1053
  delay (1);
  vs1053_write_register (SPI_MODE, B00001000, B00000000);
  //  vs1053_set_volume (0x3A, 0x3A);
  vs1053_set_volume (0x20, 0x20);
  // ram up the SPI speed now that the VS internal clock has been set
  SPI.setClockDivider (SPI_DEFAULT_SPEED);

}

void
vs1053_softreset_without_patch ()
{
  vs1053_write_register (SPI_AIADDR, 0x03, 0x00);
  delay (1);
  // GET STUCK HERE
//  while (!fastDigitalRead (MP3_DREQ));


  vs1053_write_register (SPI_CLOCKF, 0xc0, 0x00); // VS1003
  delay (1);
  vs1053_write_register (SPI_MODE, B00001000, B00010000);
}


// Transfers 32 bytes of data

void
transfer_32bytes (unsigned char *buf)
{

  deselect_sd ();
  vs1053_deselect_control ();
  vs1053_select_data ();

  // GETS STUCK HERE
//  while (!fastDigitalRead (MP3_DREQ));

  for (unsigned char i = 0; i < 32; i++)
  {
    SPI.transfer (buf[i]);  // Send out 32 bytes
  }

  vs1053_deselect_data ();
}


//transfer's a user specified number of bytes (<255) to the vs1053
void
transfer_xbytes (unsigned char *buf, unsigned char num_bytes)
{

  deselect_sd ();
  vs1053_deselect_control ();
  vs1053_select_data ();

  // GETS STUCK HERE
//  while (!fastDigitalRead (MP3_DREQ));

  for (unsigned char i = 0; i < num_bytes; i++)
  {
    SPI.transfer (buf[i]);  // Send out num bytes
  }

  vs1053_deselect_data ();
}



// transfers a maximum of 32 bytes of a specified value to the mp3 decoder

void
transfer_value_bytes (unsigned char value, unsigned int num)
{

  if (num > 32)
    Serial.println ("error invalid byte number");

  deselect_sd ();
  vs1053_deselect_control ();
  vs1053_select_data ();
  
// GETS STUCK HERE
//  while (!fastDigitalRead (MP3_DREQ));

  for (int i = 0; i < num; i++)
  {
    SPI.transfer (value); // Send out 32 bytes
    // delayMicroseconds(10);
  }

  vs1053_deselect_data ();

}

// Transfer's num_bytes of value to the VS1053

void
transfer_num_bytes (unsigned char value, unsigned int num_bytes)
{

  unsigned int i;

  if (num_bytes == 0)
    return;

  for (i = 0; i < num_bytes / 32; i++)
  {

    transfer_value_bytes (value, 32);

  }

  if (num_bytes % 32 == 0)
    return;
  else
    transfer_value_bytes (value, num_bytes % 32);

}


void
init_vs ()
{
  Serial.println ("Initing VS");

  vs1053_softreset_and_patch ();

  Serial.println ("Init VS - Done");
}


int
init_sd ()
{
  Serial.print ("Starting SD Card...  ");
  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  vs1053_deselect_control ();
  vs1053_deselect_data ();

  delay (10);
  if (!card.init (SPI_FULL_SPEED, SD_CS))
  {
    //    error ("card init failed");
    Serial.println("card init failed");
    return -1;
  }
  else
    Serial.println ("Success!");
  // initialize a FAT volume
  if (!volume.init (&card))
  {
    //    error ("volume.init failed");
    Serial.println("volume.init failed");
    return -1;
  }
  // open the root directory
  if (!root.openRoot (&volume))
  {
    //    error ("openRoot failed");
    Serial.println("openRoot failed");
    return -1;
  }
  deselect_sd ();
  vs1053_deselect_control ();
  vs1053_deselect_data ();
  return 1;

}

void
vs1053_terminate_stream ()
{

  int sci_mode;
  int hdat0;
  int hdat1;

  // Send proper VS commands at EOF
  unsigned int endfillbyte = vs1053_get_endfillbyte ();
  transfer_num_bytes (endfillbyte, 2052);
  sci_mode = vs1053_read_register (SPI_MODE);
  sci_mode |= 0x8;
  vs1053_write_register (SPI_MODE, sci_mode >> 8, sci_mode & 0xff);

  // send up 2048 bytes of endFillByte[7:0].
  for (unsigned char i = 0; i < 64; i++)
  {
    // send at least 32 bytes of endFillByte[7:0]
    transfer_num_bytes (endfillbyte, 32);
    // read SPI MODE; if SM CANCEL is still set, repeat
    sci_mode = vs1053_read_register (SPI_MODE);
    if ((sci_mode & 0x08) == 0x0000)
    {
      break;
    }
  }

  if (sci_mode & 0x8)
  {
    Serial.println ("Software reset at EOF");
    vs1053_softreset_without_patch ();
  }

  hdat0 = vs1053_read_register (SPI_HDAT0);
  hdat1 = vs1053_read_register (SPI_HDAT1);
  Serial.print ("HDAT0 - ");
  Serial.println (hdat0);
  Serial.print ("HDAT1 - ");
  Serial.println (hdat1);

}


typedef enum
{
  P_PLAYING,
  P_PAUSE,
  P_NEXT_TRACK,
  P_PREV_TRACK,
  P_DONE,
  P_STOPPED,
  P_ERROR
} play_status;




unsigned long button_down_millis = 0;

int
get_user_input ()
{

  if (forward_button.update () && (forward_button.read () == LOW))
  {
    return P_NEXT_TRACK;
  }


  if (backward_button.update () && (backward_button.read () == LOW))
  {
    return P_PREV_TRACK;
  }


  if (Serial.available ())
  {
    int c = Serial.read ();
    if (c == 'n')
    {
      Serial.println ("next track");
      return P_NEXT_TRACK;

    }

    if (c == 'p')
    {
      Serial.println ("prev track");
      return P_PREV_TRACK;

    }

    if (c == 'b')
    {
      vs1053_toggle_bass_boost ();
      return -1;
      return P_PREV_TRACK;

    }
    if (c == 's')
    {
      Serial.println ("inEar");
      vs1053_write_register (SPI_MODE, B00001000, B00010000);
      return -1;
      return P_PREV_TRACK;

    }
    if (c == 'x')
    {
      Serial.println ("inEaroff");
      vs1053_write_register (SPI_MODE, B00001000, B00000000);
      return -1;
      return P_PREV_TRACK;

    }

  }

  return -1;
}


unsigned long prev_time = 0;

void
extra_cpu_time ()
{

}



// This will play a opened file sent to it and call a function to handle
// user input, processing accordingly


int
play_file (SdFile * file_p)
{

  unsigned char buf[32];

  unsigned long file_size = 0;
  unsigned char n;
  byte end_fill_byte = 0;
  unsigned int sci_mode;
  unsigned int hdat0;
  unsigned int hdat1;
  char cancel_flag = 0;
  unsigned int cancel_byte_count = 0;
  unsigned char user_input;


  file_size = file_p->fileSize ();
  Serial.print ("File size= ");
  Serial.println (file_size);

  //reset the start time for the file to zero
  vs1053_write_register (SPI_DECODE_TIME, 0, 0);
  vs1053_write_register (SPI_DECODE_TIME, 0, 0);

  while ((n = file_p->read (buf, sizeof (buf))) > 0)
  {


    //       extra_cpu_time();
    if (cancel_flag == 0)
      user_input = get_user_input ();

    if (user_input == P_NEXT_TRACK || user_input == P_PREV_TRACK
        || cancel_flag == 1)
    {
      // is program in cancel loop already
      if (cancel_flag == 0)
      {
        sci_mode = vs1053_read_register (SPI_MODE);
        sci_mode |= 0x8;
        vs1053_write_register (SPI_MODE, sci_mode >> 8,
                               sci_mode & 0xff);
        cancel_flag = 1;
      }

      transfer_xbytes (buf, n);
      cancel_byte_count += n;

      sci_mode = vs1053_read_register (SPI_MODE);

      if ((sci_mode & B1000) == 0)
      {
        unsigned int endfillbyte = vs1053_get_endfillbyte ();
        Serial.print ("endFillByte =");
        Serial.println (endfillbyte, DEC);
        transfer_num_bytes (endfillbyte, 2052);
        hdat0 = vs1053_read_register (SPI_HDAT0);
        hdat1 = vs1053_read_register (SPI_HDAT1);

        Serial.print ("HDAT0 - ");
        Serial.println (hdat0);
        Serial.print ("HDAT1 - ");
        Serial.println (hdat1);
        return user_input;

      }

      if (cancel_byte_count >= 2048)
      {
        Serial.print ("sci mode in cancel");
        Serial.println (sci_mode);
        Serial.println ("Software reseting in  cancel");
        vs1053_softreset_without_patch ();
        return user_input;
      }
      continue;

    }

    if (n < sizeof (buf))
    {

      transfer_xbytes (buf, n);
      Serial.println ("End of file reached");
      vs1053_terminate_stream ();

      return P_DONE;
    }

    transfer_32bytes (buf);
  }

  if (n == 0)
  {
    Serial.println ("End of file reached");
    vs1053_terminate_stream ();
    return P_DONE;
  }
  else
    return P_ERROR;
}




void
setup ()
{

  Serial.begin (9600);

  pinMode (10, OUTPUT);
  pinMode (MP3_DREQ, INPUT);
  pinMode (SD_CS, OUTPUT);
  pinMode (MP3_XCS, OUTPUT);
  pinMode (MP3_XDCS, OUTPUT);

  //Forward backward buttons etc....
  pinMode (FORWARD_PIN, INPUT);
  digitalWrite (FORWARD_PIN, HIGH);
  pinMode (BACK_PIN, INPUT);
  digitalWrite (BACK_PIN, HIGH);

  SPI.begin ();
  init_sd ();
  SD.begin(SD_CS); // For SD.exists() SD.begin must have the correct chip select  parameter.
  init_vs ();

  deselect_sd ();
  vs1053_deselect_control ();
  vs1053_deselect_data ();

}


// goes to a specific file number.....

int
goto_file_num (int num)
{

  dir_t dir;

  root.rewind ();

  for (int i = 0; i <= num; i++)
  {
    root.readDir (&dir);
  }

  Serial.println ("At file");
  file.printDirName (dir, 10);
}

//Goes to the next file number in the SD card
int
next_file (int cur_number, int num_entries)
{

  if (cur_number + 1 >= num_entries)
  {
    goto_file_num (0);
    return 0;
  }
  else
  {
    goto_file_num (cur_number + 1);
    return cur_number + 1;
  }
}

//Goes to the previous file number in the SD card
int
prev_file (int cur_number, int num_entries)
{

  if (cur_number - 1 < 0)
  {
    goto_file_num (num_entries - 1);
    return num_entries - 1;
  }
  else
  {
    goto_file_num (cur_number - 1);
    return cur_number - 1;
  }
}


void
loop ()
{
  unsigned int num_files = 0;
  int num_dirs = 0;
  int num_entries = 0;
  int cur_file_num = 0;
  int ret = P_NEXT_TRACK;
  dir_t dir;


  while (root.readDir (&dir) > 0)
  {
    if (file.open (&root, root.curPosition () / 32 - 1, O_READ))
    {
      num_entries++;

      if (file.isDir ())
        num_dirs++;
      else if (file.isFile ())
        num_files++;
      file.close ();
    }

  }


  Serial.print ("Number of files in root ");
  Serial.println (num_files);
  Serial.print ("Number of dirs in root ");
  Serial.println (num_dirs);


  if (num_files == 0)
  {
    Serial.print ("No FILES in SD");
    while (1);
  }


  root.rewind ();

  goto_file_num (0);

  while (1)
  {
    if (file.open (&root, root.curPosition () / 32 - 1, O_READ))
    {
      Serial.print ("Opened ");

    }
    else
    {
      Serial.print ("file.open failed for ");
      file.printDirName (dir, 10);
      file.close ();
      delay (500);
    }
    if (file.isDir ())
    {
      Serial.print ("Is directory");
      file.close ();
      if (ret == P_PREV_TRACK)
        cur_file_num = prev_file (cur_file_num, num_entries);
      else
        cur_file_num = next_file (cur_file_num, num_entries);

      continue;
    }
    // play file

    switch (ret = play_file (&file))
    {
      case P_DONE:
        cur_file_num = next_file (cur_file_num, num_entries);
        break;
      case P_NEXT_TRACK:
        cur_file_num = next_file (cur_file_num, num_entries);
        break;
      case P_PREV_TRACK:
        cur_file_num = prev_file (cur_file_num, num_entries);
        break;
      case P_ERROR:
        Serial.println ("error occured while playing file");
        cur_file_num = next_file (cur_file_num, num_entries);
        break;
      default:
        cur_file_num = next_file (cur_file_num, num_entries);
        break;

    }
    file.close ();

  }

  Serial.println ("All files played");
  while (1);
}

