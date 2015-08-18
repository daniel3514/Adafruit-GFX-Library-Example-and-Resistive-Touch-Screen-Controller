#include <Adafruit_GFX.h>    	// Core graphics library
#include <Adafruit_TFTLCD.h> 	// Hardware-specific library
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_STMPE610.h" 	// Adafruit STMPE610 Resistive touch screen controller breakout http://www.adafruit.com/products/1571

// LCD
	// The control pins for the LCD can be assigned to any digital or
	// analog pins...but we'll use the analog pins as this allows us to
	// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
	// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
	// For the Arduino Uno, Duemilanove, Diecimila, etc.:
	//   D0 connects to digital pin 8  (Notice these are
	//   D1 connects to digital pin 9   NOT in order!)
	//   D2 connects to digital pin 2
	//   D3 connects to digital pin 3
	//   D4 connects to digital pin 4
	//   D5 connects to digital pin 5
	//   D6 connects to digital pin 6
	//   D7 connects to digital pin 7
	// For the Arduino Mega, use digital pins 22 through 29
	// (on the 2-row header at the end of the board).
	// Assign human-readable names to some common 16-bit color values:
		// colores
#define	Black   	0x0000	// color 0
#define	Blue    	0x001F	// color 1
#define	Red     	0xF800	// color 2
#define	Green   	0x07E0	// color 3
#define Cyan   	 	0x07FF 	// color 4
#define Magenta 	0xF81F	// color 5
#define Yellow  	0xFFE0	// color 6
#define White   	0xFFFF	// color 7
#define Navy        0x000F	// color 8
#define DarkGreen   0x03E0	// color 9
#define DarkCyan    0x03EF	// color 10
#define Maroon      0x7800	// color 11
#define Purple      0x780F	// color 12
#define Olive       0x7BE0	// color 13
#define LightGrey   0xC618	// color 14
#define DarkGrey    0x7BEF	// color 15
#define Orange      0xFD20	// color 16
#define GreenYellow 0xAFE5 	// color 17
	// coneccion de pantalla
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// Touch Screen
	// Pick one of three wiring options below!
		// Option #1 - uses I2C, connect to hardware I2C port only!
		// SCL to I2C clock (#A5 on Uno) and SDA to I2C data (#A4 on Uno)
		// tie MODE to GND and POWER CYCLE (there is no reset pin)
Adafruit_STMPE610 touch = Adafruit_STMPE610();
		// Option #2 - use hardware SPI, connect to hardware SPI port only!
		// SDI to MOSI, SDO to MISO, and SCL to SPI CLOCK
		// on Arduino Uno, that's 11, 12 and 13 respectively
		// Then pick a CS pin, any pin is OK but we suggest #10 on an Uno
		// tie MODE to 3.3V and POWER CYCLE the STMPE610 (there is no reset pin)
	//Adafruit_STMPE610 touch = Adafruit_STMPE610(STMPE_CS);
		// Option #3 - use software SPI, connect to *any* 4 I/O pins!
		// define the following pins to whatever 4 you want and wire up!
		// Tie MODE to 3.3V and POWER CYCLE the STMPE610 (there is no reset pin)
	// Adafruit_STMPE610 touch = Adafruit_STMPE610(STMPE_CS, STMPE_SDI, STMPE_SDO, STMPE_SCK);
// DMX
int	DMX_rt_Values 	[512];  // array de valores actuales DMX
int	DMX_rt_Colors	[512];  // array de valores de colores actuales DMX	
// GUI default 
	// reticula
int color_reticula = DarkGrey;

void setup() 
{
	// Debbugger
	Serial.begin(9600);
	// TFT inicializador de pantalla
	tft.reset();
	tft.begin(tft.readID());	// readID, el valor ID que lee la libreria desde la pantalla
	// Touch Screen
		// if using hardware SPI on an Uno #10 must be an output, remove line
			// if using software SPI or I2C
			// pinMode(10, OUTPUT);
		// If using I2C you can select the I2C address (there are two options) by calling
			// touch.begin(0x41), the default, or touch.begin(0x44) if A0 is tied to 3.3V
			// If no address is passed, 0x41 is used
}

