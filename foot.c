#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <linux/input.h>
#include <linux/uinput.h>

/* This was intended to be a X input driver for a certain footpedal.
   Regrettably - either it or the usb driver is flaky at the moment.

   I'll go back to working on it whenever I figure out if it is the
   hardware or me.

/*      Dictation commands:
 
	PedalCommand("Rew", KeyCombo("XF86AudioPrev")),
	PedalCommand("Play", KeyCombo("XF86AudioPlay")),
	PedalCommand("FF", KeyCombo("XF86AudioNext")),

	Emacs commands:
*/

#define FOOTPEDAL "/dev/usb/hiddev0"

static int footread(char *buf, int block) {
  static int fd = 0;
  if(buf == NULL) { close(fd); return -1; }

  if(fd == 0) {
    fd = open(FOOTPEDAL, O_RDONLY);
    if(fd < 1) { perror("can't open hid device"); exit(1); }
  }

  if(block) fcntl(fd,F_SETFD,O_RDONLY|O_NONBLOCK);
  
  int size = read(fd,buf,24);
  if(size !=24 ) perror("wtf\n");
  
  // Nonblocking can lead to partial reads
  
  if(block) {
    while(size < 24 && size > 0) {
      printf("Had a partial read of %x\n", size);
      size += read(fd,&buf[size],24 - size);
    }
  }

  if(block) fcntl(fd,F_SETFD,O_RDONLY);
  
  if(size == 24) 
    return buf[4] << 2 | buf[12] << 1 | buf[20];

  return -1;
}

void hexread() {
  int newstate = 0;
  int oldstate = 0;
  char buf[255];
  while(1) {
    newstate = footread(buf,0);
    // keybounce, sliding off the left or right
    /*    if(oldstate & 5 && newstate & 2) {
      usleep(200);
      int tmp = footread(buf, 0);
      if (tmp != -1 && tmp != newstate) newstate = tmp;
    }
    */
    printf("%x\n", newstate);
  }
  footread(NULL,0);
}

int main() {
  hexread();
}
