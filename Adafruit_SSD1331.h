/*************************************************** 
This is a library for the 0.96" 16-bit Color OLED with SSD1331 driver chip

Pick one up today in the adafruit shop!
------> http://www.adafruit.com/products/684

These displays use SPI to communicate, 4 or 5 pins are required to  
interface
Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.  
BSD license, all text above must be included in any redistribution

Rewritten by Rob Giseburt for speed and code size. - Aug 27, 2012

****************************************************/

#ifndef ADAFRUIT_SSD1331_H
#define ADAFRUIT_SSD1331_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "../Motate/MotatePins.h"
using namespace Motate;

// Ugly hack to prevent the need for include SPI.h from sketch
// TODO: Test on Linux and Windows
//#include <../../../../libraries/SPI/SPI.h>
#include <SPI.h>

// Select one of these defines to set the pixel color order
#define SSD1331_COLORORDER_RGB
// #define SSD1331_COLORORDER_BGR

#if defined SSD1331_COLORORDER_RGB && defined SSD1331_COLORORDER_BGR
	#error "RGB and BGR can not both be defined for SSD1331_COLORODER."
#endif

// Timing Delays
#define SSD1331_DELAYS_HWFILL		(1200)
#define SSD1331_DELAYS_HWLINE		(500)

// SSD1331 Commands
#define SSD1331_CMD_DRAWLINE 		0x21
#define SSD1331_CMD_DRAWRECT 		0x22
#define SSD1331_CMD_FILL 			0x26
#define SSD1331_CMD_SETCOLUMN 		0x15
#define SSD1331_CMD_SETROW    		0x75
#define SSD1331_CMD_CONTRASTA 		0x81
#define SSD1331_CMD_CONTRASTB 		0x82
#define SSD1331_CMD_CONTRASTC		0x83
#define SSD1331_CMD_MASTERCURRENT 	0x87
#define SSD1331_CMD_SETREMAP 		0xA0
#define SSD1331_CMD_STARTLINE 		0xA1
#define SSD1331_CMD_DISPLAYOFFSET 	0xA2
#define SSD1331_CMD_NORMALDISPLAY 	0xA4
#define SSD1331_CMD_DISPLAYALLON  	0xA5
#define SSD1331_CMD_DISPLAYALLOFF 	0xA6
#define SSD1331_CMD_INVERTDISPLAY 	0xA7
#define SSD1331_CMD_SETMULTIPLEX  	0xA8
#define SSD1331_CMD_SETMASTER 		0xAD
#define SSD1331_CMD_DISPLAYOFF 		0xAE
#define SSD1331_CMD_DISPLAYON		0xAF
#define SSD1331_CMD_POWERMODE 		0xB0
#define SSD1331_CMD_PRECHARGE 		0xB1
#define SSD1331_CMD_CLOCKDIV 		0xB3
#define SSD1331_CMD_PRECHARGEA 		0x8A
#define SSD1331_CMD_PRECHARGEB 		0x8B
#define SSD1331_CMD_PRECHARGEC 		0x8C
#define SSD1331_CMD_PRECHARGELEVEL 	0xBB
#define SSD1331_CMD_VCOMH 			0xBE

