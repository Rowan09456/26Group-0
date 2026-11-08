#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"

#include "ssd1306_i2c.h"
#include "bme280_spi.h"

int main()
{
    stdio_init_all();

    printf("Hello, SSD1306! ...\n");
    i2c_init(I2C_PORT, I2C_CLOCK * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
 

    SSD1306_initialise();
      if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    int i = 0;
    int32_t humidity, pressure, temperature;

    spi_init(mySPI, 500 * 1000);
    gpio_set_function(mySPI_RX, GPIO_FUNC_SPI);
    gpio_set_function(mySPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(mySPI_TX, GPIO_FUNC_SPI);

    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(mySPI_RX, mySPI_TX, mySPI_SCK, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(mySPI_CSN);
    gpio_set_dir(mySPI_CSN, GPIO_OUT);
    gpio_put(mySPI_CSN, 1);
    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(mySPI_CSN, "SPI CS"));

    // See if SPI is working - interrograte the device for its I2C ID number, should be 0x60
    uint8_t id;
    read_registers(0xD0, &id, 1);
    printf("Chip ID is 0x%x\n", id);
    sleep_ms(5000);

    read_compensation_parameters();

    write_register(0xF2, 0x1); // Humidity oversampling register - going for x1
    write_register(0xF4, 0x27);// Set rest of oversampling modes and run mode to normal
    

    while (1) {
        printf("Hell0, world [%d]!\n", i++);
        sleep_ms(1000);
         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN,!cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN) );

        ClearDisplay();

        DrawLine(0,0,SSD1306_LCDWIDTH-1, 0, true);
        DrawLine(0,0,0,SSD1306_LCDHEIGHT-1, true);
        DrawLine(SSD1306_LCDWIDTH-1, 0,SSD1306_LCDWIDTH-1, SSD1306_LCDHEIGHT-1, true);
        DrawLine(0,SSD1306_LCDHEIGHT-1,SSD1306_LCDWIDTH-1, SSD1306_LCDHEIGHT-1, true);

        WriteString(13,8,"HELLO SSD1306");

        for (int ch = '9'; ch>='1';ch--)
        {
            WriteBigChar(56, 36, ch);
            UpdateDisplay();
            sleep_ms(500);
        }

        WriteBigString(0,35,"BLASTOFF");
        UpdateDisplay();

        for (int i = 0;i<10;i++)
        {
            InvertDisplay(true);
            sleep_ms(100);
            InvertDisplay(false);
            sleep_ms(100);
        }

        for (int i=0;i<SSD1306_LCDWIDTH;i++) {
            if (i >= SSD1306_LCDWIDTH/2) {
                DrawLine(i,0,i, SSD1306_LCDHEIGHT-1, false);
                DrawLine(SSD1306_LCDWIDTH - i - 1,0,SSD1306_LCDWIDTH - i - 1, SSD1306_LCDHEIGHT-1, false);
            }
            else {
                DrawLine(i,0,i, SSD1306_LCDHEIGHT-1, true);
                DrawLine(SSD1306_LCDWIDTH - i - 1,0,SSD1306_LCDWIDTH - i - 1, SSD1306_LCDHEIGHT-1,  true);
            }
            UpdateDisplay();
        }
        bme280_read_raw(&humidity, &pressure, &temperature);
       // These are the raw numbers from the chip, so we need to run through the
        // compensations to get human understandable numbers
        pressure = compensate_pressure(pressure);
        temperature = compensate_temp(temperature);
        humidity = compensate_humidity(humidity);

        printf("Humidity = %.2f%%\n", humidity / 1024.0);
        printf("Pressure = %dPa\n", pressure);
        printf("Temp. = %.2fC\n", temperature / 100.0);

    }

    printf("Done\n");

    return 0;
}
