#include "lcd.h"

char lcd_screen_chars[25][16];
unsigned short lcd_screen_chars_color[25][16];
unsigned short lcd_screen_x = 0, lcd_screen_y = 0;
unsigned short lcd_screencolor = LCD_COLOR_BLACK;


void lcd_command(unsigned char com) 
{
    LCD_OP = LCD_OP_CMD;
    LCD_CS = 0; // CS
    spi_write_byte(&com);
    LCD_CS = 1; // CS
}

void lcd_data(unsigned char dat) 
{
    LCD_OP = LCD_OP_DAT;
    LCD_CS = 0; // CS
    spi_write_byte(&dat);
    LCD_CS = 1; // CS
}

void lcd_data16(unsigned short dat) 
{
    LCD_OP = LCD_OP_DAT;
    LCD_CS = 0; // CS
    spi_io_short(&dat);
    LCD_CS = 1; // CS
}

void lcd_start() 
{
    LCD_CS_TRIS = 0;
    LCD_OP_TRIS = 0;
    
    LCD_CS = 1;
    
    spi_init();
    unsigned int time = 0;
    
    lcd_command(LCD_CMD_SWRESET);//software reset
    time = timer_start();
    while (!timer_timeout(time,500 * TIMER_MILLISECOND)) {} //delay(500);

	lcd_command(LCD_CMD_SLPOUT);//exit sleep
    time = timer_start();
	while (!timer_timeout(time,5 * TIMER_MILLISECOND)) {} //delay(5);

	lcd_command(LCD_CMD_PIXFMT);//Set Color Format 16bit
	lcd_data(0x05);
    time = timer_start();
	while (!timer_timeout(time,5 * TIMER_MILLISECOND)) {} //delay(5);

	lcd_command(LCD_CMD_GAMMASET);//default gamma curve 3
	lcd_data(0x04);//0x04;
    time = timer_start();
	while (!timer_timeout(time,TIMER_MILLISECOND)) {} //delay(1);

	lcd_command(LCD_CMD_GAMRSEL);//Enable Gamma adj
	lcd_data(0x01);
    time = timer_start();
	while (!timer_timeout(time,TIMER_MILLISECOND)) {} //delay(1);

	lcd_command(LCD_CMD_NORML);

	lcd_command(LCD_CMD_DFUNCTR);
	lcd_data(0b11111111);
	lcd_data(0b00000110);

    int i = 0;
	lcd_command(LCD_CMD_PGAMMAC);//Positive Gamma Correction Setting
	for (i=0;i<15;i++){
		lcd_data(pGammaSet[i]);
	}

	lcd_command(LCD_CMD_NGAMMAC);//Negative Gamma Correction Setting
	for (i=0;i<15;i++){
		lcd_data(nGammaSet[i]);
	}

	lcd_command(LCD_CMD_FRMCTR1);//Frame Rate Control (In normal mode/Full colors)
	lcd_data(0x08);//0x0C//0x08
	lcd_data(0x02);//0x14//0x08
    time = timer_start();
	while (!timer_timeout(time,TIMER_MILLISECOND)) {} //delay(1);

	lcd_command(LCD_CMD_DINVCTR); //display inversion
	lcd_data(0x07);
	time = _CP0_GET_COUNT();
	while (!timer_timeout(time,TIMER_MILLISECOND)) {} //delay(1);

	lcd_command(LCD_CMD_PWCTR1);//Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	lcd_data(0x0A);//4.30 - 0x0A
	lcd_data(0x02);//0x05
    time = timer_start();
	while (!timer_timeout(time,TIMER_MILLISECOND)) {} //delay(1);

	lcd_command(LCD_CMD_PWCTR2);//Set BT[2:0] for AVDD & VCL & VGH & VGL
	lcd_data(0x02);
    time = timer_start();
	while (!timer_timeout(time,TIMER_MILLISECOND)) {} //delay(1);

	lcd_command(LCD_CMD_VCOMCTR1);//Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	lcd_data(0x50);//0x50
	lcd_data(99);//0x5b
    time = timer_start();
	while (!timer_timeout(time,TIMER_MILLISECOND)) {} //delay(1);

	lcd_command(LCD_CMD_VCOMOFFS);
	lcd_data(0);//0x40
    time = timer_start();
	while (!timer_timeout(time,TIMER_MILLISECOND)) {} //delay(1);

	lcd_command(LCD_CMD_CLMADRS);//Set Column Address
	lcd_data16(0x00);
    lcd_data16(LCD_GRAMWIDTH);

	lcd_command(LCD_CMD_PGEADRS);//Set Page Address
	lcd_data16(0x00);
    lcd_data16(LCD_GRAMHEIGH);

	lcd_command(LCD_CMD_VSCLLDEF);
	lcd_data16(0); // __OFFSET
	lcd_data16(LCD_GRAMHEIGH); // _GRAMHEIGH - __OFFSET
	lcd_data16(0);

	lcd_command(LCD_CMD_MADCTL); // rotation
    lcd_data(0b00001000); // bit 3 0 for RGB, 1 for GBR, rotation: 0b00001000, 0b01101000, 0b11001000, 0b10101000

	lcd_command(LCD_CMD_DISPON);//display ON
    time = timer_start();
	while (!timer_timeout(time,TIMER_MILLISECOND)) {} //delay(1);

	lcd_command(LCD_CMD_RAMWR);//Memory Write
    
    lcd_clearScreen(LCD_COLOR_BLACK);
}

