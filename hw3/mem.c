
/* 

   Physical memory management
   
   Memory size = MT_SIZE * FRAMESIZE
 */

#define MT_SIZE 4
int resident_page[MT_SIZE];
int next_frame = 0;

void init_mem(){
  int i;
  for (i=0; i < MT_SIZE; i++) {
    resident_page[i] = -1;
  }
  next_frame = 0;
}

int get_frame() {
  /* simple round robin frame manager */
  int res = next_frame;
  next_frame = (next_frame + 1) % MT_SIZE;
  return res;
}

void print_frames() {
  int i;
  printf("Next frame: %d\n", next_frame);
  for (i=0; i<MT_SIZE; i++) printf("Frame: %d is page %d\n", i, resident_page[i]);
}