// Bit-Banged version
template<uint8_t CS_num, uint8_t RS_num, uint8_t SID_num, uint8_t SCLK_num, uint8_t RST_num>
class Adafruit_SSD1331_Base {
protected:
	Pin<CS_num> _cs;
	Pin<RS_num> _rs;
	Pin<SID_num> _sid;
	Pin<SCLK_num> _sclk;
	Pin<RST_num> _rst;
	PinHolder<SID_num, -1, -1, -1,  -1, -1, SCLK_num, -1> _sid_scl7;
	PinHolder<-1, SID_num, -1, -1,  -1, -1, SCLK_num, -1> _sid_scl6;
	PinHolder<-1, -1, SID_num, -1,  -1, -1, SCLK_num, -1> _sid_scl5;
	PinHolder<-1, -1, -1,  SID_num, -1, -1, SCLK_num, -1> _sid_scl4;
	PinHolder<-1, -1, -1, -1,  SID_num, -1, SCLK_num, -1> _sid_scl3;
	PinHolder<-1, -1, -1, -1, -1,  SID_num, SCLK_num, -1> _sid_scl2;
	PinHolder<-1, -1, -1, -1,  -1, -1, SID_num, SCLK_num> _sid_scl1;
	PinHolder<-1, -1, -1, -1,  -1, -1, SCLK_num, SID_num> _sid_scl0;
	// PinHolder<-1, -1, -1, -1,  -1, -1, SCLK_num, SID_num> _sid_scl;

public:
	Adafruit_SSD1331_Base() : _cs(Output), _rs(Output), _sid(Output), _sclk(Output), _rst(Output) {
		_sid = LOW;
		_sclk = LOW;
		_cs = HIGH;
		pin7 = Output;
		pin7 = LOW;
	};

	// commands
	void begin2(void) {
		
		// Toggle RST low to reset; CS low so it'll listen to us
		_cs = LOW;
		
		_rst = HIGH;
		delay(500);
		_rst = LOW;
		delay(500);
		_rst = HIGH;
		delay(500);

		// leave _cs LOW?
	}

	/* low level */

protected:
	void spiwrite(uint8_t c) {
		// Unroll this loop:
		// _sclk = LOW;
		// _sid = (c & 1<<7);
		_sid_scl7.set((c & 0b10000000));
		_sclk = HIGH;

		// _sclk = LOW;
		// _sid = (c & 1<<6);
		_sid_scl6.set((c & 0b01000000));
		_sclk = HIGH;

		// _sclk = LOW;
		// _sid = (c & 1<<5);
		_sid_scl5.set((c & 0b00100000));
		_sclk = HIGH;

		// _sclk = LOW;
		// _sid = (c & 1<<4);
		_sid_scl4.set((c & 0b00010000));
		_sclk = HIGH;

		// _sclk = LOW;
		// _sid = (c & 1<<3);
		_sid_scl3.set((c & 0b00001000));
		_sclk = HIGH;

		// _sclk = LOW;
		// _sid = (c & 1<<2);
		_sid_scl2.set((c & 0b00000100));
		_sclk = HIGH;

		// _sclk = LOW;
		// _sid = (c & 1<<1);
		_sid_scl1.set((c & 0b00000010));
		_sclk = HIGH;
		
		// _sclk = LOW;
		// _sid = (c & 1<<0);
		_sid_scl0.set((c & 0b00000001));
		_sclk = HIGH;
	};
};

// Hardware SPI version version
template<uint8_t CS_num, uint8_t RS_num, uint8_t RST_num>
class Adafruit_SSD1331_Base <CS_num, RS_num, -1, -1, RST_num> {
protected:
	Pin<CS_num> _cs;
	Pin<RS_num> _rs;
	Pin<RST_num> _rst;
public:
	Adafruit_SSD1331_Base() :_cs(Output), _rs(Output), _rst(Output) {};

	// commands
	void begin2(void) {
		SPI.begin();
		SPI.setDataMode(SPI_MODE3);

    // Toggle RST low to reset; CS low so it'll listen to us
		_cs = LOW;
		
		_rst = HIGH;
		delay(500);
		_rst = LOW;
		delay(500);
		_rst = HIGH;
		delay(500);
	}

protected:
	void spiwrite(uint8_t c) {
		SPI.transfer(c);
	}
};


