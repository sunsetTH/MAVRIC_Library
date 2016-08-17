/*******************************************************************************
 * Copyright (c) 2009-2016, MAV'RIC Development Team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/*******************************************************************************
 * \file    sparky_chibi.hpp
 *
 * \author  MAV'RIC Team
 * \author  Jean-Francois Burnier
 *
 * \brief   Autopilot for Sparky board based on STM32
 *
 ******************************************************************************/


#ifndef SPARKY_CHIBI_HPP_
#define SPARKY_CHIBI_HPP_


#include "drivers/barometer_ms5611.hpp"
#include "hal/common/led_gpio.hpp"
#include "drivers/state_display_sparky_v2.hpp"
#include "drivers/servo.hpp"

#include "hal/dummy/gpio_dummy.hpp"
#include "hal/dummy/pwm_dummy.hpp"
#include "hal/dummy/serial_dummy.hpp"

#include "hal/chibios/gpio_chibios.hpp"
#include "hal/chibios/i2c_chibios.hpp"

extern "C"
{
#include "util/streams.h"
}


// Preprocessor definitions

/*
 * Should the ESC be calibrated?
 * 0 for false (normal flight)
 * 1 for true (calibration)
 * !!!IMPORTANT!!!
 * IF CALIBRATING, TAKE OFF PROPS
 */
#define CALIBRATE_ESC 0


/**
 * \brief   Configuration structure
 */
// typedef struct
// {
    // gpio_stm32_conf_t        dsm_receiver_gpio_config;
    // gpio_stm32_conf_t        dsm_power_gpio_config;
    // gpio_stm32_conf_t        led_err_gpio_config;
    // gpio_stm32_conf_t        led_stat_gpio_config;
    // gpio_stm32_conf_t        led_rf_gpio_config;
    // Pwm_stm32::config_t      pwm_config[8];
    // Serial_usb_stm32::conf_t serial_usb_config;
    // servo_conf_t             servo_config[8];
    // spi_stm32_conf_t         spi_config[3];
    // i2c_stm32_conf_t         i2c_config[2];
    //
    // Barometer_MS5611::conf_t barometer_config;
// } sparky_v2_conf_t;


/**
 * \brief   Default configuration for the board
 *
 * \return  Config structure
 */
// static inline sparky_v2_conf_t sparky_v2_default_config();


/**
 * \brief  Boardsupport for the Sparky board
 *
 */
class Sparky_chibi
{
public:

    /**
     * \brief   Configuration structure
     */
    struct conf_t
    {
        Gpio_chibios::conf_t      gpio_led_err;
        I2c_chibios::conf_t       i2c1;
        Barometer_MS5611::conf_t  barometer;
    };


    /**
     * \brief           Constructor
     *
     * \param   config  Board configuration
     */
    Sparky_chibi(conf_t config = default_config());


    /**
     * \brief   Hardware initialisation

     * \return  Success
     */
    bool init(void);


    /**
     * \brief   Default configuration for the board
     *
     * \return  Config structure
     */
    static inline conf_t default_config();


    /**
     * Public Members
     */
    // Gpio_stm32              led_err_gpio_;
    // Gpio_stm32              led_stat_gpio_;
    // Gpio_stm32              led_rf_gpio_;
    // Led_gpio                led_err_;
    // Led_gpio                led_stat_;
    // Led_gpio                led_rf_;
    // Pwm_stm32               pwm_0_;
    // Pwm_stm32               pwm_1_;
    // Pwm_stm32               pwm_2_;
    // Pwm_stm32               pwm_3_;
    // Pwm_stm32               pwm_4_;
    // Pwm_stm32               pwm_5_;
    // Pwm_dummy               pwm_6_;
    // Pwm_dummy               pwm_7_;
    // Serial_usb_stm32        serial_;
    // Servo                   servo_0_;
    // Servo                   servo_1_;
    // Servo                   servo_2_;
    // Servo                   servo_3_;
    // Servo                   servo_4_;
    // Servo                   servo_5_;
    // Servo                   servo_6_;
    // Servo                   servo_7_;
    // Spi_stm32               spi_1_;
    // Spi_stm32               spi_3_;
    // I2c_stm32               i2c_1_;
    // I2c_stm32               i2c_2_;
    // State_display_sparky_v2 state_display_sparky_v2_;


