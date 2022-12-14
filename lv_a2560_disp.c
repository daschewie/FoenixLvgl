/**
 * @file lv_a2560_disp.c
 *
 */

/*Copy this file as "lv_a2560_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_a2560_disp.h"
#include "mcp/syscalls.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
    #define MY_DISP_HOR_RES    640
#endif

#ifndef MY_DISP_VER_RES
    #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    480
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/
static unsigned char rgb332[] = {
  0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x55, 0xff, 0x00, 0x00, 0xaa,
  0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x24, 0x00, 0xff, 0x00, 0x24, 0x55,
  0xff, 0x00, 0x24, 0xaa, 0xff, 0x00, 0x24, 0xff, 0xff, 0x00, 0x48, 0x00,
  0xff, 0x00, 0x48, 0x55, 0xff, 0x00, 0x48, 0xaa, 0xff, 0x00, 0x48, 0xff,
  0xff, 0x00, 0x6d, 0x00, 0xff, 0x00, 0x6d, 0x55, 0xff, 0x00, 0x6d, 0xaa,
  0xff, 0x00, 0x6d, 0xff, 0xff, 0x00, 0x91, 0x00, 0xff, 0x00, 0x91, 0x55,
  0xff, 0x00, 0x91, 0xaa, 0xff, 0x00, 0x91, 0xff, 0xff, 0x00, 0xb6, 0x00,
  0xff, 0x00, 0xb6, 0x55, 0xff, 0x00, 0xb6, 0xaa, 0xff, 0x00, 0xb6, 0xff,
  0xff, 0x00, 0xda, 0x00, 0xff, 0x00, 0xda, 0x55, 0xff, 0x00, 0xda, 0xaa,
  0xff, 0x00, 0xda, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x55,
  0xff, 0x00, 0xff, 0xaa, 0xff, 0x00, 0xff, 0xff, 0xff, 0x24, 0x00, 0x00,
  0xff, 0x24, 0x00, 0x55, 0xff, 0x24, 0x00, 0xaa, 0xff, 0x24, 0x00, 0xff,
  0xff, 0x24, 0x24, 0x00, 0xff, 0x24, 0x24, 0x55, 0xff, 0x24, 0x24, 0xaa,
  0xff, 0x24, 0x24, 0xff, 0xff, 0x24, 0x48, 0x00, 0xff, 0x24, 0x48, 0x55,
  0xff, 0x24, 0x48, 0xaa, 0xff, 0x24, 0x48, 0xff, 0xff, 0x24, 0x6d, 0x00,
  0xff, 0x24, 0x6d, 0x55, 0xff, 0x24, 0x6d, 0xaa, 0xff, 0x24, 0x6d, 0xff,
  0xff, 0x24, 0x91, 0x00, 0xff, 0x24, 0x91, 0x55, 0xff, 0x24, 0x91, 0xaa,
  0xff, 0x24, 0x91, 0xff, 0xff, 0x24, 0xb6, 0x00, 0xff, 0x24, 0xb6, 0x55,
  0xff, 0x24, 0xb6, 0xaa, 0xff, 0x24, 0xb6, 0xff, 0xff, 0x24, 0xda, 0x00,
  0xff, 0x24, 0xda, 0x55, 0xff, 0x24, 0xda, 0xaa, 0xff, 0x24, 0xda, 0xff,
  0xff, 0x24, 0xff, 0x00, 0xff, 0x24, 0xff, 0x55, 0xff, 0x24, 0xff, 0xaa,
  0xff, 0x24, 0xff, 0xff, 0xff, 0x48, 0x00, 0x00, 0xff, 0x48, 0x00, 0x55,
  0xff, 0x48, 0x00, 0xaa, 0xff, 0x48, 0x00, 0xff, 0xff, 0x48, 0x24, 0x00,
  0xff, 0x48, 0x24, 0x55, 0xff, 0x48, 0x24, 0xaa, 0xff, 0x48, 0x24, 0xff,
  0xff, 0x48, 0x48, 0x00, 0xff, 0x48, 0x48, 0x55, 0xff, 0x48, 0x48, 0xaa,
  0xff, 0x48, 0x48, 0xff, 0xff, 0x48, 0x6d, 0x00, 0xff, 0x48, 0x6d, 0x55,
  0xff, 0x48, 0x6d, 0xaa, 0xff, 0x48, 0x6d, 0xff, 0xff, 0x48, 0x91, 0x00,
  0xff, 0x48, 0x91, 0x55, 0xff, 0x48, 0x91, 0xaa, 0xff, 0x48, 0x91, 0xff,
  0xff, 0x48, 0xb6, 0x00, 0xff, 0x48, 0xb6, 0x55, 0xff, 0x48, 0xb6, 0xaa,
  0xff, 0x48, 0xb6, 0xff, 0xff, 0x48, 0xda, 0x00, 0xff, 0x48, 0xda, 0x55,
  0xff, 0x48, 0xda, 0xaa, 0xff, 0x48, 0xda, 0xff, 0xff, 0x48, 0xff, 0x00,
  0xff, 0x48, 0xff, 0x55, 0xff, 0x48, 0xff, 0xaa, 0xff, 0x48, 0xff, 0xff,
  0xff, 0x6d, 0x00, 0x00, 0xff, 0x6d, 0x00, 0x55, 0xff, 0x6d, 0x00, 0xaa,
  0xff, 0x6d, 0x00, 0xff, 0xff, 0x6d, 0x24, 0x00, 0xff, 0x6d, 0x24, 0x55,
  0xff, 0x6d, 0x24, 0xaa, 0xff, 0x6d, 0x24, 0xff, 0xff, 0x6d, 0x48, 0x00,
  0xff, 0x6d, 0x48, 0x55, 0xff, 0x6d, 0x48, 0xaa, 0xff, 0x6d, 0x48, 0xff,
  0xff, 0x6d, 0x6d, 0x00, 0xff, 0x6d, 0x6d, 0x55, 0xff, 0x6d, 0x6d, 0xaa,
  0xff, 0x6d, 0x6d, 0xff, 0xff, 0x6d, 0x91, 0x00, 0xff, 0x6d, 0x91, 0x55,
  0xff, 0x6d, 0x91, 0xaa, 0xff, 0x6d, 0x91, 0xff, 0xff, 0x6d, 0xb6, 0x00,
  0xff, 0x6d, 0xb6, 0x55, 0xff, 0x6d, 0xb6, 0xaa, 0xff, 0x6d, 0xb6, 0xff,
  0xff, 0x6d, 0xda, 0x00, 0xff, 0x6d, 0xda, 0x55, 0xff, 0x6d, 0xda, 0xaa,
  0xff, 0x6d, 0xda, 0xff, 0xff, 0x6d, 0xff, 0x00, 0xff, 0x6d, 0xff, 0x55,
  0xff, 0x6d, 0xff, 0xaa, 0xff, 0x6d, 0xff, 0xff, 0xff, 0x91, 0x00, 0x00,
  0xff, 0x91, 0x00, 0x55, 0xff, 0x91, 0x00, 0xaa, 0xff, 0x91, 0x00, 0xff,
  0xff, 0x91, 0x24, 0x00, 0xff, 0x91, 0x24, 0x55, 0xff, 0x91, 0x24, 0xaa,
  0xff, 0x91, 0x24, 0xff, 0xff, 0x91, 0x48, 0x00, 0xff, 0x91, 0x48, 0x55,
  0xff, 0x91, 0x48, 0xaa, 0xff, 0x91, 0x48, 0xff, 0xff, 0x91, 0x6d, 0x00,
  0xff, 0x91, 0x6d, 0x55, 0xff, 0x91, 0x6d, 0xaa, 0xff, 0x91, 0x6d, 0xff,
  0xff, 0x91, 0x91, 0x00, 0xff, 0x91, 0x91, 0x55, 0xff, 0x91, 0x91, 0xaa,
  0xff, 0x91, 0x91, 0xff, 0xff, 0x91, 0xb6, 0x00, 0xff, 0x91, 0xb6, 0x55,
  0xff, 0x91, 0xb6, 0xaa, 0xff, 0x91, 0xb6, 0xff, 0xff, 0x91, 0xda, 0x00,
  0xff, 0x91, 0xda, 0x55, 0xff, 0x91, 0xda, 0xaa, 0xff, 0x91, 0xda, 0xff,
  0xff, 0x91, 0xff, 0x00, 0xff, 0x91, 0xff, 0x55, 0xff, 0x91, 0xff, 0xaa,
  0xff, 0x91, 0xff, 0xff, 0xff, 0xb6, 0x00, 0x00, 0xff, 0xb6, 0x00, 0x55,
  0xff, 0xb6, 0x00, 0xaa, 0xff, 0xb6, 0x00, 0xff, 0xff, 0xb6, 0x24, 0x00,
  0xff, 0xb6, 0x24, 0x55, 0xff, 0xb6, 0x24, 0xaa, 0xff, 0xb6, 0x24, 0xff,
  0xff, 0xb6, 0x48, 0x00, 0xff, 0xb6, 0x48, 0x55, 0xff, 0xb6, 0x48, 0xaa,
  0xff, 0xb6, 0x48, 0xff, 0xff, 0xb6, 0x6d, 0x00, 0xff, 0xb6, 0x6d, 0x55,
  0xff, 0xb6, 0x6d, 0xaa, 0xff, 0xb6, 0x6d, 0xff, 0xff, 0xb6, 0x91, 0x00,
  0xff, 0xb6, 0x91, 0x55, 0xff, 0xb6, 0x91, 0xaa, 0xff, 0xb6, 0x91, 0xff,
  0xff, 0xb6, 0xb6, 0x00, 0xff, 0xb6, 0xb6, 0x55, 0xff, 0xb6, 0xb6, 0xaa,
  0xff, 0xb6, 0xb6, 0xff, 0xff, 0xb6, 0xda, 0x00, 0xff, 0xb6, 0xda, 0x55,
  0xff, 0xb6, 0xda, 0xaa, 0xff, 0xb6, 0xda, 0xff, 0xff, 0xb6, 0xff, 0x00,
  0xff, 0xb6, 0xff, 0x55, 0xff, 0xb6, 0xff, 0xaa, 0xff, 0xb6, 0xff, 0xff,
  0xff, 0xda, 0x00, 0x00, 0xff, 0xda, 0x00, 0x55, 0xff, 0xda, 0x00, 0xaa,
  0xff, 0xda, 0x00, 0xff, 0xff, 0xda, 0x24, 0x00, 0xff, 0xda, 0x24, 0x55,
  0xff, 0xda, 0x24, 0xaa, 0xff, 0xda, 0x24, 0xff, 0xff, 0xda, 0x48, 0x00,
  0xff, 0xda, 0x48, 0x55, 0xff, 0xda, 0x48, 0xaa, 0xff, 0xda, 0x48, 0xff,
  0xff, 0xda, 0x6d, 0x00, 0xff, 0xda, 0x6d, 0x55, 0xff, 0xda, 0x6d, 0xaa,
  0xff, 0xda, 0x6d, 0xff, 0xff, 0xda, 0x91, 0x00, 0xff, 0xda, 0x91, 0x55,
  0xff, 0xda, 0x91, 0xaa, 0xff, 0xda, 0x91, 0xff, 0xff, 0xda, 0xb6, 0x00,
  0xff, 0xda, 0xb6, 0x55, 0xff, 0xda, 0xb6, 0xaa, 0xff, 0xda, 0xb6, 0xff,
  0xff, 0xda, 0xda, 0x00, 0xff, 0xda, 0xda, 0x55, 0xff, 0xda, 0xda, 0xaa,
  0xff, 0xda, 0xda, 0xff, 0xff, 0xda, 0xff, 0x00, 0xff, 0xda, 0xff, 0x55,
  0xff, 0xda, 0xff, 0xaa, 0xff, 0xda, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
  0xff, 0xff, 0x00, 0x55, 0xff, 0xff, 0x00, 0xaa, 0xff, 0xff, 0x00, 0xff,
  0xff, 0xff, 0x24, 0x00, 0xff, 0xff, 0x24, 0x55, 0xff, 0xff, 0x24, 0xaa,
  0xff, 0xff, 0x24, 0xff, 0xff, 0xff, 0x48, 0x00, 0xff, 0xff, 0x48, 0x55,
  0xff, 0xff, 0x48, 0xaa, 0xff, 0xff, 0x48, 0xff, 0xff, 0xff, 0x6d, 0x00,
  0xff, 0xff, 0x6d, 0x55, 0xff, 0xff, 0x6d, 0xaa, 0xff, 0xff, 0x6d, 0xff,
  0xff, 0xff, 0x91, 0x00, 0xff, 0xff, 0x91, 0x55, 0xff, 0xff, 0x91, 0xaa,
  0xff, 0xff, 0x91, 0xff, 0xff, 0xff, 0xb6, 0x00, 0xff, 0xff, 0xb6, 0x55,
  0xff, 0xff, 0xb6, 0xaa, 0xff, 0xff, 0xb6, 0xff, 0xff, 0xff, 0xda, 0x00,
  0xff, 0xff, 0xda, 0x55, 0xff, 0xff, 0xda, 0xaa, 0xff, 0xff, 0xda, 0xff,
  0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x55, 0xff, 0xff, 0xff, 0xaa,
  0xff, 0xff, 0xff, 0xff
};
static unsigned int rgb332_len = 1024;


/**********************
 *      MACROS
 **********************/
