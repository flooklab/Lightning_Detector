/*
////////////////////////////////////////////////////////////////////////////////////
//
//  This file is part of the firmware of Lightning Detector, an experimental open
//  hardware project for early notification about approaching thunderstorm activity.
//  Copyright (C) 2024 M. Frohne
//
//  Lightning Detector's firmware is free software: you can redistribute it
//  and/or modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation, either version 3
//  of the License, or (at your option) any later version.
//
//  Lightning Detector's firmware is distributed in the hope that it
//  will be useful, but WITHOUT ANY WARRANTY; without even the implied
//  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Lightning Detector. If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////////
*/

#include "as3935.h"
#include "auxil.h"
#include "buzzer.h"
#include "configuration.h"
#include "display.h"
#include "muxeddipswitch.h"
#include "pins.h"
#include "pushbutton.h"
#include "timercallback.h"
#include "vddmeasurement.h"

#include <USB/PluggableUSBSerial.h>

#include <nrf_nvic.h>
#include <nrf_soc.h>

#include <array>

//Function  declarations

void setupPowerSave();

void readConfiguration();

void tuneAntenna();

void detectLightnings();
bool processInterruptAS3935();

void isrAS3935();
void isrButtonClr();
void isrButtonDsp();
void isrWakeTimer();

//int main();
//void setup();
//void loop();

//Constants

constexpr size_t pushButtonBounceTime = 20;     //Debounce time for used push buttons in milliseconds

constexpr float buzzerFreq = 1000;              //Buzzer beep frequency in Hz
constexpr float buzzerDutyCycle = 0.1;          //Buzzer signal duty cycle

constexpr float buzzerLightBeepSecs = 0.2;      //Buzzer beep duration for lightning notification in seconds

constexpr size_t wakeTimerIntervalSecs = 120;   //Seconds of inactivity before forced wake-up via RTC (prevent missed AS3935 IRQ, etc.)

constexpr size_t vddMeasIntervalMins = 30;      //Scheduled interval between VDD measurements in minutes
constexpr size_t vddMeasSmallIntervalMins = 10; //Smaller VDD measurement interval in minutes for high lightning activity
constexpr float vddMeasLightRateThr = 10.;      //Number of lightnings per minute to switch to smaller measurement interval

constexpr size_t lightRateAvrgMins = 5;     //Minimum ('minimum' due to sleep) averaging time to determine current lightning activity/rate

constexpr float as3935MinVoltage = 2.4;     //Minimum allowed operating voltage for the AS3935 sensor chip in Volt
constexpr float systemMinVoltage = 2.4;     //Largest minimum allowed operating voltage for any system component in Volt
constexpr float systemIdleCurrent = 0.0041; //System current during VDD measurement (while awake and idle otherwise) in Ampere
constexpr float systemMaxCurrent = 0.0081;  //Maximum system current (e.g. during beep or display update) in Ampere
constexpr float batteryOCVoltage0 = 3.0;    //Open-circuit voltage of a nearly fresh coin cell (i.e. ~ nominal voltage) in Volt
constexpr float batteryIntResEOL = 55.0;    //Internal resistance of single coin cell around end of life (~systemMinVoltage) in Ohm
constexpr uint8_t systemBatteryCount = 2;   //Number of installed coin cells

//Minimum allowed voltage for VDD measurement from idle running system
constexpr float lowBatteryThrVoltage = systemMinVoltage + (batteryIntResEOL / systemBatteryCount) * (systemMaxCurrent - systemIdleCurrent);

//Interrupt flags

volatile bool irqWake = false;
volatile bool irqWakeAS3935 = false;
volatile bool irqWakeClr = false;
volatile bool irqWakeDsp = false;
volatile bool irqWakeTimer = false;

//External symbols

extern "C" { extern bool _us_ticker_initialized; }
const bool& timer1Used = _us_ticker_initialized;    //Determines if TIMER1 is used by mbed (see <mbed_wait_api.h>)

//Devices

Buzzer buzzer(Pins::Buzzer, buzzerFreq, buzzerDutyCycle);

