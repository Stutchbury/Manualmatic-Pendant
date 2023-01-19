/**
 * The various icons used on the button row.
 * 
 * GPLv2 Licence https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 * 
 * Copyright (c) 2022 Philip Fletcher <philip.fletcher@stutchbury.com>
 * 
 */


/** ***************************************************************

*/

#include <Arduino.h>
#include <math.h>
#include "ManualmaticIcons.h"
#include "ManualmaticUtils.h"


ManualmaticIcons::ManualmaticIcons(Adafruit_GFX& gfx)
  : gfx(gfx) {}


void ManualmaticIcons::fillOctagon(Coords_s cp, uint8_t r, uint16_t colour) {
  float a = 22.5;
  Coords_s c1 = { round(r * cos(degree2radian(a)) + cp.x), round(r * sin(degree2radian(a)) + cp.y) };
  Coords_s c2 = {0, 0};
  for (uint8_t i = 0; i < 8; i++ ) {
    a += 45;
    c2 = { round(r * cos(degree2radian(a)) + cp.x), round(r * sin(degree2radian(a)) + cp.y) };
    gfx.fillTriangle(cp.x, cp.y, c1.x, c1.y, c2.x, c2.y, colour);
    c1.x = c2.x;
    c1.y = c2.y;
  }

}


void ManualmaticIcons::drawIconCancel(Coords_s cp, uint16_t fgColour/*=RED*/, uint8_t r/*=14*/, uint8_t t /*=4*/ ) {
    gfx.fillCircle(cp.x, cp.y, r, fgColour);
    gfx.fillCircle(cp.x, cp.y, r - t, WHITE);
    int8_t h = floor(t/2); //half thickness
    r = round(r*0.7);
    //      '/'                BL                   TR                     BR  
    gfx.fillTriangle(cp.x-r-h, cp.y+r-h,     cp.x+r+h, cp.y-r+h,    cp.x-r+h, cp.y+r+h, fgColour);
    //      '/'                BL                   TR                     TL  
    gfx.fillTriangle(cp.x-r-h, cp.y+r-h,     cp.x+r+h, cp.y-r+h,    cp.x+r-h, cp.y-r-h, fgColour);    
}


/**
   Draw an 'X' centred at Coords, approx radius r, thickness t
*/
void ManualmaticIcons::drawIconX(Coords_s cp, uint16_t c/*=RED*/, uint8_t r/*=10*/, uint8_t t/*=5*/ ) {  
  int8_t h = t/2; //half thickness
  r = round(r*1.3);
  //      '/'                BL                   TR                     BR  
  gfx.fillTriangle(cp.x-r-h, cp.y+r-h,     cp.x+r+h, cp.y-r+h,    cp.x-r+h, cp.y+r+h, c);
  //      '/'                BL                   TR                     TL  
  gfx.fillTriangle(cp.x-r-h, cp.y+r-h,     cp.x+r+h, cp.y-r+h,    cp.x+r-h, cp.y-r-h, c);  
  //      '\'                BR                   TL                     BL  
  gfx.fillTriangle(cp.x+r+h, cp.y+r-h,     cp.x-r-h, cp.y-r+h,    cp.x+r-h, cp.y+r+h, c);
  //      '\'                BR                   TL                     TR  
  gfx.fillTriangle(cp.x+r+h, cp.y+r-h,     cp.x-r-h, cp.y-r+h,    cp.x-r+h, cp.y-r-h, c);
}


void ManualmaticIcons::drawIconTick(Coords_s cp, uint8_t r/*=10*/, uint8_t t/*=4*/, uint16_t c/*=DARKGREEN*/ ) {  
  uint8_t h = t/2; //half thickness
  r = round(r*1.3);
  Coords_s tr = {cp.x+r, cp.y-r-(h/2)};
  Coords_s b = { cp.x-r+t+h, cp.y+r+h };
  Coords_s tl = { cp.x-r-h, cp.y+h };
//  //      '/'       B               TR                V  
  gfx.fillTriangle(b.x, b.y+h,     tr.x, tr.y,    b.x, b.y-t-h, c);
  //                   V               T
  gfx.fillTriangle(b.x, b.y-t-h,     tr.x, tr.y,    tr.x-h, tr.y-h, c);
  //      '\'         B
  gfx.fillTriangle(b.x, b.y+h,  tl.x-h, tl.y+h,   tl.x+h, tl.y-h, c);
  //                  B             V
  gfx.fillTriangle(b.x, b.y+h,  b.x, b.y-t-h,    tl.x+h, tl.y-h,     c);
}

