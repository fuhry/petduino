/*
 *    ScrollingText.ino - Scrolling text example using the Petduino library
 *    Copyright (c) 2016 Circuitbeard
 *    Portions contributed by Dan Fuhry <dan@fuhry.com>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    This permission notice shall be included in all copies or
 *    substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#include <avr/pgmspace.h>
#include <LedControl.h>
#include <Petduino.h>
#include "font_thin.h"

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8
#define TIMESTEP 35

const char displaystr[] PROGMEM = { "HELLO WORLD #CIRCUITBEARD 2016  " };
font_char_t glyph;
byte buf[CHAR_HEIGHT];

int charptr, colptr;

Petduino pet = Petduino();

void setup() {
  pet.begin();
  charptr = colptr = 0;
}

void loop() {
  int i;
  int fontidx;
  byte letter;

  pet.update();
  
  // shift every current column one pixel to the left, blanking
  // the rightmost column
  for (i = 0; i < CHAR_HEIGHT; i++) {
    buf[i] <<= 1;
  }

  // which character are we working on?
  letter = pgm_read_byte_near(&(displaystr[charptr]));
  fontidx = letter - FONT_CHAR_BASE;

  // safety check, ensure we are not out of bounds
  if (letter < FONT_CHAR_BASE || fontidx > sizeof(font)) {
    // if out of bounds, print a space
    letter = FONT_CHAR_BASE;
  }
  
  // fetch glyph from program memory
  for (i = 0; i < CHAR_HEIGHT; i++) {
    glyph.glyph[i] = pgm_read_byte_near(&(font[fontidx].glyph[i]));
  }
  glyph.width = pgm_read_byte_near(&(font[fontidx].width));

  // kerning data may instruct us to draw a character with a width
  // of 9 px, but that's beyond the glyph buffer width so the logic
  // here exists to draw blank columns when out of bounds from the
  // glyph width.
  if ( colptr < CHAR_WIDTH ) {
    for (i = 0; i < CHAR_HEIGHT; i++) {
      // shift the current column all the way to the right, compute
      // a "1" or "0" and set the rightmost column on the display
      // as appropriate.
      buf[i] |= (glyph.glyph[i] >> (CHAR_WIDTH-1 - colptr)) & 1;
    }
  }

  // increment column pointer
  ++colptr;

  // if we've exceeded the width of this glyph, move on to the next
  // one
  if (colptr >= glyph.width) {
    colptr = 0;
    ++charptr;
  }

  // if we've reached the end of the string, reset to the beginning
  if (charptr == strlen_P(displaystr)) {
    charptr = 0;
  }

  // end draw code, display image and move on to the next iteration
  pet.drawImage(buf);
  delay(TIMESTEP);
}