PushButton buttonCLR(Pins::PushButtonClr, true, pushButtonBounceTime);
PushButton buttonDSP(Pins::PushButtonDsp, true, pushButtonBounceTime);

MuxedDIPSwitch<5, 5> dip({Pins::DIPSel1, Pins::DIPSel2, Pins::DIPSel3, Pins::DIPSel4, Pins::DIPSel5},
                         {Pins::DIPSens1, Pins::DIPSens2, Pins::DIPSens3, Pins::DIPSens4, Pins::DIPSens5});

AS3935 lDet(Pins::SPI_CS, Pins::IRQ);

#if USE_DISPLAY == USE_DISPLAY_WAVESHAREEPAPER154BW_SX1509IOEXPANDER
    constexpr uint8_t sx1509Addr = 0x3E;
    Display display(sx1509Addr, SX1509Pins::Display_CS, SX1509Pins::Display_DC, SX1509Pins::Display_RST, SX1509Pins::Display_BUSY);
#else
    Display display;
#endif

//"Virtual devices" (using internal periphery)

TimerCallback wakeTimer(8*wakeTimerIntervalSecs, &isrWakeTimer);    //Periodic wake-up timer (binds RTC interrupt)

//Other globals

Configuration config;

using Auxil::RunMode;
RunMode runMode = RunMode::Normal;

bool serialEnabled = false;

AS3935::InterruptType lDetLastInterrupt = AS3935::InterruptType::Lightning; //Last reported interrupt type from AS3935
uint32_t lDetLastEnergy = 0;                                                //Last reported lightning energy from AS3935 (raw value)
uint8_t lDetStormDist = AS3935::stormDistanceOutOfRange;                    //Last reported thunderstorm distance from AS3935 in km

//Function definitions

/*!
 * \brief Configure some power save settings.
 */
void setupPowerSave()
{
    //Turn off power LED
    digitalWrite(Pins::LED_Power, LOW);

    //Disable IMU
    digitalWrite(PIN_ENABLE_SENSORS_3V3, LOW);
    digitalWrite(PIN_ENABLE_I2C_PULLUP, LOW);

    //Configure pull-down for unused RX/TX pins
    pinMode(0, INPUT_PULLDOWN);
    pinMode(1, INPUT_PULLDOWN);

    //Disable UART

    NRF_UARTE0->TASKS_STOPTX = 1;
    NRF_UARTE0->TASKS_STOPRX = 1;
    NRF_UARTE0->ENABLE = 0;

    NRF_UARTE1->TASKS_STOPTX = 1;
    NRF_UARTE1->TASKS_STOPRX = 1;
    NRF_UARTE1->ENABLE = 0;

    NRF_UART0->TASKS_STOPTX = 1;
    NRF_UART0->TASKS_STOPRX = 1;
    NRF_UART0->ENABLE = 0;

    //Disable unused I2C peripheral(s)
    if (!useDisplay)
        NRF_TWI0->ENABLE = 0;
    NRF_TWI1->ENABLE = 0;

    //Disable two unused SPI peripherals
    NRF_SPI0->ENABLE = 0;
    NRF_SPI1->ENABLE = 0;

    //Ensure system is in low power mode
    NRF_POWER->TASKS_LOWPWR = 1;
}

/*!
 * \brief Read the DIP switch configuration bits for setting the AS3935 registers.
 */
void readConfiguration()
{
    std::array<bool, 25> conf25 = dip.readAllBits();
    std::array<bool, 24> conf;

    for (size_t i = 0; i < 24; ++i)
        conf[i] = conf25[i];

    config.load(conf);
}

/*!
 * \brief Perform manual AS3935 antenna tuning (endless loop).
 */
