#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "lcd_backlight.hpp"
#include <cstdint>


TFT_eSPI tft = TFT_eSPI();
static LCDBackLight backLight;

static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];


#if USE_LV_LOG != 0
void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *dsc)
{
    Serial.printf("%s@%d->%s\r\n", file, line, dsc);
    delay(100);
}
#endif

void my_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint16_t c;

    tft.startWrite(); /* Start new TFT transaction */
    tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
    for (int y = area->y1; y <= area->y2; y++) {
        for (int x = area->x1; x <= area->x2; x++) {
            c = color_p->full;
            tft.writeColor(c, 1);
            color_p++;
        }
    }
    tft.endWrite(); /* terminate TFT transaction */
    lv_disp_flush_ready(disp);
}

/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
    return millis();
}


void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */

    lv_init();

#if USE_LV_LOG != 0
    lv_log_register_print(my_print); /* register print function for debugging */
#endif

    tft.begin(); /* TFT init */
    tft.setRotation(3); /* Landscape orientation */
    //tft.fillScreen(TFT_BLACK);
    backLight.initialize();
    backLight.setBrightness(50);
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 320;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = my_flush;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    lv_obj_t *label1 = lv_label_create(lv_scr_act());
    lv_obj_set_pos(label1, 140,120);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label1, LV_HOR_RES_MAX - 30);
    lv_label_set_text(label1, "LVGL ON WIO");

}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5); /* let this time pass */
}