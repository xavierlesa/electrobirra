/*
 * config.h
 * configuración de los pines y constantes
 */


// PIN MAPPING
//
// ANALOGOS
//
// A0:  BUTTONS
// A1:  NC
// A2:  NC
// A3:  NC
// A4:  NC
// A5:  NC
// 
// DIGITAL
// 
// 0:   NC
// 1:   NC
// 2:   NC
// 3~:  NC (RELE_PUMP_B_PWM_PIN RELE_LT_PWM_PIN)
// 4:   LCD_D4
// 5~:  LCD_D5
// 6~:  LCD_D6
// 7:   LCD_D7
// 8:   LCD_RS
// 9~:  LCD_ENABLE
// 10~: LCD_BACKLIGHT
// 11~: RELE_PUMP_A_PWM_PIN RELE_HT_PWM_PIN 
// 12:  RELE_R1_PWM_PIN RELE_R2_PWM_PIN
// 13:  SENSOR_ONEWIRE_PIN
//
// Pines A0 y 4 a 9 reservados

// Menu, LCD setting col/rows
#define BUTTONS_PIN             A0      // Por defecto usa el ANALOG 0

#define LCD_D4                  4
#define LCD_D5                  5
#define LCD_D6                  6
#define LCD_D7                  7
#define LCD_RS                  8
#define LCD_ENABLE              9
#define LCD_BACKLIGHT           10

#define LCD_COLUMNS             16
#define LCD_ROWS                2

// Bus I2C
// Sensores de temp boil, mash, etc
#define SENSOR_ONEWIRE_PIN      13      // puede ser el 2

// RELES
// Reles para resistencias x 2
#define SINGLE_R_PWN            true    // indica si usa dos resistencias individuales o solo una (unica, o en paralelo)
#define RELE_R1_PWM_PIN         12      // rele para r1
#define RELE_R2_PWM_PIN         12      // rele para r2, solo es tomado en cuenta si SINGLE_R_PWN es false

#define RELE_HT_PWM_PIN         11      // rele para fermentador temp >= H
#define RELE_LT_PWM_PIN         3       // rele para fermentador temp <= L

// Rele para bomba A y B
#define SINGLE_PUMP_PWN         true    // indica si usa dos bombas individuales (A y B) o solo una (A)
// Bomba A: recirculado, [trasvase, enfriado]
#define RELE_PUMP_A_PWM_PIN     11      // A) rele para bomba recirculado, [whirlpool, trasvase y enfriado]
#define RELE_PUMP_B_PWM_PIN     3       // B) rele para bomba trasvase y enfriado

#define RELE_NA                 true    // el rele es normal abierto = true o normal cerrado = false?