void lcd_printf (char *s, unsigned short color)
{
    while (*s)
    {
        switch (*s)
        {
            case '\n':
            {
                lcd_screen_x=0;
                lcd_screen_y++;
                if (lcd_screen_y==17)
                {
                    lcd_screen_y=0;
                }
                break;
            }
            default:
            {
                char newcdata[5];
                int i=0;
                char oldcdata[5];
                unsigned char rewrite = lcd_screen_chars_color[lcd_screen_x][lcd_screen_y] != color;
                for (i=0;i<5;i++)
                {
                    newcdata[i]=lcd_char_data[lcd_char_map[*s]][i];
                    if (!rewrite)
                        oldcdata[i] =lcd_char_data[lcd_screen_chars[lcd_screen_x][lcd_screen_y]][i];
                }
                int x_base = lcd_screen_x * 5;
                int y_base = lcd_screen_y * 8;
                int x=0,y=0;
                unsigned short c=0;
                unsigned short oldc=0;
                for (y=0;y<8;y++)
                    for (x=0;x<5;x++)
                    {
                        c=lcd_screencolor;
                        if (newcdata[x] & (1<<y))
                            c=color;
                        if (rewrite)
                        {
                            lcd_drawPixel(x_base+x, y_base+y, c);
                        }
                        else
                        {
                            oldc=lcd_screencolor;
                            if (oldcdata[x] & (1<<y))
                                oldc=color;
                            if (oldc!=c)
                                lcd_drawPixel(x_base+x, y_base+y, c);
                        }
                    }
                lcd_screen_x++;
                if (lcd_screen_x==26) 
                {
                    lcd_screen_x=0;
                    lcd_screen_y++;
                    if (lcd_screen_y==17)
                    {
                        lcd_screen_y=0;
                    }
                }
            }
        }
        s++;
    }
}

void lcd_drawPixel(unsigned short x, unsigned short y, unsigned short color) 
{
    // check boundary
    lcd_setAddr(x,y,x+1,y+1);
    lcd_data16(color);
}

void lcd_setAddr(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1) 
{
    lcd_command(LCD_CMD_CLMADRS); // Column
    lcd_data16(x0);
	lcd_data16(x1);

	lcd_command(LCD_CMD_PGEADRS); // Page
	lcd_data16(y0);
	lcd_data16(y1);

	lcd_command(LCD_CMD_RAMWR); //Into RAM
}

void lcd_clearScreen(unsigned short color) 
{
    int i;
    lcd_setAddr(0,0,LCD_GRAMWIDTH,LCD_GRAMHEIGH);
    for (i = 0;i < LCD_GRAMSIZE; i++){
        lcd_data16(color);
    }
    lcd_screencolor=color;
    int x , y;
    for (y = 0; y<16; y++)
        for (x = 0; x<25; x++)
            lcd_screen_chars[x][y]=' ';
    
    lcd_screen_x = 0;
    lcd_screen_y = 0;
}