// Common parts in here, the Base is specialized
template<uint8_t CS_num, uint8_t RS_num, uint8_t SID_num, uint8_t SCLK_num, uint8_t RST_num>
class Adafruit_SSD1331 :
		public Adafruit_SSD1331_Base<CS_num, RS_num, SID_num, SCLK_num, RST_num>,
		public Adafruit_GFX< Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num> > {
public:
	static const int16_t TFTWIDTH = 96;
	static const int16_t TFTHEIGHT = 64;

	Adafruit_SSD1331() : Adafruit_SSD1331_Base<CS_num, RS_num, SID_num, SCLK_num, RST_num>(), Adafruit_GFX< Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num> >((uint16_t)TFTWIDTH, (uint16_t)TFTHEIGHT) {};

	uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);

	// drawing primitives!
	void drawPixelImpl(int16_t x, int16_t y, uint16_t color);
	void drawLineImpl(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void fillRectImpl(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t fillcolor);
	// void drawRectImpl(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void pushColor(uint16_t c);

	// commands
	void init(void);
	using Adafruit_SSD1331_Base<CS_num, RS_num, SID_num, SCLK_num, RST_num>::begin2;
	void begin(void) {
		begin2();
		init();
	}
	void goHome(void);
	void goTo(int x, int y);

	using Adafruit_SSD1331_Base<CS_num, RS_num, SID_num, SCLK_num, RST_num>::_rs;
	using Adafruit_SSD1331_Base<CS_num, RS_num, SID_num, SCLK_num, RST_num>::_cs;
	using Adafruit_SSD1331_Base<CS_num, RS_num, SID_num, SCLK_num, RST_num>::spiwrite;
	
	using Adafruit_GFX< Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num> >::WIDTH;
	using Adafruit_GFX< Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num> >::HEIGHT;
	
	using Adafruit_GFX< Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num> >::height;
	using Adafruit_GFX< Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num> >::width;

	using Adafruit_GFX< Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num> >::setRotation;
	using Adafruit_GFX< Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num> >::getRotation;

	/* low level */
	void writeData(uint8_t d) {
		_rs = HIGH;
		_cs = LOW;
		spiwrite(d);
		_cs = HIGH;
	}
	void writeCommand(uint8_t c) {
		_rs = LOW;
		_cs = LOW;
		spiwrite(c);
		_cs = HIGH;
	}
};

template<uint8_t CS_num, uint8_t RS_num, uint8_t SID_num, uint8_t SCLK_num, uint8_t RST_num>
void Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num>::drawPixelImpl(int16_t x, int16_t y, uint16_t color)
{
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

  goTo(x, y);
  
  // setup for data
  _rs = HIGH;
  _cs = LOW;
  
  spiwrite(color >> 8);
  spiwrite(color);
  
  _cs = HIGH;
}

template<uint8_t CS_num, uint8_t RS_num, uint8_t SID_num, uint8_t SCLK_num, uint8_t RST_num>
void Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num>::pushColor(uint16_t color) {
  // setup for data
  _rs = HIGH;
  _cs = LOW;
  
  spiwrite(color >> 8);
  spiwrite(color);
  
  _cs = HIGH;
}

template<uint8_t CS_num, uint8_t RS_num, uint8_t SID_num, uint8_t SCLK_num, uint8_t RST_num>
void Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num>::goHome(void) {
  goTo(0,0);
}

template<uint8_t CS_num, uint8_t RS_num, uint8_t SID_num, uint8_t SCLK_num, uint8_t RST_num>
void Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num>::goTo(int x, int y) {
  if ((x >= WIDTH) || (y >= HEIGHT)) return;
  
  // set x and y coordinate
  writeCommand(SSD1331_CMD_SETCOLUMN);
  writeCommand(x);
  writeCommand(WIDTH-1);

  writeCommand(SSD1331_CMD_SETROW);
  writeCommand(y);
  writeCommand(HEIGHT-1);
}

template<uint8_t CS_num, uint8_t RS_num, uint8_t SID_num, uint8_t SCLK_num, uint8_t RST_num>
uint16_t Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num>::Color565(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t c;
  c = r >> 3;
  c <<= 6;
  c |= g >> 2;
  c <<= 5;
  c |= b >> 3;

  return c;
}

#if 0
/**************************************************************************/
/*! 
    @brief  Draws a filled rectangle using HW acceleration
*/
/**************************************************************************/

