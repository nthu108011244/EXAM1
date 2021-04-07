# EXAM1
# Inputs and display

  InterruptIn button_up(D10);\
  InterruptIn button_down(D12);\
  InterruptIn button_select(D11);\
  void monitor_display_menu();\
  void monitor_display_wave();\
  void ISR1 ();\
  void ISR2 ();\
  void ISR3 ();\
  bool if_change = 1;\
  bool if_generate = 0;

# Generate waveform

  AnalogOut aout(D7);\
  Thread waveThread;\
  EventQueue waveQueue;\
  int slew = 1;\
  int slew_mode = 3;\
  int slew_table[4] = {8, 4, 2, 1};\
  int slew_time = 0;\
  void wave_generate();
  
# Sample

  AnalogIn ain(A0);\
  Thread sampleThread;\
  EventQueue sampleQueue;\
  
  