// void ManualmaticIcons::drawIconTap(Coords_s cp, uint16_t c/*=WHITE*/) {
//   //uint8_t w = 63;
//   uint8_t h = 37;
//   //gfx.drawRect(cp.x-(w/2), cp.y-(h/2), w, h, c);
//   gfx.fillRect(cp.x-5-5, cp.y-13, 10, 3, c); //handle
//   gfx.fillCircle(cp.x-5+7, cp.y-12, 3, c); //end
//   gfx.fillCircle(cp.x-5-7, cp.y-12, 3, c); //end
//   gfx.fillRect(cp.x-5-1, cp.y-13, 3, 8, c); //shaft
//   gfx.fillCircle(cp.x-5, cp.y, 7, c); //valve body
//   gfx.fillRect(cp.x-19, cp.y-4, 30, 8, c); //body
//   gfx.fillRect(cp.x+9, cp.y, 8, 8, c); //spout
//   gfx.fillCircle(cp.x+11, cp.y, 4, c); //curve
//   gfx.fillCircle(cp.x+12, cp.y+(h/2)-3, 3, c); //drip
//   gfx.fillCircle(cp.x+12, cp.y+(h/2)-6, 1, c); //drip top
// }
//void drawTouchIconTap(TouchKey& tk) {
//  Coords_s cp = { tk.xCl(), tk.yCl()};
//  drawIconTap(cp);
//}

void ManualmaticIcons::drawIconTap(Coords_s cp, uint16_t c/*=WHITE*/) {
  gfx.fillRect(cp.x-5-5, cp.y-14, 10, 3, c); //handle
  gfx.fillCircle(cp.x-5+7, cp.y-13, 3, c); //end
  gfx.fillCircle(cp.x-5-7, cp.y-13, 3, c); //end
  gfx.fillRect(cp.x-5-1, cp.y-14, 2, 8, c); //handle shaft
  gfx.fillCircle(cp.x-5, cp.y-2, 7, c); //valve body
  gfx.fillRect(cp.x-19, cp.y-5, 30, 7, c); //body
  gfx.fillRect(cp.x+9, cp.y-1, 7, 5, c); //spout
  gfx.fillCircle(cp.x+11, cp.y-1, 3, c); //curve
}

void ManualmaticIcons::drawIconCoolant(Coords_s cp, uint16_t c/*=WHITE*/) {
  cp.y += 4;
  drawIconTap(cp, c);
}

void ManualmaticIcons::drawIconMist(Coords_s cp, uint16_t c/*=WHITE*/) {
  drawIconTap(cp, c);
  gfx.drawCircle(cp.x+12, cp.y+7, 3, c); //drip
  gfx.drawCircle(cp.x+20, cp.y+12, 3, c); //drip
  gfx.drawCircle(cp.x+4, cp.y+12, 3, c); //drip
  gfx.drawCircle(cp.x+15, cp.y+15, 3, c); //drip
  gfx.drawCircle(cp.x+9, cp.y+15, 3, c); //drip
  
}

void ManualmaticIcons::drawIconFlood(Coords_s cp, uint16_t c/*=WHITE*/) {
  drawIconTap(cp, c);
  for (int8_t i=-3; i<4; i++ ) {
    gfx.drawLine(cp.x+12+i, cp.y+5, cp.x+12+(i*4), cp.y+17, c);
  }
}

void ManualmaticIcons::drawIconMistFlood(Coords_s cp, uint16_t c/*=WHITE*/) {
  drawIconMist(cp, c);
  drawIconFlood(cp, c);
}


void ManualmaticIcons::drawIconTouchOff(Coords_s cp, uint16_t c/*=WHITE*/) {
  //gfx.drawRect(cp.x-(w/2), cp.y-(h/2), w, h, c);
  gfx.fillRect(cp.x-4, cp.y-15, 9, 8, c); //shaft
  gfx.fillRect(cp.x-1, cp.y-12, 3, 20, c); //shaft
  gfx.fillCircle(cp.x, cp.y+7, 3, c); //ruby
  gfx.drawLine(cp.x-6, cp.y+9, cp.x-10, cp.y+5, c);
  gfx.drawLine(cp.x+6, cp.y+9, cp.x+10, cp.y+5, c);
  gfx.drawRect(cp.x-15, cp.y+12, 30, 5, c); //stock
}