template<uint8_t CS_num, uint8_t RS_num, uint8_t SID_num, uint8_t SCLK_num, uint8_t RST_num>
void Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num>::drawRectImpl(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  switch (getRotation()) {
  case 1:
    swap(x, y);
    swap(w, h);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    swap(x, y);
    swap(w, h);
    y = HEIGHT - y - 1;
    break;
  }

  // Bounds check
  if ((x >= TFTWIDTH) || (y >= TFTHEIGHT))
	return;

  // Y bounds check
  if (y+h > TFTHEIGHT)
  {
    h = TFTHEIGHT - y;
  }

  // X bounds check
  if (x+w > TFTWIDTH)
  {
    w = TFTWIDTH - x;
  }

  // Disable fill
  writeCommand(SSD1331_CMD_FILL);
  writeCommand(0x00);

  writeCommand(SSD1331_CMD_DRAWRECT);
  writeCommand(x & 0xFF);       // Starting column
  writeCommand(y & 0xFF);       // Starting row
  writeCommand((x+w-1) & 0xFF); // End column
  writeCommand((y+h-1) & 0xFF); // End row

  // Outline color
  writeCommand((uint8_t)((color >> 11) << 1));
  writeCommand((uint8_t)((color >> 5) & 0x3F));
  writeCommand((uint8_t)((color << 1) & 0x3F));
  // Fill color -- none
  writeCommand(0x00);
  writeCommand(0x00);
  writeCommand(0x00);

  // Delay while the fill completes
  delayMicroseconds(SSD1331_DELAYS_HWFILL); 
}
#endif

/**************************************************************************/
/*! 
    @brief  Draws a filled rectangle using HW acceleration
*/
/**************************************************************************/

template<uint8_t CS_num, uint8_t RS_num, uint8_t SID_num, uint8_t SCLK_num, uint8_t RST_num>
void Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num>::fillRectImpl(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t fillcolor) 
{
//Serial.println("fillRect");
  // check rotation, move rect around if necessary
  switch (getRotation()) {
  case 1:
    swap(x, y);
    swap(w, h);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    swap(x, y);
    swap(w, h);
    y = HEIGHT - y - 1;
    break;
  }

  // Bounds check
  if ((x >= TFTWIDTH) || (y >= TFTHEIGHT))
	return;

  // Y bounds check
  if (y+h > TFTHEIGHT)
  {
    h = TFTHEIGHT - y;
  }

  // X bounds check
  if (x+w > TFTWIDTH)
  {
    w = TFTWIDTH - x;
  }
  
  // Enable fill
  writeCommand(SSD1331_CMD_FILL);
  writeCommand(0x01);

  writeCommand(SSD1331_CMD_DRAWRECT);
  writeCommand(x & 0xFF);       // Starting column
  writeCommand(y & 0xFF);       // Starting row
  writeCommand((x+w-1) & 0xFF); // End column
  writeCommand((y+h-1) & 0xFF); // End row
  
  // Outline color
  writeCommand((uint8_t)((fillcolor >> 11) << 1));
  writeCommand((uint8_t)((fillcolor >> 5) & 0x3F));
  writeCommand((uint8_t)((fillcolor << 1) & 0x3F));
  // Fill color
  writeCommand((uint8_t)((fillcolor >> 11) << 1));
  writeCommand((uint8_t)((fillcolor >> 5) & 0x3F));
  writeCommand((uint8_t)((fillcolor << 1) & 0x3F));
 
  // Delay while the fill completes
  delayMicroseconds(SSD1331_DELAYS_HWFILL); 
  // 
  // writeCommand(SSD1331_CMD_FILL);
  // writeCommand(0x00);
}