#define R32(x)	*((volatile unsigned long* const)(x))	// make sure we read a 32 bit long; for VICKY registers, etc.
#define P32(x)	(volatile unsigned long* const)(x)		// make sure we read a 32 bit long; for VICKY registers, etc.
#define R16(x)	*((volatile unsigned short* const)(x))	// make sure we read an 16 bit short; for VICKY registers, etc.
#define P16(x)	(volatile unsigned short* const)(x)		// make sure we read an 16 bit short; for VICKY registers, etc.
#define R8(x)	*((volatile uint8_t* const)(x))			// make sure we read an 8 bit byte; for VICKY registers, etc.
#define P8(x)	(volatile uint8_t* const)(x)			// make sure we read an 8 bit byte; for VICKY registers, etc.

#define GRAPHICS_MODE_MASK		0xFFFFFF00	//!> the mask for the system control register that holds the graphics/bitmap/text/sprite mode bits
#define GRAPHICS_MODE_TEXT		0x01	// 0b00000001	Enable the Text Mode
#define GRAPHICS_MODE_TEXT_OVER	0x02	// 0b00000010	Enable the Overlay of the text mode on top of Graphic Mode (the Background Color is ignored)
#define GRAPHICS_MODE_GRAPHICS	0x04	// 0b00000100	Enable the Graphic Mode
#define GRAPHICS_MODE_EN_BITMAP	0x08	// 0b00001000	Enable the Bitmap Module In Vicky
#define GRAPHICS_MODE_EN_TILE	0x10	// 0b00010000	Enable the Tile Module in Vicky
#define GRAPHICS_MODE_EN_SPRITE	0x20	// 0b00100000	Enable the Sprite Module in Vicky
#define GRAPHICS_MODE_EN_GAMMA	0x40	// 0b01000000	Enable the GAMMA correction - The Analog and DVI have different color values, the GAMMA is great to correct the difference
#define GRAPHICS_MODE_DIS_VIDEO	0x80	// 0b10000000	This will disable the Scanning of the Video information in the 4Meg of VideoRAM hence giving 100% bandwidth to the CPU