    I2c_chibios             i2c1_;

    Led_gpio                led_err_;
    // Led_gpio                led_stat_;
    // Led_gpio                led_rf_;

    Barometer_MS5611        barometer_;

private:
    Gpio_chibios            gpio_led_err_;
    // Gpio_chibios            gpio_led_stat_;
    // Gpio_chibios            gpio_led_rf_;

    // byte_stream_t   dbg_stream_;  ///< Temporary member to make print_util work TODO: remove
};


/**
 * \brief   Default configuration for the board
 *
 * \return  Config structure
 */
Sparky_chibi::conf_t Sparky_chibi::default_config()
{
    conf_t conf = {};

    // -------------------------------------------------------------------------
    // GPIO config
    // -------------------------------------------------------------------------
    conf.gpio_led_err =
    {
        .port  = GPIOB,
        .pin   = GPIOB_PIN4
    };

    // -------------------------------------------------------------------------
    // I2C config
    // -------------------------------------------------------------------------
    conf.i2c1 =
    {
        .driver  = &I2CD1,
        .config  =
        {
            .op_mode     = OPMODE_I2C,
            .clock_speed = 400000,
            .duty_cycle  = FAST_DUTY_CYCLE_2
        },
        .timeout = 1000,
    };

    // -------------------------------------------------------------------------
    // Barometer config
    // -------------------------------------------------------------------------
    conf.barometer = Barometer_MS5611::default_config();

    // // -------------------------------------------------------------------------
    // // GPIO config
    // // -------------------------------------------------------------------------
    // // Error Led
    // conf.led_err_gpio_config.port    = GPIO_STM32_PORT_B;
    // conf.led_err_gpio_config.pin     = GPIO_STM32_PIN_4;
    // conf.led_err_gpio_config.dir     = GPIO_OUTPUT;
    // conf.led_err_gpio_config.pull    = GPIO_PULL_UPDOWN_NONE;
    //
    // // Status Led
    // conf.led_stat_gpio_config.port   = GPIO_STM32_PORT_B;
    // conf.led_stat_gpio_config.pin    = GPIO_STM32_PIN_5;
    // conf.led_stat_gpio_config.dir    = GPIO_OUTPUT;
    // conf.led_stat_gpio_config.pull   = GPIO_PULL_UPDOWN_NONE;
    //
    // // Rf Led
    // conf.led_rf_gpio_config.port     = GPIO_STM32_PORT_B;
    // conf.led_rf_gpio_config.pin      = GPIO_STM32_PIN_6;
    // conf.led_rf_gpio_config.dir      = GPIO_OUTPUT;
    // conf.led_rf_gpio_config.pull     = GPIO_PULL_UPDOWN_NONE;
    //
    //
    //
    // // -------------------------------------------------------------------------
    // // PWM config
    // // -------------------------------------------------------------------------
    // conf.pwm_config[0].gpio_config.port     = GPIO_STM32_PORT_B;
    // conf.pwm_config[0].gpio_config.pin      = GPIO_STM32_PIN_0;
    // conf.pwm_config[0].gpio_config.dir      = GPIO_OUTPUT;
    // conf.pwm_config[0].gpio_config.pull     = GPIO_PULL_UPDOWN_NONE;
    // conf.pwm_config[0].gpio_config.alt_fct  = GPIO_STM32_AF_2;
    // conf.pwm_config[0].timer                = TIM3;
    // conf.pwm_config[0].rcc_timer            = RCC_TIM3;
    // conf.pwm_config[0].channel              = Pwm_stm32::PWM_STM32_CHANNEL_3;
    // conf.pwm_config[0].prescaler            = 84; //since APB1 clock is main_clk/2
    // conf.pwm_config[0].period               = 20000; //50Hz
    // conf.pwm_config[0].pulse_us             = 5000;
    //
    // conf.pwm_config[1].gpio_config.port     = GPIO_STM32_PORT_B;
    // conf.pwm_config[1].gpio_config.pin      = GPIO_STM32_PIN_1;
    // conf.pwm_config[1].gpio_config.dir      = GPIO_OUTPUT;
    // conf.pwm_config[1].gpio_config.pull     = GPIO_PULL_UPDOWN_NONE;
    // conf.pwm_config[1].gpio_config.alt_fct  = GPIO_STM32_AF_2;
    // conf.pwm_config[1].timer                = TIM3;
    // conf.pwm_config[1].rcc_timer            = RCC_TIM3;
    // conf.pwm_config[1].channel              = Pwm_stm32::PWM_STM32_CHANNEL_4;
    // conf.pwm_config[1].prescaler            = 84;
    // conf.pwm_config[1].period               = 20000; //50Hz
    // conf.pwm_config[1].pulse_us             = 5000;
    //
    // conf.pwm_config[2].gpio_config.port     = GPIO_STM32_PORT_A;
    // conf.pwm_config[2].gpio_config.pin      = GPIO_STM32_PIN_3;
    // conf.pwm_config[2].gpio_config.dir      = GPIO_OUTPUT;
    // conf.pwm_config[2].gpio_config.pull     = GPIO_PULL_UPDOWN_NONE;
    // conf.pwm_config[2].gpio_config.alt_fct  = GPIO_STM32_AF_3;
    // conf.pwm_config[2].timer                = TIM9;
    // conf.pwm_config[2].rcc_timer            = RCC_TIM9;
    // conf.pwm_config[2].channel              = Pwm_stm32::PWM_STM32_CHANNEL_2;
    // // max timer clock freq of TIM9 is not 84 but 168 MHz
    // conf.pwm_config[2].prescaler            = 168;
    // conf.pwm_config[2].period               = 20000; //50Hz
    // conf.pwm_config[2].pulse_us             = 5000;
    //
    // conf.pwm_config[3].gpio_config.port     = GPIO_STM32_PORT_A;
    // conf.pwm_config[3].gpio_config.pin      = GPIO_STM32_PIN_2;
    // conf.pwm_config[3].gpio_config.dir      = GPIO_OUTPUT;
    // conf.pwm_config[3].gpio_config.pull     = GPIO_PULL_UPDOWN_NONE;
    // conf.pwm_config[3].gpio_config.alt_fct  = GPIO_STM32_AF_1;
    // conf.pwm_config[3].timer                = TIM2;
    // conf.pwm_config[3].rcc_timer            = RCC_TIM2;
    // conf.pwm_config[3].channel              = Pwm_stm32::PWM_STM32_CHANNEL_3;
    // conf.pwm_config[3].prescaler            = 84;
    // conf.pwm_config[3].period               = 20000; //50Hz
    // conf.pwm_config[3].pulse_us             = 5000;
    //
    // conf.pwm_config[4].gpio_config.port     = GPIO_STM32_PORT_A;
    // conf.pwm_config[4].gpio_config.pin      = GPIO_STM32_PIN_1;
    // conf.pwm_config[4].gpio_config.dir      = GPIO_OUTPUT;
    // conf.pwm_config[4].gpio_config.pull     = GPIO_PULL_UPDOWN_NONE;
    // conf.pwm_config[4].gpio_config.alt_fct  = GPIO_STM32_AF_2;
    // conf.pwm_config[4].timer                = TIM5;
    // conf.pwm_config[4].rcc_timer            = RCC_TIM5;
    // conf.pwm_config[4].channel              = Pwm_stm32::PWM_STM32_CHANNEL_2;
    // conf.pwm_config[4].prescaler            = 84;
    // conf.pwm_config[4].period               = 20000; //50Hz
    // conf.pwm_config[4].pulse_us             = 5000;
    //
    // conf.pwm_config[5].gpio_config.port     = GPIO_STM32_PORT_A;
    // conf.pwm_config[5].gpio_config.pin      = GPIO_STM32_PIN_0;
    // conf.pwm_config[5].gpio_config.dir      = GPIO_OUTPUT;
    // conf.pwm_config[5].gpio_config.pull     = GPIO_PULL_UPDOWN_NONE;
    // conf.pwm_config[5].gpio_config.alt_fct  = GPIO_STM32_AF_2;
    // conf.pwm_config[5].timer                = TIM5;
    // conf.pwm_config[5].rcc_timer            = RCC_TIM5;
    // conf.pwm_config[5].channel              = Pwm_stm32::PWM_STM32_CHANNEL_1;
    // conf.pwm_config[5].prescaler            = 84;
    // conf.pwm_config[5].period               = 20000; //50Hz
    // conf.pwm_config[5].pulse_us             = 5000;
    //
    // // -------------------------------------------------------------------------
    // // Servo config
    // // -------------------------------------------------------------------------
    // conf.servo_config[0] = servo_default_config_esc();
    // conf.servo_config[1] = servo_default_config_esc();
    // conf.servo_config[2] = servo_default_config_esc();
    // conf.servo_config[3] = servo_default_config_esc();
    // conf.servo_config[4] = servo_default_config_esc();
    // conf.servo_config[5] = servo_default_config_esc();
    // conf.servo_config[6] = servo_default_config_esc();
    // conf.servo_config[7] = servo_default_config_esc();
    //
    // // -------------------------------------------------------------------------
    // // USB config
    // // -------------------------------------------------------------------------
    // conf.serial_usb_config = Serial_usb_stm32::default_config();
    //
    // // -------------------------------------------------------------------------
    // // SPI config
    // // -------------------------------------------------------------------------
    // conf.spi_config[0].spi_device       = STM32_SPI1;
    // conf.spi_config[0].mode             = STM32_SPI_IN_OUT;
    // conf.spi_config[0].clk_div          = SPI_CR1_BAUDRATE_FPCLK_DIV_128;
    //
    // conf.spi_config[0].miso_gpio_config.port    = GPIO_STM32_PORT_A;
    // conf.spi_config[0].miso_gpio_config.pin     = GPIO_STM32_PIN_6;
    // conf.spi_config[0].miso_gpio_config.dir     = GPIO_INPUT;
    // conf.spi_config[0].miso_gpio_config.pull    = GPIO_PULL_UPDOWN_DOWN;
    // conf.spi_config[0].miso_gpio_config.alt_fct = GPIO_STM32_AF_5;
    //
    // conf.spi_config[0].mosi_gpio_config.port    = GPIO_STM32_PORT_A;
    // conf.spi_config[0].mosi_gpio_config.pin     = GPIO_STM32_PIN_7;
    // conf.spi_config[0].mosi_gpio_config.dir     = GPIO_OUTPUT;
    // conf.spi_config[0].mosi_gpio_config.pull    = GPIO_PULL_UPDOWN_DOWN;
    // conf.spi_config[0].mosi_gpio_config.alt_fct = GPIO_STM32_AF_5;
    //
    // conf.spi_config[0].nss_gpio_config.port     = GPIO_STM32_PORT_A;
    // conf.spi_config[0].nss_gpio_config.pin      = GPIO_STM32_PIN_4;
    // conf.spi_config[0].nss_gpio_config.dir      = GPIO_OUTPUT;
    // conf.spi_config[0].nss_gpio_config.pull     = GPIO_PULL_UPDOWN_NONE;
    // conf.spi_config[0].nss_gpio_config.alt_fct  = GPIO_STM32_AF_5;
    //
    // conf.spi_config[0].sck_gpio_config.port     = GPIO_STM32_PORT_A;
    // conf.spi_config[0].sck_gpio_config.pin      = GPIO_STM32_PIN_5;
    // conf.spi_config[0].sck_gpio_config.dir      = GPIO_OUTPUT;
    // conf.spi_config[0].sck_gpio_config.pull     = GPIO_PULL_UPDOWN_DOWN;
    // conf.spi_config[0].sck_gpio_config.alt_fct  = GPIO_STM32_AF_5;
    //
    //
    // conf.spi_config[2].spi_device               = STM32_SPI3;
    // conf.spi_config[2].mode                     = STM32_SPI_IN_OUT;
    // conf.spi_config[2].clk_div                  = SPI_CR1_BAUDRATE_FPCLK_DIV_64;
    //
    // conf.spi_config[2].miso_gpio_config.port    = GPIO_STM32_PORT_C;
    // conf.spi_config[2].miso_gpio_config.pin     = GPIO_STM32_PIN_11;
    // conf.spi_config[2].miso_gpio_config.dir     = GPIO_INPUT;
    // conf.spi_config[2].miso_gpio_config.pull    = GPIO_PULL_UPDOWN_DOWN;
    // conf.spi_config[2].miso_gpio_config.alt_fct = GPIO_STM32_AF_6;
    //
    // conf.spi_config[2].mosi_gpio_config.port    = GPIO_STM32_PORT_C;
    // conf.spi_config[2].mosi_gpio_config.pin     = GPIO_STM32_PIN_12;
    // conf.spi_config[2].mosi_gpio_config.dir     = GPIO_OUTPUT;
    // conf.spi_config[2].mosi_gpio_config.pull    = GPIO_PULL_UPDOWN_DOWN;
    // conf.spi_config[2].mosi_gpio_config.alt_fct = GPIO_STM32_AF_6;
    //
    // conf.spi_config[2].nss_gpio_config.port     = GPIO_STM32_PORT_A;
    // conf.spi_config[2].nss_gpio_config.pin      = GPIO_STM32_PIN_15;
    // conf.spi_config[2].nss_gpio_config.dir      = GPIO_OUTPUT;
    // conf.spi_config[2].nss_gpio_config.pull     = GPIO_PULL_UPDOWN_NONE;
    // conf.spi_config[2].nss_gpio_config.alt_fct  = GPIO_STM32_AF_6;
    //
    // conf.spi_config[2].sck_gpio_config.port     = GPIO_STM32_PORT_C;
    // conf.spi_config[2].sck_gpio_config.pin      = GPIO_STM32_PIN_10;
    // conf.spi_config[2].sck_gpio_config.dir      = GPIO_OUTPUT;
    // conf.spi_config[2].sck_gpio_config.pull     = GPIO_PULL_UPDOWN_DOWN;
    // conf.spi_config[2].sck_gpio_config.alt_fct  = GPIO_STM32_AF_6;
    //
    // conf.i2c_config[0]                        = i2c_stm32_default_config();
    // conf.i2c_config[0].i2c_device_config      = STM32_I2C1;
    // conf.i2c_config[0].rcc_i2c_config         = RCC_I2C1;
    // conf.i2c_config[0].rcc_sda_port_config    = RCC_GPIOB;
    // conf.i2c_config[0].sda_config.port        = GPIO_STM32_PORT_B;
    // conf.i2c_config[0].sda_config.pin         = GPIO_STM32_PIN_9;
    // conf.i2c_config[0].sda_config.alt_fct     = GPIO_STM32_AF_4;
    // conf.i2c_config[0].rcc_clk_port_config    = RCC_GPIOB;
    // conf.i2c_config[0].clk_config.port        = GPIO_STM32_PORT_B;
    // conf.i2c_config[0].clk_config.pin         = GPIO_STM32_PIN_8;
    // conf.i2c_config[0].clk_config.alt_fct     = GPIO_STM32_AF_4;
    // conf.i2c_config[0].clk_speed              = 100000;
    // conf.i2c_config[0].tenbit_config          = false;  // currently only support 8 bits addressing
    // conf.i2c_config[0].timeout                = 20000;
    //
    // conf.i2c_config[1]                        = i2c_stm32_default_config();
    // conf.i2c_config[1].i2c_device_config      = STM32_I2C2;
    // conf.i2c_config[1].rcc_i2c_config         = RCC_I2C2;
    // conf.i2c_config[1].rcc_sda_port_config    = RCC_GPIOB;
    // conf.i2c_config[1].sda_config.port        = GPIO_STM32_PORT_B;
    // conf.i2c_config[1].sda_config.pin         = GPIO_STM32_PIN_11;
    // conf.i2c_config[1].sda_config.alt_fct     = GPIO_STM32_AF_4;
    // conf.i2c_config[1].rcc_clk_port_config    = RCC_GPIOB;
    // conf.i2c_config[1].clk_config.port        = GPIO_STM32_PORT_B;
    // conf.i2c_config[1].clk_config.pin         = GPIO_STM32_PIN_10;
    // conf.i2c_config[1].clk_config.alt_fct     = GPIO_STM32_AF_4;
    // conf.i2c_config[1].clk_speed              = 100000;
    // conf.i2c_config[1].tenbit_config          = false;  // currently only support 8 bits addressing
    // conf.i2c_config[1].timeout                = 20000;

    return conf;
}


#endif /* SPARKY_CHIBI_HPP_ */