void tuneAntenna()
{
    while (true)
    {
        buttonDSP.waitPressed();

        readConfiguration();

        lDet.writeConfiguration(config);

        uint8_t lcoFDIV = config.getRegister(Configuration::RegIdent::LCO_FDIV);

        //If divided frequency is in audible range, output corresponding sound using buzzer; otherwise just wait for button release
        if (lcoFDIV == 0b11 || lcoFDIV == 0b10)
        {
            size_t tBuzzerOnDelay;
            if (lcoFDIV == 0b11)
                tBuzzerOnDelay = static_cast<size_t>(1000000./3906.25*buzzerDutyCycle);
            else
                tBuzzerOnDelay = static_cast<size_t>(1000000./7812.50*buzzerDutyCycle);

            while (buttonDSP.pressed())
            {
                while (!lDet.irqHigh())
                    ;

                buzzer.switchOutputOn();

                delayMicroseconds(tBuzzerOnDelay);  //Use normal beep duty cycle to improve sound

                buzzer.switchOutputOff();

                while (lDet.irqHigh())
                    ;
            }
        }
        buttonDSP.waitReleased();
    }
}

/*!
 * \brief Perform the lightning detection (endless loop).
 */
void detectLightnings()
{
    //Enable lightning notification via buzzer beep (will be disabled when supply voltage gets too low)
    bool beepEnabled = true;

    //Detected low battery voltage during occasional VDD measurement below
    bool lowBattery = false;

    //Count number of wake-ups from sleep
    size_t wakeUpCtr = 0;

    //Count number of detected lightnings
    size_t lightningCtr = 0;

    //Accumulate passed time during sleep when waking up to occasionally trigger a VDD measurement
    size_t sleepSecsSinceLastVDDMeas = 60*vddMeasIntervalMins;  //Do initial measurement at startup

    //Latest VDD measurement result
    float supplyVoltage = 0;

    //Successively calculate the average rate of current lightning activity
    size_t sleepSecsSinceLastRateAvrg = 0;  //Passed time during sleep since last rate average
    size_t lightsSinceLastRateAvrg = 0;     //Number of lightnings since last rate average
    float lightningRate = 0;                //Lightnings per minute

    //Accumulate estimated total run time
    size_t runTimeFullHours = 0;        //Full hours of run time (only updated from below seconds upon every display update)
    size_t runTimeRemainderSecs = 0;    //Accumulated seconds of run time (carried over to above hours upon every display update)
    size_t lightsSinceLastDisplay = 0;  //Number of lightnings since last display update

    //Define a common display update routine
    auto updateDisplay = [beepEnabled, &supplyVoltage, &lightningCtr, &lightningRate,
                          &runTimeFullHours, &runTimeRemainderSecs, &lightsSinceLastDisplay]() -> void
    {
        float batteryPercentage = Auxil::calcBatteryPercentage(supplyVoltage, batteryOCVoltage0, systemMinVoltage,
                                                               systemIdleCurrent, systemMaxCurrent, systemBatteryCount);

        if (batteryPercentage < 0.0)
            batteryPercentage = 0;
        else if (batteryPercentage > 100.0)
            batteryPercentage = 100.0;

        if (beepEnabled)
            runTimeRemainderSecs += static_cast<size_t>(buzzerLightBeepSecs*lightsSinceLastDisplay);

        lightsSinceLastDisplay = 0;

        if (runTimeRemainderSecs >= 3600)
        {
            size_t runTimeRemainderHours = runTimeRemainderSecs / 3600;

            runTimeFullHours += runTimeRemainderHours;
            runTimeRemainderSecs -= runTimeRemainderHours * 3600;
        }

        float runTimeHours = runTimeFullHours + (static_cast<float>(runTimeRemainderSecs) / 3600.);

        display.init();
        display.update(lightningCtr, lightningRate, lDetStormDist, batteryPercentage, supplyVoltage,
                       runTimeHours, runMode, serialEnabled, lDetLastInterrupt);
        display.sleep();
    };

    while (true)
    {
        //Indicate wake-up reason via short RGB LED flash
        if (irqWakeAS3935)
            digitalWrite(Pins::LED_RGB_R, LOW);
        if (irqWakeClr || irqWakeDsp)
            digitalWrite(Pins::LED_RGB_G, LOW);
        if (irqWakeTimer)
            digitalWrite(Pins::LED_RGB_B, LOW);

        //Debounce pressed buttons
        if (irqWakeClr)
            buttonCLR.waitPressed();
        if (irqWakeDsp)
            buttonDSP.waitPressed();

        //Wait and turn off LEDs again (see above)
        delay(10);
        digitalWrite(Pins::LED_RGB_R, HIGH);
        digitalWrite(Pins::LED_RGB_G, HIGH);
        digitalWrite(Pins::LED_RGB_B, HIGH);

        //Calculate average rate of current lightning activity

        size_t secsSinceLastRateAvrg = sleepSecsSinceLastRateAvrg;
        if (beepEnabled)
            secsSinceLastRateAvrg += static_cast<size_t>(buzzerLightBeepSecs*lightsSinceLastRateAvrg);

        if (secsSinceLastRateAvrg >= 60*lightRateAvrgMins)
        {
            float avMins = static_cast<float>(secsSinceLastRateAvrg) / 60.;

            lightningRate = static_cast<float>(lightsSinceLastRateAvrg) / avMins;

            sleepSecsSinceLastRateAvrg = 0;
            lightsSinceLastRateAvrg = 0;
        }

        //Occasionally measure VDD (every >~ N minutes and only if already awake); adjust interval with
        //lightning activity (i.e. current consumption); always measure on push button event (user interaction)
        if (irqWakeClr || irqWakeDsp || (sleepSecsSinceLastVDDMeas >= 60*vddMeasIntervalMins) ||
                ((lightningRate > vddMeasLightRateThr) && (sleepSecsSinceLastVDDMeas >= 60*vddMeasSmallIntervalMins)))
        {
            sleepSecsSinceLastVDDMeas = 0;

            //Wait for release of pressed buttons first in order to avoid the influence of the additional current on the measurement
            if (irqWakeClr)
                buttonCLR.waitReleased();
            if (irqWakeDsp)
                buttonDSP.waitReleased();

            //Let VDD settle a bit
            delay(50);

            supplyVoltage = VDDMeasurement::measureVoltage();

            if (serialEnabled)
            {
                Serial.print("VDD: ");
                Serial.print(supplyVoltage, 3);
                Serial.print(" V\n");
            }

            //Warn when voltage is expected to drop below minimum system operating voltage during buzzer beep; also disable beep then
            if (!lowBattery && (supplyVoltage < lowBatteryThrVoltage))
            {
                lowBattery = true;

                //Play warning sound
                for (size_t i = 0; i < 6; ++i)
                {
                    delay(400);
                    buzzer.beepSingle(0.2);
                    delay(150);
                    buzzer.beepSingle(0.05);
                }

                beepEnabled = false;

                //Automatically update display to also visually notify about low battery state
                updateDisplay();

                if (serialEnabled)
                {
                    Serial.print("Warning: Low battery! Measured VDD should be >~ ");
                    Serial.print(lowBatteryThrVoltage, 3);
                    Serial.print(" V! Disabling beep and display update.\n");
                }
            }

            //Check if low voltage is still sufficient to run AS3935; if not, finally stop operation, repeatedly show LED notifications
            if (lowBattery && (supplyVoltage < as3935MinVoltage))
            {
                bool emptyBattery = true;

                //Measure voltage 10 times in 10 seconds to be sure about empty battery state
                for (size_t i = 0; i < 10; ++i)
                {
                    delay(1000);
                    if (VDDMeasurement::measureVoltage() >= as3935MinVoltage)
                    {
                        emptyBattery = false;
                        break;
                    }
                }

                //Voltage too low to still run AS3935! Stop main loop and sleep, occasionally wake up to let LED blink for notification
                if (emptyBattery)
                {
                    if (serialEnabled)
                    {
                        Serial.print("Warning: Empty battery! Measured VDD is below minimum AS3935 operating voltage of ");
                        Serial.print(as3935MinVoltage, 3);
                        Serial.print(" V! Going to sleep...\n");

                        serialEnabled = false;

                        Serial.end();
                        NRF_USBD->ENABLE = 0;
                        while (NRF_USBD->ENABLE == 1)
                            ;
                    }

                    while (true)
                    {
                        irqWakeTimer = false;

                        wakeTimer.startTimer();

                        sd_nvic_DisableIRQ(RTC1_IRQn);
                        sd_nvic_ClearPendingIRQ(RTC1_IRQn);

                        __set_FPSCR(__get_FPSCR() & ~(0x9Fu));
                        (void) __get_FPSCR();
                        sd_nvic_ClearPendingIRQ(FPU_IRQn);

                        if (timer1Used)
                            NRF_TIMER1->TASKS_STOP = 1;

                        while (!irqWakeTimer)
                            sd_app_evt_wait();

                        if (timer1Used)
                            NRF_TIMER1->TASKS_START = 1;

                        sd_nvic_EnableIRQ(RTC1_IRQn);

                        wakeTimer.stopTimer();

                        for (size_t i = 0; i < 5; ++i)
                        {
                            delay(400);
                            digitalWrite(Pins::LED_RGB_B, LOW);
                            delay(350);
                            digitalWrite(Pins::LED_RGB_B, HIGH);
                        }
                    }
                }
            }
        }

        //If woke up from AS3935, process all AS3935 events; if woke up from button,
        //stay awake until button released and poll for new AS3935 interrupts in the meantime
        while (lDet.irqHigh() || (irqWakeClr && buttonCLR.pressed()) || (irqWakeDsp && buttonDSP.pressed()))
        {
            if (lDet.irqHigh())
            {
                digitalWrite(Pins::LED_RGB_R, LOW);
                bool lightning = processInterruptAS3935();
                digitalWrite(Pins::LED_RGB_R, HIGH);

                if (lightning)
                {
                    ++lightningCtr;
                    ++lightsSinceLastRateAvrg;
                    ++lightsSinceLastDisplay;

                    if (beepEnabled)
                        buzzer.beepSingle(buzzerLightBeepSecs);
                }
            }
        }

        //If requested, reset lightning statistics (in particular also AS3935 internal statistics) and rate measurement
        if (irqWakeClr)
        {
            //Clear AS3935 lightning statistics
            lDet.clearStatistics();

            //Reset latest interrupt type, lightning energy and storm distance received from AS3935
            lDetLastInterrupt = AS3935::InterruptType::Lightning;
            lDetLastEnergy = 0;
            lDetStormDist = AS3935::stormDistanceOutOfRange;

            //Reset lightning counter
            lightningCtr = 0;

            //Reset lightning rate measurement
            sleepSecsSinceLastRateAvrg = 0;
            lightsSinceLastRateAvrg = 0;
            lightningRate = 0;
        }

        //If requested, update display content (disable updating when battery voltage gets too low)
        if (irqWakeDsp)
        {
            if (!lowBattery)
                updateDisplay();
            else
            {
                //Let LED blink briefly to remember about display not updating due to low battery voltage
                for (size_t i = 0; i < 3; ++i)
                {
                    delay(130);
                    digitalWrite(Pins::LED_RGB_G, LOW);
                    delay(20);
                    digitalWrite(Pins::LED_RGB_G, HIGH);
                }
            }
        }

        //Debounce released buttons before attaching interrupts again
        if (irqWakeClr)
            buttonCLR.waitReleased();
        if (irqWakeDsp)
            buttonDSP.waitReleased();

        //Reset interrupt flags
        irqWake = false;
        irqWakeAS3935 = false;
        irqWakeClr = false;
        irqWakeDsp = false;
        irqWakeTimer = false;

        //Go to sleep

        wakeTimer.startTimer();

        lDet.enableInterrupt(isrAS3935);
        buttonCLR.enableInterrupt(isrButtonClr);
        buttonDSP.enableInterrupt(isrButtonDsp);

        sd_nvic_DisableIRQ(RTC1_IRQn);
        sd_nvic_ClearPendingIRQ(RTC1_IRQn);

        __set_FPSCR(__get_FPSCR() & ~(0x9Fu));
        (void) __get_FPSCR();
        sd_nvic_ClearPendingIRQ(FPU_IRQn);

        if (timer1Used)
            NRF_TIMER1->TASKS_STOP = 1;

        if (!lDet.irqHigh())
        {
            while (!irqWake)
                sd_app_evt_wait();

            ++wakeUpCtr;
        }

        //Wake up again

        if (timer1Used)
            NRF_TIMER1->TASKS_START = 1;

        sd_nvic_EnableIRQ(RTC1_IRQn);

        lDet.disableInterrupt();
        buttonCLR.disableInterrupt();
        buttonDSP.disableInterrupt();

        //Accumulate sleep time

        uint32_t timerTicksSinceLastWake = wakeTimer.stopTimer();

        size_t sleepSecs;

        if (irqWakeTimer)
            sleepSecs = wakeTimerIntervalSecs;  //Use fixed interval value here as timer interrupt already cleared counter
        else
            sleepSecs = static_cast<size_t>(static_cast<float>(timerTicksSinceLastWake) / 8.);

        sleepSecsSinceLastVDDMeas += sleepSecs;
        sleepSecsSinceLastRateAvrg += sleepSecs;

        runTimeRemainderSecs += sleepSecs;
    }
}