template<uint8_t CS_num, uint8_t RS_num, uint8_t SID_num, uint8_t SCLK_num, uint8_t RST_num>
void Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num>::drawLineImpl(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {	
  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    swap(x0, y0);
    swap(x1, y1);
    x0 = WIDTH - x0 - 1;
    x1 = WIDTH - x1 - 1;
    break;
  case 2:
    x0 = WIDTH - x0 - 1;
    y0 = HEIGHT - y0 - 1;
    x1 = WIDTH - x1 - 1;
    y1 = HEIGHT - y1 - 1;
    break;
  case 3:
    swap(x0, y0);
    swap(x1, y1);
    y0 = HEIGHT - y0 - 1;
    y1 = HEIGHT - y1 - 1;
    break;
  }

  // Boundary check
  if ((y0 >= TFTHEIGHT) && (y1 >= TFTHEIGHT))
	return;
  if ((x0 >= TFTWIDTH) && (x1 >= TFTWIDTH))
	return;	
  if (x0 >= TFTWIDTH)
    x0 = TFTWIDTH - 1;
  if (y0 >= TFTHEIGHT)
    y0 = TFTHEIGHT - 1;
  if (x1 >= TFTWIDTH)
    x1 = TFTWIDTH - 1;
  if (y1 >= TFTHEIGHT)
    y1 = TFTHEIGHT - 1;
  
  writeCommand(SSD1331_CMD_DRAWLINE);
  writeCommand(x0);
  writeCommand(y0);
  writeCommand(x1);
  writeCommand(y1);
  // delayMicroseconds(SSD1331_DELAYS_HWLINE);
  writeCommand((uint8_t)((color >> 11) << 1));
  writeCommand((uint8_t)((color >> 5) & 0x3F));
  writeCommand((uint8_t)((color << 1) & 0x3F));
  delayMicroseconds(SSD1331_DELAYS_HWLINE);
}

template<uint8_t CS_num, uint8_t RS_num, uint8_t SID_num, uint8_t SCLK_num, uint8_t RST_num>
void Adafruit_SSD1331<CS_num, RS_num, SID_num, SCLK_num, RST_num>::init(void) {
    // Initialization Sequence
    writeCommand(SSD1331_CMD_DISPLAYOFF);  	// 0xAE
    writeCommand(SSD1331_CMD_SETREMAP); 	// 0xA0
#if defined SSD1331_COLORORDER_RGB
    writeCommand(0x72);				// RGB Color
#else
    writeCommand(0x76);				// BGR Color
#endif
    writeCommand(SSD1331_CMD_STARTLINE); 	// 0xA1
    writeCommand(0x0);
    writeCommand(SSD1331_CMD_DISPLAYOFFSET); 	// 0xA2
    writeCommand(0x0);
    writeCommand(SSD1331_CMD_NORMALDISPLAY);  	// 0xA4
    writeCommand(SSD1331_CMD_SETMULTIPLEX); 	// 0xA8
    writeCommand(0x3F);  			// 0x3F 1/64 duty
    writeCommand(SSD1331_CMD_SETMASTER);  	// 0xAD
    writeCommand(0x8E);
    writeCommand(SSD1331_CMD_POWERMODE);  	// 0xB0
    writeCommand(0x0B);
    writeCommand(SSD1331_CMD_PRECHARGE);  	// 0xB1
    writeCommand(0x31);
    writeCommand(SSD1331_CMD_CLOCKDIV);  	// 0xB3
    writeCommand(0xF0);  // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
    writeCommand(SSD1331_CMD_PRECHARGEA);  	// 0x8A
    writeCommand(0x64);
    writeCommand(SSD1331_CMD_PRECHARGEB);  	// 0x8B
    writeCommand(0x78);
    writeCommand(SSD1331_CMD_PRECHARGEA);  	// 0x8C
    writeCommand(0x64);
    writeCommand(SSD1331_CMD_PRECHARGELEVEL);  	// 0xBB
    writeCommand(0x3A);
    writeCommand(SSD1331_CMD_VCOMH);  		// 0xBE
    writeCommand(0x3E);
    writeCommand(SSD1331_CMD_MASTERCURRENT);  	// 0x87
    writeCommand(0x06);
    writeCommand(SSD1331_CMD_CONTRASTA);  	// 0x81
    writeCommand(0x91);
    writeCommand(SSD1331_CMD_CONTRASTB);  	// 0x82
    writeCommand(0x50);
    writeCommand(SSD1331_CMD_CONTRASTC);  	// 0x83
    writeCommand(0x7D);
    writeCommand(SSD1331_CMD_DISPLAYON);	//--turn on oled panel    
}

#endif /* end of include guard: ADAFRUIT_SSD1331_H */
