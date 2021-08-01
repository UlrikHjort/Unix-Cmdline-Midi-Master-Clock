/**************************************************************************
--                      Unix Midi Master Clock 
-- 
--           Copyright (C) 2021 By Ulrik Hørlyk Hjort
--
--  This Program is Free Software; You Can Redistribute It and/or
--  Modify It Under The Terms of The GNU General Public License
--  As Published By The Free Software Foundation; Either Version 2
--  of The License, or (at Your Option) Any Later Version.
--
--  This Program is Distributed in The Hope That It Will Be Useful,
--  But WITHOUT ANY WARRANTY; Without Even The Implied Warranty of
--  MERCHANTABILITY or FITNESS for A PARTICULAR PURPOSE.  See The
--  GNU General Public License for More Details.
--
-- You Should Have Received A Copy of The GNU General Public License
-- Along with This Program; if not, See <Http://Www.Gnu.Org/Licenses/>.
***************************************************************************/
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

/*************************************************
 *
 * Prototypes
 *
 *************************************************/
uint32_t current_time_ms (void);
void send_clock(int, uint32_t);

/*************************************************
 *
 * Returns current time in ms
 *
 *************************************************/
uint32_t current_time_ms (void) {

    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);
    
    return (spec.tv_sec + round(spec.tv_nsec / 1.0e6)); // resolution in ms
}

/*****************************************************
 *
 *  Send midi time sync cmd to device at actual bpm.
 *  Run clock at 24 pulses per quarter note.
 *
 ****************************************************/
void send_clock(int fd, uint32_t bpm) {

  uint8_t  time_sync_cmd[] = {0xF8};
  uint32_t tempo = 1000/(bpm/60.0);
  
  uint32_t interval = tempo/24.0; // 24 ppqn    
  uint32_t prev_time = 0;
  
  while (1) {
   
    uint32_t  current_time = current_time_ms();

    if (current_time - prev_time > interval) {
      prev_time = current_time;
      write(fd, time_sync_cmd, 1);	  
    }
  }
}

/*************************************************
 *
 * main
 *
 *************************************************/
int main(int argc, char *argv[]) {

   if (argc != 3) {
     printf("Usage %s <device> <tempo>\n",argv[0]);
     printf("E.g %s /dev/midi 120\n",argv[0]);	 
     exit(1);
   }
   
   int fd = open(argv[1], O_WRONLY, 0);
   if (fd < 0) {
     printf("Error: cannot open %s\n", argv[1]);
     exit(1);
   }

   send_clock(fd, atoi(argv[2]));

   return 0;
}