#define BITMAP_L0_CTRL_L		        0x40	//!> the (long) offset from the VICKY control register to the bitmap layer0 control register (foreground layer)		
#define BITMAP_L0_VRAM_ADDR_OFFSET_L    0x41	//!> the (long) offset from the VICKY control register to the bitmap layer0 VRAM address pointer)

#define VICKY_A2560K_B				0xfec80000				// vicky III channel B control register
#define LUT_0                       0x2000
	


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_a2560_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    // /* Example for 2) */
    // static lv_disp_draw_buf_t draw_buf_dsc_2;
    // static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*A buffer for 10 rows*/
    // static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                        /*An other buffer for 10 rows*/
    // lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    // /* Example for 3) also set disp_drv.full_refresh = 1 below*/
    // static lv_disp_draw_buf_t draw_buf_dsc_3;
    // static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
    // static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
    // lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
    //                       MY_DISP_VER_RES * LV_VER_RES_MAX);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_1;

    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1;

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void sof_handler() {
	lv_tick_inc(16);
	sys_int_clear(INT_SOF_B);
}


/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    // Setup graphics mode
	R32(VICKY_A2560K_B) = (R32(VICKY_A2560K_B) & GRAPHICS_MODE_MASK | GRAPHICS_MODE_GRAPHICS | GRAPHICS_MODE_EN_BITMAP);
	R32(VICKY_A2560K_B + BITMAP_L0_CTRL_L) = 0x01;
    R32(VICKY_A2560K_B + BITMAP_L0_VRAM_ADDR_OFFSET_L) = 0;     // Start of VRAM

    // Load RGB332 palette
    memcpy(P8(VICKY_A2560K_B + LUT_0), rgb332, rgb332_len);

    // Enable SOF Interrupt
    sys_int_register(INT_SOF_B, sof_handler);
	sys_int_enable(INT_SOF_B);
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    if(disp_flush_enabled) {
        /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

        int32_t x;
        int32_t y;
        for(y = area->y1; y <= area->y2; y++) {
            for(x = area->x1; x <= area->x2; x++) {
                R8(VICKY_A2560K_B + (y * 640 + x)) = lv_color_to8(*color_p);
                color_p++;
            }
        }
    }

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