void loop() 
{
	Debbugger_Init();	// inicializador de debbugger
	GUI();
	Matrix(1);
	
	for (byte numero = 1; numero <= 48; numero ++)
	{
		//DMX_rt_Colors[numero] = numero;
		Matrix_Element(numero, numero, 0, White);	// Matrix_Element(int canal, byte element, byte marco, int color_marco)
	}
	
	GUI_Channel_Options(1);
	
	while(1)
	{
		GUI_Touch_Event();
	}
}

void Debbugger_Init()
{
	Serial.println("Arduino DMX-512 Tester and Controller");
	Serial.println("Open Hardware");
	Serial.println("Hardware v0.0");
	Serial.println("Software v0.0");
	Serial.println(" ");
	// Touch Screen
		// saber si esta presente el hardware
	if (! touch.begin()) 
	{
		Serial.println("Fail - Touch Screen Controller");
	}
	else
	{
		Serial.println("  Ok - Touch Screen Controller");
	}
	// TFT screen
	if (tft.readID() == 33623)	// id de pantalla, lo usamos como verificador de funcionamiento
	{
		Serial.println("  Ok - TFT Screen");
	}
	else
	{
		Serial.println("Fail - TFT Screen");
	}

}

void GUI_Touch_Event()
{
	uint16_t 	x;
	uint16_t 	y;
	uint8_t 	z;
	
	if (touch.touched()) 
	{
		if (touch.bufferEmpty() == 0) // esta vacio el buffer?
		{
			// X - 0­4095
			// Y - 0­4095
			// Z - presure, 0-255
			Serial.print(touch.bufferSize());
			touch.readData(&x, &y, &z);
			Serial.print("->(");
			Serial.print(x); Serial.print(", ");
			Serial.print(y); Serial.print(", ");
			Serial.print(z);
			Serial.println(")");
			// ajuste a resolucion
			
		}
		touch.writeRegister8(STMPE_INT_STA, 0xFF); // reset all ints
	}
}

void GUI_Channel_Options(int canal)
{
	// Titulo
	tft.setTextColor(White);		// color de texto
	tft.setTextSize(1);				// tamaño de texto, lo probamos 1 hasta 100 y funciona
	tft.setCursor(387, 5);	// posicion del cursor
	tft.println("Channel Options");	
	tft.drawRect(383, 0, 97, 17, color_reticula);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);	
	// boton exit
	GUI_boton(385, 268, 1, 1);	// GUI_boton(int x, int y, byte texto, byte press)
	// boton Locate Channel
	GUI_boton(385, 216, 2, 1);	// GUI_boton(int x, int y, byte texto, byte press)
}

void GUI_boton(int x, int y, byte texto, byte press)
{
	// dibuja boton y texto, y el invertido cuando se preciona con retardo para mostrar
	// siempre tienen fondo negro sin presionar
	// textos:
	// 1 - Exit
	// 2 - Locate Channel
	int  retardo_press 	= 200;
	byte ancho 			= 94;
	byte alto  			= 52;
	byte radio 			= 15;
	int  color;
	tft.setTextSize(1);			// tamaño de texto, lo probamos 1 hasta 100 y funciona
		// dibujar boton
press_on:
	if (press == 0)
	{
		// contorno
		switch (texto)
		{
			case 1:	// Exit
				color = Red;
				break;
			case 2: // Locate Channel
				color = Blue;
				break;
		}
		tft.drawRoundRect(x, y, ancho, alto, radio, color);	// rectangulo redondeado, drawRoundRect(X, Y, ancho, alto, radio, color)
		tft.drawRoundRect(x + 1, y + 1, ancho - 2, alto - 2, radio, color);	// rectangulo redondeado, drawRoundRect(X, Y, ancho, alto, radio, color)
		tft.drawRoundRect(x + 2, y + 2, ancho - 4, alto - 4, radio, color);	// rectangulo redondeado, drawRoundRect(X, Y, ancho, alto, radio, color)
	}
	else
	{
		// relleno
		switch (texto)
		{
			case 1:	// Exit
				color = Red;
				break;
			case 2: // Blue
				color = Blue;
				break;
		}
		tft.fillRoundRect(x, y, 94, 52, 15, color);			// rectangulo redondeado, drawRoundRect(X, Y, ancho, alto, radio, color)
	}	
	// texto
	switch (texto)
	{
		case 1:
			tft.setTextColor(White);			// color de texto
			tft.setCursor(x + 36, y + 22);		// posicion del cursor
			tft.println("Exit");				// texto
			break;
		case 2:
			tft.setTextColor(White);			// color de texto
			tft.setCursor(x + 30, y + 16);		// posicion del cursor
			tft.println("Locate");				// texto
			tft.setCursor(x + 27, y + 28);		// posicion del cursor
			tft.println("Channel");				// texto
			break;
	}
	// repintar boton
	delay (retardo_press);
	if (press == 1)
	{
		tft.fillRoundRect(x, y, 94, 52, 15, Black);	// rectangulo redondeado, drawRoundRect(X, Y, ancho, alto, radio, color)
		press = 0;
		goto press_on;
	}
}

