
#include "mcu_api.h"
#include "stdlib.h"
#include "string.h"

void cycle_pixels();

static Wire pixels;

void pixel_cycle_lo(Wire * w);
void pixel_cycle_hi(Wire * w);

#define BITMASK_0_4 0x1F
#define BITMASK_0_5 0x3F
#define COLOR_16BIT_BGR(R,G,B) ((B&BITMASK_0_4) | (G&BITMASK_0_5) << 5 | (R&BITMASK_0_4) << 11)

#define NUM_PIXELS 3
static int pixel_pattern[NUM_PIXELS] = {COLOR_16BIT_BGR(0xff,0x00,0x00),
                              COLOR_16BIT_BGR(0x00,0xff,0x00),
                              COLOR_16BIT_BGR(0x00,0x00,0xff)};

#define BITBANG_ON(P) { \
   gpio_write(P, 1); \
   gpio_write(P, 1); \
   gpio_write(P, 0); }

#define BITBANG_OFF(P) { \
   gpio_write(P, 1); \
   gpio_write(P, 0); \
   gpio_write(P, 0); }


// #define BITBANG_ON(P) { \
// 	Wire_write(P, 1); \
// 	Wire_write(P, 1); \
// 	Wire_write(P, 0); }

// #define BITBANG_OFF(P) { \
// 	Wire_write(P, 1); \
// 	Wire_write(P, 0); \
// 	Wire_write(P, 0); }


//#define BITBANG_16(P,B) {
//	(B >> 0) ? BITBANG_ON(P) : BITBANG_OFF(P) }

#define BITBANG_24(P,B) { \
   if((B >> 0 ) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 1 ) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 2 ) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 3 ) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 4 ) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 5 ) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 6 ) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 7 ) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 8 ) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 9 ) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 10) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 11) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 12) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 13) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 14) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 15) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 16) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 17) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 18) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 19) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 20) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 21) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 22) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 23) & 1) BITBANG_ON(P) else BITBANG_OFF(P) \
   if((B >> 24) & 1) BITBANG_ON(P) else BITBANG_OFF(P) }

void cycle_pixels_sliced_run()
{
   int i;
   int num_writes = 0;
   unsigned long bang_time;
   //Wire_Wire(&pixels);
   //Wire_connect(&pixels, 15, WIRE_DIRECTION_OUT);
   debug_print (DBG_INFO, "cycle_pixels: STARTING\n");
   while (1)
   {
      bang_time = time_us();
      //BITBANG_24(&pixels,0)
      BITBANG_24(49,0)
      for (i=0; i < NUM_PIXELS; i++)
      {
         //BITBANG_24(&pixels,pixel_pattern[i])
         BITBANG_24(49,pixel_pattern[i])
      }
      //BITBANG_24(&pixels,0)
      BITBANG_24(49,0)
      bang_time = time_us() - bang_time;
      {
         char pbuf[64];
         unsigned long bang_rate = (NUM_PIXELS + 2) * 24 * 3 * (1000000 / bang_time);
         num_writes++;
         mcu_snprintf(pbuf, 64, "wrote pixels: %d[%u kbits/s]\n", num_writes, bang_rate / 1000);
         debug_print (DBG_INFO, pbuf);
      }
      mcu_sleep(1000);
   }
}

void pixel_cycle_lo(Wire * w)
{
   //---- T0H ----
   Wire_write(w, 1);
   //---- T0L ----
   Wire_write(w, 0);
   Wire_write(w, 0);
   Wire_write(w, 0);
   Wire_write(w, 0);
}
void pixel_cycle_hi(Wire * w)
{
   //---- T1H ----
   Wire_write(w, 1);
   Wire_write(w, 1);
   Wire_write(w, 1);
   Wire_write(w, 1);
   //---- T1L ----
   Wire_write(w, 0);

}

