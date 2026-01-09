/*
                                        ::                                                      
                                        ::                                                      
                                        ::                                                      
                                        ::                                                      
                                        ::                                                      
    ..    ..........    :.      ::      ::     .........  ..    ..........    ...      .        
    ::    ::            : .:.   ::      .::.       ::      ::    ::       :    :: :.    :        
    ::    ::   ..:::    :   .:. ::    ::::::      ::      ::    ::       :    ::   ::  :        
    ::    ::......::    :      :::    ::::::      ::      ::    ::.......:    ::     :::        
                                      ::::::                                                    
                                      :.::.:                                                    
                         .::::          ::          ::::.                                       
                       .::::::::.       ::       .:::::::::                                     
                       ::::::::::::....::::.....:::::::::::                                     
                        .:::::::::::::::::::::::::::::::::.        

                  © Copyright of Ignition Avionics
*/

/**************************************************************************************************
* File:        buzzer.cpp
* Author:      Kunsh Jain
* Created On:  2025-12-22
* Brief:       Buzzer driver implementation.
* Description: Implements PWM initialization and tone/melody playback.
***************************************************************************************************
* HISTORY:
* +----- (NEW | MODify | ADD | DELete)
* |
* No#   |       when       who                  what
******+*********+**********+********************+**************************************************
* 000  NEW      2025-12-22   Kunsh Jain           Added header and Doxygen
* 001  MODify   2025-12-24   Kunsh Jain           Added Stop() and made PlayTone non-blocking
* 002  MODify   2026-01-09   Pranjal Giri         Added Mute/Unmute functionality
* 003  MODify   2026-01-10   Pranjal Giri         Added Volume and SetArmed/IsArmed functionality
**************************************************************************************************/

#include "buzzer.h"

Buzzer::Buzzer(uint pin) : _pin(pin), _volume(50), _armed(false), _muted(false) {}

void Buzzer::Init() {
    gpio_set_function(_pin, GPIO_FUNC_PWM);
    _slice = pwm_gpio_to_slice_num(_pin);
    pwm_set_wrap(_slice, 65535);
    pwm_set_chan_level(_slice, pwm_gpio_to_channel(_pin), 0);
    pwm_set_enabled(_slice, true);
}

void Buzzer::PlayTone(uint freq) {
    if (freq == 0) {
        Stop();
        return;
    }
    uint32_t clock = 125000000;
    uint32_t divider = clock / (freq * 65536);
    if (divider < 1) divider = 1;
    pwm_set_clkdiv(_slice, (float)divider);
    pwm_set_wrap(_slice, 65535);
    _applyLevel();
}

void Buzzer::Stop() {
    pwm_set_chan_level(_slice, pwm_gpio_to_channel(_pin), 0);
}

void Buzzer::Mute() {
    _muted = true;
    pwm_set_chan_level(_slice, pwm_gpio_to_channel(_pin), 0);
}

void Buzzer::Unmute() {
    _muted = false;
    _applyLevel();
}

void Buzzer::SetVolume(uint8_t vol) {
    _volume = (vol > 100) ? 100 : vol;
    _applyLevel();
}

uint8_t Buzzer::GetVolume() const { return _volume; }

void Buzzer::SetArmed(bool armed) {
    _armed = armed;
    _applyLevel();
}

bool Buzzer::IsArmed() const { return _armed; }

void Buzzer::_applyLevel() {
    if (!_armed || _muted) {
        pwm_set_chan_level(_slice, pwm_gpio_to_channel(_pin), 0);
    } else {
        uint16_t level = (_volume * 328);  // 0-100 -> 0-32800 (~50% max duty)
        pwm_set_chan_level(_slice, pwm_gpio_to_channel(_pin), level);
    }
}

void Buzzer::PlayMario() {
    uint16_t mel[11][2] = {
        {660, 150}, {660, 150}, {0, 150}, {660, 150}, {0, 150},
        {523, 150}, {660, 150}, {0, 150}, {784, 150}, {0, 150}, {392, 150}
    };
    for (int i = 0; i < 11; i++) {
        if (mel[i][0] == 0) {
            Stop();
        } else {
            PlayTone(mel[i][0]);
        }
        sleep_ms(mel[i][1]);
    }
    Stop();
}