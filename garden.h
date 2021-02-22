#ifndef GARDEN_H
#define GARDEN_H



typedef struct Game_Memory {
  //MERK Alt i PermanentStorage må være initialisert til 0
  int IsInitialized;
  
  int PermanentStorageSize;
  void *PermanentStorage; 

  int TransientStorageSize;
  void *TransientStorage; 
} Game_Memory;

typedef struct Game_State {
  Stone stone;
} Game_State;

/*
internal void GameUpdateAndRender(game_memory *Memory,
                                  game_input *Input,
                                  game_offscreen_buffer *VideoBuffer,
                                  game_sound_output_buffer *SoundBuffer);
*/
#endif 