/*!
 * \brief Retrieve/process actual information from AS3935 after receiving an interrupt request from it.
 *
 * \return True if interrupt type was AS3935::InterruptType::Lightning, false else.
 */
bool processInterruptAS3935()
{
    AS3935::InterruptType interruptType = lDet.processIRQ(lDetLastEnergy, lDetStormDist);

    if (serialEnabled)
    {
        Serial.print("Event: ");

        switch (interruptType)
        {
            case AS3935::InterruptType::DistanceChanged:
            {
                Serial.print("Distance Changed:\t{");
                Serial.print(lDetStormDist);
                Serial.print(" km [");
                Serial.print(lDetStormDist, BIN);
                Serial.print("]}");
                break;
            }
            case AS3935::InterruptType::Noise:
            {
                Serial.print("Noise!!!");
                break;
            }
            case AS3935::InterruptType::Disturber:
            {
                Serial.print("Disturber!");
                break;
            }
            case AS3935::InterruptType::Lightning:
            {
                Serial.print("Lightning:\t{");
                Serial.print("Energy: ");
                Serial.print(lDetLastEnergy);
                Serial.print(" a.u. [");
                Serial.print(lDetLastEnergy, BIN);
                Serial.print("],\t");
                Serial.print("Distance: ");
                Serial.print(lDetStormDist);
                Serial.print(" km [");
                Serial.print(lDetStormDist, BIN);
                Serial.print("]}");
                break;
            }
            case AS3935::InterruptType::Invalid:
            default:
            {
                Serial.print("UNKNOWN:\t{Interrupt Type: [");
                Serial.print(static_cast<uint8_t>(interruptType), BIN);
                Serial.print("]}");
                break;
            }
        }

        Serial.print("\n");
    }

    lDetLastInterrupt = interruptType;

    if (interruptType == AS3935::InterruptType::Lightning)
        return true;

    return false;
}