void ManualmaticIcons::drawIconHalf(Coords_s cp, int c/*=WHITE*/) {
    gfx.setTextColor(c);
    gfx.setFont(&FreeSansBold12pt7b);
    gfx.setCursor(cp.x-19, cp.y+3);
    gfx.print("1");
    gfx.setFont(&FreeSansBold18pt7b);
    gfx.setCursor(cp.x-6, cp.y+12);
    gfx.print("/");
    gfx.setFont(&FreeSansBold12pt7b);
    gfx.setCursor(cp.x+3, cp.y+12);
    gfx.print("2");
}


void ManualmaticIcons::drawIconHalt(Coords_s cp, uint16_t c/*=RED*/ ) {
    uint8_t r = 15;
    fillOctagon(cp, r, c);
    drawIconX(cp, WHITE, 5, 3);
}


void ManualmaticIcons::drawIconOneStep(Coords_s cp, uint16_t c/*=WHITE*/) {
    uint16_t x = cp.x - 12;
    uint8_t y = cp.y - 14;
    uint8_t s = 8; //step size
    gfx.setFont(&FreeMonoBold18pt7b);
    gfx.setTextColor(c);
    gfx.setCursor(x - 10, cp.y+12);
    gfx.print("1");
    for ( uint8_t i = 0; i < 4; (i++) ) {
      gfx.drawLine(x + (s * i), y + (s * i), x + (s * i) + s, y + (s * i), c);
      gfx.drawLine(x + (s * i) + s, y + (s * i), x + (s * i) + s, y + (s * i) + s, c);
    }
    gfx.setTextColor(WHITE);
}



void ManualmaticIcons::drawIconPause(Coords_s cp, uint16_t c/*=WHITE*/, uint8_t h/*=27*/) {
    uint8_t w = round(h / 3);
    uint16_t x = cp.x - floor(w * 1.3);
    uint8_t y = cp.y - floor(h / 2);
    gfx.fillRect(x, y, w, h, c);
    gfx.fillRect(x + (w * 1.8), y, w, h, c);
}

void ManualmaticIcons::drawIconPlay(Coords_s cp, uint16_t c/*=WHITE*/, uint8_t h/*=26*/) {
    uint8_t w = h / 1.5;
    gfx.fillTriangle(
      cp.x-(w/2)+3, cp.y-(h/2),
      cp.x+(w/2)+3, cp.y,
      cp.x-(w/2)+3, cp.y+(h/2),
      c
    );
}

void ManualmaticIcons::drawIconStop(Coords_s cp, int c/*=WHITE*/, uint8_t h/*=24*/) {
    gfx.fillRect((cp.x-floor(h / 2)), (cp.y-floor(h / 2)), h, h, c);
}


void ManualmaticIcons::drawJoystickMarker(Coords_s cp) {
    gfx.fillTriangle(
      0, cp.y+5, //axisDisplayY[i],
      10, cp.y+15, //axisDisplayY[i] + 15,
      0, cp.y+25, //axisDisplayY[i] + 30,
      ORANGE
    );
    // gfx.drawTriangle(
    //   0, cp.y+1, //axisDisplayY[i],
    //   14, cp.y+15, //axisDisplayY[i] + 15,
    //   0, cp.y+29, //axisDisplayY[i] + 30,
    //   RED
    // );
}

/** ***************************************************************

*/
void ManualmaticIcons::drawEstopped(bool forceRefresh /*= false*/) {
  if ( forceRefresh ) {
    gfx.fillScreen(BLACK);
    uint8_t r = 110;
    Coords_s cp = { 160, 120 }; //centre point
    uint8_t lw = 7;
    //Could put this on a canvas first but I quite like the 'animation'.
    fillOctagon(cp, r, RED);
    fillOctagon(cp, r - lw, WHITE);
    fillOctagon(cp, r - (lw * 2), RED);
    gfx.drawBitmap(cp.x - 55, cp.y - 80, stop_hand, 128, 160, WHITE);

  }
}