void GUI_icon_key(int x, int y, int color)
{
	// x, y corresponden a la esquina superior izq
	// contorno
	tft.drawRoundRect(x, y, 15, 15, 3, color);	// rectangulo redondeado, drawRoundRect(X, Y, ancho, alto, radio, color)
	// texto
	tft.setTextColor(color);		// color de texto
	tft.setTextSize(1);				// tamaño de texto, lo probamos 1 hasta 100 y funciona
	tft.setCursor(x + 5, y + 4);	// posicion del cursor
	tft.println("A");				// texto
}

void GUI_icon_encoder(int x, int y, int color)
{
	// x, y corresponden a la esquina superior izq
	// contorno
	tft.drawCircle(x + 7, y + 7, 7, color);	// dibuja el contorno de un circulo, drawCircle(X, Y, radio, color)
	// texto
	tft.setTextColor(color);		// color de texto
	tft.setTextSize(1);				// tamaño de texto, lo probamos 1 hasta 100 y funciona
	tft.setCursor(x + 5, y + 4);			// posicion del cursor
	tft.println("E");				// texto
}

void GUI()
{
	tft.setRotation(3);								// rotacion de pantalla 0 = 0*, 1 = 90*, 2 = 180*, 3 = 270*
	tft.fillScreen(Black);							// fondo negro
	// marco
	tft.drawRect(0, 0, 384, 320, color_reticula);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);	
}

void Menu()
{
	// separadores botones
	int separador = 53;
	tft.drawLine(385, separador, 480, separador, color_reticula);			// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(385, separador * 2, 480, separador * 2, color_reticula);	// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(385, separador * 3, 480, separador * 3, color_reticula);	// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(385, separador * 4, 480, separador * 4, color_reticula);	// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(385, separador * 5, 480, separador * 5, color_reticula);	// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	// opciones 
	tft.setTextColor(White);	// color de texto
	tft.setTextSize(1);			// tamaño de texto, lo probamos 1 hasta 100 y funciona
	// memory
	tft.setCursor(413, 25);		// posicion del cursor
	tft.println("Memory");		// texto
	// chaser
	tft.setCursor(415, 76);		// posicion del cursor
	tft.println("Chaser");		// texto
	// Secuencer
	tft.setCursor(407, 129);	// posicion del cursor
	tft.println("Secuencer");	// texto
	// Multiply
	tft.setCursor(410, 182);	// posicion del cursor
	tft.println("Multiply");	// texto
	// Info
	tft.setCursor(420, 235);	// posicion del cursor
	tft.println("Info");		// texto
	// Config
	tft.setCursor(414, 290);	// posicion del cursor
	tft.println("Config");		// texto
}
	
