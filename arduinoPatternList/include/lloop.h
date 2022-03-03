#ifndef LLOOP_H
#define LLOOP_H

  #include <Arduino.h>
  #include <adri_timer.h>

  #include "constants.h"

  class ListLoop
  {

    // uint32_t  _delay        = 35;
    uint32_t  _delayCurrent = 35;
    boolean   _delayManual  = false;
    boolean   _delayMin     = false;
    
    boolean   _play         = false;
    boolean   _pause        = false;

    boolean   _random       = false;
    
  protected:
    adri_timer  * _timer;

    LL_flag _pltFlag;
    LL_flag _lbtFlag;

    void get_remainingTime(uint32_t & v);
    void get_delay(uint32_t&v) {v=_delayCurrent;}
    void get_delayMin(boolean&v) {v=_delayMin;}

    boolean isPause();
    boolean isRndPlaying();    

    void set_rnd();
    void set_delayMin();
    void set_delayMin(boolean);
    void set_delayCurrent(const uint32_t &);
    void set_delay(const uint32_t &);
    void set_delayManual(boolean);
    void set_pos(LL_flag * flag, const uint16_t &);

    void loop_next(LL_flag * flag);
    void loop_prev(LL_flag * flag);
    void next(LL_flag * flag);
    void prev(LL_flag * flag);
    void rnd(LL_flag * flag); 

    void setup(LL_flag * flag, const uint16_t & v);
    void reset(LL_flag * flag, int v);
    void reset();

    void start();
    void stop();       
    void pause();       
    void resume();  
    // int       _pos          = 0;
    // uint8_t   _posMin       = 0;
    // uint16_t  _posMax       = 0;        
  public:
    ListLoop();
    ~ListLoop();
    boolean isPlaying();


  };


#endif // LLOOP_H