//Interrupt service routines

/*!
 * \brief ISR for wake-up from AS3935 IRQ.
 */
void isrAS3935()
{
    irqWake = true;
    irqWakeAS3935 = true;
}

/*!
 * \brief ISR for wake-up from pressed "CLR" push button.
 */
void isrButtonClr()
{
    irqWake = true;
    irqWakeClr = true;
}

/*!
 * \brief ISR for wake-up from pressed "DSP" push button.
 */
void isrButtonDsp()
{
    irqWake = true;
    irqWakeDsp = true;
}

/*!
 * \brief ISR for wake-up from RTC timer timeout.
 */
void isrWakeTimer()
{
    irqWake = true;
    irqWakeTimer = true;
}

//Define modified main function (to save power)

int main()
{
    init();
    initVariant();

    setup();

    for (;;)
        loop();

    return 0;
}

/*!
 * \brief Main setup function.
 */
void setup()
{
    //Indicate activity via RGB LEDs (controlled below)
    pinMode(Pins::LED_RGB_R, OUTPUT);
    pinMode(Pins::LED_RGB_G, OUTPUT);
    pinMode(Pins::LED_RGB_B, OUTPUT);
    digitalWrite(Pins::LED_RGB_R, HIGH);
    digitalWrite(Pins::LED_RGB_G, HIGH);
    digitalWrite(Pins::LED_RGB_B, HIGH);

    //Save power
    setupPowerSave();

    //Initialize devices

    buzzer.setup();

    buttonCLR.setup();
    buttonDSP.setup();

    dip.setup();

    lDet.setup();

    display.setup();

    //Initialize used internal periphery

    wakeTimer.setup();

    VDDMeasurement::setup();

    buzzer.beepSingle(0.1);

    //Check for user request (push button pressed during startup) to use special configuration or to start antenna tuning mode
    delay(500);
    if (buttonDSP.pressed())
        runMode = RunMode::TuneAntenna;
    else if (buttonCLR.pressed())
        runMode = RunMode::UnmaskDisturbers;
    else
        runMode = RunMode::Normal;

    buzzer.beepSingle(0.1);

    //Load configuration from DIP switches
    readConfiguration();

    //Configure lightning detector

    lDet.writeConfiguration(config);

    lDet.disableAntennaTuning();
    lDet.maskDisturbers();

    lDet.clearStatistics();

    switch(runMode)
    {
        case RunMode::TuneAntenna:
        {
            uint8_t lcoFDIV = config.getRegister(Configuration::RegIdent::LCO_FDIV);

            //Test distinguishability of buzzer response for a number of frequencies around divided resonance (only for ~4kHz and ~8kHz)
            if (lcoFDIV == 0b11)
                buzzer.testBuzzerResponse4kHz();
            else if (lcoFDIV == 0b10)
                buzzer.testBuzzerResponse4kHz(true);

            lDet.enableAntennaTuning();

            break;
        }
        case RunMode::UnmaskDisturbers:
        {
            //Enable USB CDC
            PluggableUSBD().begin();
            _SerialUSB.begin(115200);

            //Give user 5 seconds to connect with serial console
            Serial.begin(9600);
            delay(5000);
            if (Serial)
                serialEnabled = true;
            else
            {
                Serial.end();
                NRF_USBD->ENABLE = 0;
                while (NRF_USBD->ENABLE == 1)
                    ;
            }

            //Print DIP switch configuration
            if (serialEnabled)
            {
                Serial.print("Configuration: ");

                std::array<bool, 24> conf = config.getRawConfig();

                for (size_t i = 0; i < 24; ++i)
                    Serial.print(conf[i] ? 1 : 0);

                Serial.print("\n- AFE_GB: ");
                Serial.print(config.getRegister(Configuration::RegIdent::AFE_GB), BIN);
                Serial.print("\n- NF_LEV: ");
                Serial.print(config.getRegister(Configuration::RegIdent::NF_LEV), BIN);
                Serial.print("\n- WDTH: ");
                Serial.print(config.getRegister(Configuration::RegIdent::WDTH), BIN);
                Serial.print("\n- SREJ: ");
                Serial.print(config.getRegister(Configuration::RegIdent::SREJ), BIN);
                Serial.print("\n- TUN_CAP: ");
                Serial.print(config.getRegister(Configuration::RegIdent::TUN_CAP), BIN);
                Serial.print("\n- LCO_FDIV: ");
                Serial.print(config.getRegister(Configuration::RegIdent::LCO_FDIV), BIN);
                Serial.print("\n- MIN_NUM_LIGH: ");
                Serial.print(config.getRegister(Configuration::RegIdent::MIN_NUM_LIGH), BIN);
                Serial.print("\n");
            }

            lDet.unmaskDisturbers();

            break;
        }
        case RunMode::Normal:
        default:
            break;
    }

    //Initially reset and clear display and send it to sleep
    display.init();
    display.clear();
    display.sleep();

    delay(200);
    buzzer.beepSingle(0.5);
}

/*!
 * \brief Main loop function.
 */
void loop()
{
    switch(runMode)
    {
        case RunMode::TuneAntenna:
        {
            tuneAntenna();
            break;
        }
        case RunMode::UnmaskDisturbers:
        case RunMode::Normal:
        default:
        {
            detectLightnings();
            break;
        }
    }
}
