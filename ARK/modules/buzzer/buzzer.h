/*
                                        ::                                                      
                                        ::                                                      
                                        ::                                                      
                                        ::                                                      
                                        ::                                                      
    ..    ..........    :.      ::      ::     .........  ..    ..........    ...      .        
    ::    ::            : .:.   ::     .::.       ::      ::    ::       :    :: :.    :        
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
* File:        buzzer.h
* Author:      Kunsh Jain
* Created On:  2025-12-22
* Brief:       Buzzer driver interface.
* Description: Simple PWM-based buzzer helper for tones and melodies.
***************************************************************************************************
* HISTORY:
* +----- (NEW | MODify | ADD | DELete)
* |
* No#   |       when       who                  what
******+*********+**********+********************+**************************************************
* 000  NEW      2025-12-22   Kunsh Jain           Added header and Doxygen
* 000  MODIFY   2026-01-09   Pranjal Giri         Added mute/unmute Volume and State functionality
**************************************************************************************************/

#ifndef BUZZER_H
#define BUZZER_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

/** \brief Simple PWM buzzer helper class. */
class Buzzer {
public:
    /** \brief Create buzzer bound to `pin` (default 6). */
    Buzzer(uint pin = 6);
    /** \brief Initialize PWM hardware for buzzer. */
    void Init();
    /** \brief Play a tone at `freq` Hz (non-blocking). */
    void PlayTone(uint freq);
    /** \brief Play a short melody (Mario) - blocking. */
    void PlayMario();
    /** \brief Stop/silence the buzzer. */
    void Stop();
    /** \brief Mute output without stopping state. */
    void Mute();
    /** \brief Unmuted the output. */
    void Unmute();
    /** \brief Set volume 0-100. */
    void SetVolume(uint8_t vol);
    /** \brief Get current volume. */
    uint8_t GetVolume() const;
    /** \brief Set armed state. */
    void SetArmed(bool armed);
    /** \brief Check if it is armed. */
    bool IsArmed() const;
private:
    uint _pin;
    uint _slice;
    uint8_t _volume = 50;
    bool _armed = false;
    bool _muted = false;
    void _applyLevel();
};

#endif