void Matrix_Element(int canal, byte element, byte marco, int color_marco)
{
	// llena un elemento de la matriz
	// tamaño de texto 2
	// fondo de canal y de texto
	int color;
	switch (DMX_rt_Colors[canal])
	{
		case 0:
			color = Black;				// fondo de canal
			tft.setTextColor(White);	// color de texto
			break;
		case 1:
			color = Blue;				// fondo de canal
			tft.setTextColor(White);	// color de texto
			break;
		case 2:
			color = Red;				// fondo de canal
			tft.setTextColor(White);	// color de texto
			break;
		case 3:
			color = Green;				// fondo de canal
			tft.setTextColor(Black);	// color de texto
			break;
		case 4:
			color = Cyan;				// fondo de canal
			tft.setTextColor(Black);	// color de texto
			break;
		case 5:
			color = Magenta;			// fondo de canal
			tft.setTextColor(White);	// color de texto
			break;
		case 6:
			color = Yellow;				// fondo de canal
			tft.setTextColor(Black);	// color de texto
			break;
		case 7:	
			color = White;				// fondo de canal
			tft.setTextColor(Black);	// color de texto
			break;
		case 8:
			color = Navy;				// fondo de canal
			tft.setTextColor(White);	// color de texto
			break;
		case 9:
			color = DarkGreen;			// fondo de canal
			tft.setTextColor(Black);	// color de texto
			break;
		case 10:
			color = DarkCyan;			// fondo de canal
			tft.setTextColor(Black);	// color de texto
			break;
		case 11:
			color = Maroon;				// fondo de canal
			tft.setTextColor(White);	// color de texto
			break;
		case 12:
			color = Purple;				// fondo de canal
			tft.setTextColor(White);	// color de texto
			break;
		case 13:
			color = Olive;				// fondo de canal
			tft.setTextColor(Black);	// color de texto
			break;
		case 14:
			color = LightGrey;			// fondo de canal
			tft.setTextColor(Black);	// color de texto
			break;
		case 15:
			color = DarkGrey;			// fondo de canal
			tft.setTextColor(Black);	// color de texto
			break;
		case 16:
			color = Orange;				// fondo de canal
			tft.setTextColor(Black);	// color de texto
			break;
		case 17:
			color = GreenYellow;		// fondo de canal
			tft.setTextColor(Black);	// color de texto
			break;
	}
	// Texto de matriz
	tft.setTextSize(2);					// tamaño de texto, lo probamos 1 hasta 100 y funciona
		// posicion
	int pos_x = 7;						// inicio en x
	int pos_y_init = 9;					// inicio en y
	int pos_y_sep_canal_val = 22;		// separacion en tre canal y valor
	int pos_y_sep_canval_canval = 31;	// separacion entre canal valor y canal valor
	int color_reticula = DarkGrey;
		// dibujar relleno marco y texto
	switch (element)	// elementos de la matriz
	{
		case 1:
			// fondo
			tft.fillRect(1, 1, 46, 52, color);				// rectangulo,  tft.drawRect(X, Y, alto, ancho, color);	
			// canal
			Text_2_Num_000(pos_x, pos_y_init, canal);
			// value
			Text_2_Num_000(pos_x, pos_y_init + pos_y_sep_canal_val, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(1, 1, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(2, 2, 44, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}
			break;
		case 2:
			// fondo
			tft.fillRect(48, 1, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 48, pos_y_init, canal);
			// value
			Text_2_Num_000(pos_x + 48, pos_y_init + pos_y_sep_canal_val, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(48, 1, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(49, 2, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}
			break;
		case 3:
			// fondo
			tft.fillRect(96, 1, 47, 52, color);	// rectangulo,  tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 96, pos_y_init, canal);
			// value
			Text_2_Num_000(pos_x + 96, pos_y_init + pos_y_sep_canal_val, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(96, 1, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(97, 2, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 4:
			// fondo
			tft.fillRect(144, 1, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 144, pos_y_init, canal);
			// value
			Text_2_Num_000(pos_x + 144, pos_y_init + pos_y_sep_canal_val, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(144, 1, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(145, 2, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 5:
			// fondo
			tft.fillRect(192, 1, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 192, pos_y_init, canal);
			// value
			Text_2_Num_000(pos_x + 192, pos_y_init + pos_y_sep_canal_val, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(192, 1, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(193, 2, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 6:
			// fondo
			tft.fillRect(240, 1, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 240, pos_y_init, canal);
			// value
			Text_2_Num_000(pos_x + 240, pos_y_init + pos_y_sep_canal_val, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(240, 1, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(241, 2, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 7:
			// fondo
			tft.fillRect(288, 1, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 288, pos_y_init, canal);
			// value
			Text_2_Num_000(pos_x + 288, pos_y_init + pos_y_sep_canal_val, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(288, 1, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(289, 2, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 8:
			// fondo
			tft.fillRect(336, 1, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 336, pos_y_init, canal);
			// value
			Text_2_Num_000(pos_x + 336, pos_y_init + pos_y_sep_canal_val, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(336, 1, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(337, 2, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 9:
			// fondo
			tft.fillRect(1, 54, 46, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);	
			// canal
			Text_2_Num_000(pos_x, pos_y_init + pos_y_sep_canal_val + pos_y_sep_canval_canval, canal);
			// value
			Text_2_Num_000(pos_x, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(1, 1, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(2, 2, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 10:
			// fondo
			tft.fillRect(48, 54, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 48, pos_y_init + pos_y_sep_canal_val + pos_y_sep_canval_canval, canal);
			// value
			Text_2_Num_000(pos_x + 48, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(48, 54, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(49, 55, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 11:
			// fondo
			tft.fillRect(96, 54, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 96, pos_y_init + pos_y_sep_canal_val + pos_y_sep_canval_canval, canal);
			// value
			Text_2_Num_000(pos_x + 96, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(96, 54, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(97, 55, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 12:
			// fondo
			tft.fillRect(144, 54, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 144, pos_y_init + pos_y_sep_canal_val + pos_y_sep_canval_canval, canal);
			// value
			Text_2_Num_000(pos_x + 144, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(144, 54, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(145, 55, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 13:
			// fondo
			tft.fillRect(192, 54, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 192, pos_y_init + pos_y_sep_canal_val + pos_y_sep_canval_canval, canal);
			// value
			Text_2_Num_000(pos_x + 192, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(192, 54, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(193, 55, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 14:
			// fondo
			tft.fillRect(240, 54, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 240, pos_y_init + pos_y_sep_canal_val + pos_y_sep_canval_canval, canal);
			// value
			Text_2_Num_000(pos_x + 240, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(240, 54, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(241, 55, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 15:
			// fondo
			tft.fillRect(288, 54, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 288, pos_y_init + pos_y_sep_canal_val + pos_y_sep_canval_canval, canal);
			// value
			Text_2_Num_000(pos_x + 288, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(288, 54, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(289, 55, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 16:
			// fondo
			tft.fillRect(336, 54, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 336, pos_y_init + pos_y_sep_canal_val + pos_y_sep_canval_canval, canal);
			// value
			Text_2_Num_000(pos_x + 336, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(336, 54, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(337, 55, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 17:
			// fondo
			tft.fillRect(1, 107, 46, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);	
			// canal
			Text_2_Num_000(pos_x, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval * 2, canal);
			// value
			Text_2_Num_000(pos_x, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 2, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(1, 107, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(2, 108, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 18:
			// fondo
			tft.fillRect(48, 107, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 48, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval * 2, canal);
			// value
			Text_2_Num_000(pos_x + 48, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 2, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(48, 107, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(49, 108, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 19:
			// fondo
			tft.fillRect(96, 107, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 96, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval * 2, canal);
			// value
			Text_2_Num_000(pos_x + 96, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 2, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(96, 107, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(97, 108, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 20:
			// fondo
			tft.fillRect(144, 107, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 144, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval * 2, canal);
			// value
			Text_2_Num_000(pos_x + 144, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 2, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(144, 107, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(145, 108, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 21:
			// fondo
			tft.fillRect(192, 107, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 192, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval * 2, canal);
			// value
			Text_2_Num_000(pos_x + 192, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 2, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(192, 107, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(193, 108, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 22:
			// fondo
			tft.fillRect(240, 107, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 240, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval * 2, canal);
			// value
			Text_2_Num_000(pos_x + 240, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 2, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(240, 107, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(241, 108, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 23:
			// fondo
			tft.fillRect(288, 107, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 288, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval * 2, canal);
			// value
			Text_2_Num_000(pos_x + 288, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 2, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(288, 107, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(289, 108, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 24:
			// fondo
			tft.fillRect(336, 107, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 336, pos_y_init + pos_y_sep_canal_val * 2 + pos_y_sep_canval_canval * 2, canal);
			// value
			Text_2_Num_000(pos_x + 336, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 2, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(336, 107, 47, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(337, 108, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 25:
			// fondo
			tft.fillRect(1, 160, 46, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);	
			// canal
			Text_2_Num_000(pos_x, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 3, canal);
			// value
			Text_2_Num_000(pos_x, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 3, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(1, 160, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(2, 161, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}
			break;
		case 26:
			// fondo
			tft.fillRect(48, 160, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 48, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 3, canal);
			// value
			Text_2_Num_000(pos_x + 48, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 3, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(48, 160, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(49, 161, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 27:
			// fondo
			tft.fillRect(96, 160, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 96, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 3, canal);
			// value
			Text_2_Num_000(pos_x + 96, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 3, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(96, 160, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(97, 161, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 28:
			// fondo
			tft.fillRect(144, 160, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 144, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 3, canal);
			// value
			Text_2_Num_000(pos_x + 144, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 3, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(144, 160, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(145, 161, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 29:
			// fondo
			tft.fillRect(192, 160, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 192, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 3, canal);
			// value
			Text_2_Num_000(pos_x + 192, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 3, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(192, 160, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(193, 161, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 30:
			// fondo
			tft.fillRect(240, 160, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 240, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 3, canal);
			// value
			Text_2_Num_000(pos_x + 240, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 3, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(240, 160, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(241, 161, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 31:
			// fondo
			tft.fillRect(288, 160, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 288, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 3, canal);
			// value
			Text_2_Num_000(pos_x + 288, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 3, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(288, 160, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(289, 161, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 32:
			// fondo
			tft.fillRect(336, 160, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 336, pos_y_init + pos_y_sep_canal_val * 3 + pos_y_sep_canval_canval * 3, canal);
			// value
			Text_2_Num_000(pos_x + 336, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 3, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(336, 160, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(337, 161, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 33:
			// fondo
			tft.fillRect(1, 213, 46, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);	
			// canal
			Text_2_Num_000(pos_x, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 4, canal);
			// value
			Text_2_Num_000(pos_x, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 4, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(1, 213, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(2, 214, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 34:
			// fondo
			tft.fillRect(48, 213, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 48, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 4, canal);
			// value
			Text_2_Num_000(pos_x + 48, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 4, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(48, 213, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(49, 214, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 35:
			// fondo
			tft.fillRect(96, 213, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 96, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 4, canal);
			// value
			Text_2_Num_000(pos_x + 96, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 4, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(96, 213, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(97, 214, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 36:
			// fondo
			tft.fillRect(144, 213, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 144, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 4, canal);
			// value
			Text_2_Num_000(pos_x + 144, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 4, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(144, 213, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(145, 214, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 37:
			// fondo
			tft.fillRect(192, 213, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 192, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 4, canal);
			// value
			Text_2_Num_000(pos_x + 192, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 4, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(192, 213, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(193, 214, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 38:
			// fondo
			tft.fillRect(240, 213, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 240, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 4, canal);
			// value
			Text_2_Num_000(pos_x + 240, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 4, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(240, 213, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(241, 214, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 39:
			// fondo
			tft.fillRect(288, 213, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 288, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 4, canal);
			// value
			Text_2_Num_000(pos_x + 288, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 4, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(288, 213, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(289, 214, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 40:
			// fondo
			tft.fillRect(336, 213, 47, 52, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 336, pos_y_init + pos_y_sep_canal_val * 4 + pos_y_sep_canval_canval * 4, canal);
			// value
			Text_2_Num_000(pos_x + 336, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 4, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(336, 213, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(337, 214, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 41:
			// fondo
			tft.fillRect(1, 266, 46, 53, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 5, canal);
			// value
			Text_2_Num_000(pos_x, pos_y_init + pos_y_sep_canal_val * 6 + pos_y_sep_canval_canval * 5, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(1, 266, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(2, 267, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 42:
			// fondo
			tft.fillRect(48, 266, 47, 53, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 48, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 5, canal);
			// value
			Text_2_Num_000(pos_x + 48, pos_y_init + pos_y_sep_canal_val * 6 + pos_y_sep_canval_canval * 5, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(48, 266, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(49, 267, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 43:
			// fondo
			tft.fillRect(96, 266, 47, 53, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 96, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 5, canal);
			// value
			Text_2_Num_000(pos_x + 96, pos_y_init + pos_y_sep_canal_val * 6 + pos_y_sep_canval_canval * 5, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(96, 266, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(97, 267, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 44:
			// fondo
			tft.fillRect(144, 266, 47, 53, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 144, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 5, canal);
			// value
			Text_2_Num_000(pos_x + 144, pos_y_init + pos_y_sep_canal_val * 6 + pos_y_sep_canval_canval * 5, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(144, 266, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(145, 267, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		case 45:
			// fondo
			tft.fillRect(192, 266, 47, 53, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 192, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 5, canal);
			// value
			Text_2_Num_000(pos_x + 192, pos_y_init + pos_y_sep_canal_val * 6 + pos_y_sep_canval_canval * 5, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(192, 266, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(193, 267, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 46:
			// fondo
			tft.fillRect(240, 266, 47, 53, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 240, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 5, canal);
			// value
			Text_2_Num_000(pos_x + 240, pos_y_init + pos_y_sep_canal_val * 6 + pos_y_sep_canval_canval * 5, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(240, 266, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(241, 267, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 47:
			// fondo
			tft.fillRect(288, 266, 47, 53, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 288, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 5, canal);
			// value
			Text_2_Num_000(pos_x + 288, pos_y_init + pos_y_sep_canal_val * 6 + pos_y_sep_canval_canval * 5, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(288, 266, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(289, 267, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}		
			break;
		case 48:
			// fondo
			tft.fillRect(336, 266, 47, 53, color);	// rectangulo, 	tft.drawRect(X, Y, alto, ancho, color);
			// canal
			Text_2_Num_000(pos_x + 336, pos_y_init + pos_y_sep_canal_val * 5 + pos_y_sep_canval_canval * 5, canal);
			// value
			Text_2_Num_000(pos_x + 336, pos_y_init + pos_y_sep_canal_val * 6 + pos_y_sep_canval_canval * 5, DMX_rt_Values[canal]);
			// marco
			if (marco == 1)	// esta avilitado el marco?
			{
				tft.drawRect(336, 266, 46, 52, color_marco);	// mismas coordenadas de relleno
				tft.drawRect(337, 267, 45, 50, color_marco);	// mismas coordenadas de relleno + 1 + 1 - 2 - 2
			}	
			break;
		}
}
	
void Text_2_Num_000(int x, int y, int num)
{
	if (num < 10)
	{
		tft.setCursor(x, y);		// posicion del cursor
		tft.println("00");	
		tft.setCursor(x + 24, y);	// posicion del cursor
		tft.println(num);	
	}
	if (num > 9 & num < 100)
	{
		tft.setCursor(x, y);		// posicion del cursor
		tft.println("0");	
		tft.setCursor(x + 12, y);	// posicion del cursor
		tft.println(num);	
	}
	if (num > 99)
	{
		tft.setCursor(x, y);		// posicion del cursor
		tft.println(num);		
	}
	if (num == 0)
	{
		tft.setCursor(x, y);		// posicion del cursor
		tft.println("000");
	}
}
	
void Matrix(int canal_inicial)
{
	// reticula
		// horizontales	cada 53px
	tft.drawLine(0, 53,  383, 53,  color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(0, 106, 383, 106, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(0, 159, 383, 159, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(0, 212, 383, 212, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(0, 265, 383, 265, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
		// verticales cada 48px
	tft.drawLine(47,  0,  47, 320, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(95,  0,  95, 320, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(143, 0, 143, 320, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(191, 0, 191, 320, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(239, 0, 239, 320, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(287, 0, 287, 320, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(335, 0, 335, 320, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
	tft.drawLine(383, 0, 383, 320, color_reticula);		// dibuja linea, tft.drawLine(X0, Y0, X1, Y1, color);
}