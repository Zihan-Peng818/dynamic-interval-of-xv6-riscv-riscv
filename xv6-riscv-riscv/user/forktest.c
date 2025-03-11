// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/riscv.h"
#include "user/user.h"

#define N  1000

void
print(const char *s)
{
  write(1, s, strlen(s));
}



void
forktest(void)
{
  int n, pid;

  print("fork test\n");

  for(n=0; n<N; n++){
    pid = fork();
    if(pid < 0)
      break;
    if(pid == 0)
      exit(0);
  }

  if(n == N){
    print("fork claimed to work N times!\n");
    exit(1);
  }

  for(; n > 0; n--){
    if(wait(0) < 0){
      print("wait stopped early\n");
      exit(1);
    }
  }

  if(wait(0) != -1){
    print("wait got too many\n");
    exit(1);
  }
  
  print("fork test OK\n");
}






static void putc(int fd, char c){
  write(fd, &c, 1);}
static char digits[] = "0123456789ABCDEF";
static void printint(int fd, int xx, int base, int sgn){
  char buf[16];
  int i, neg;
  uint x;
  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }
  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';
  while(--i >= 0)
    putc(fd, buf[i]);}





int
main(void)
{ uint64 start_ticks = uptime();
  uint64 start_context_switches = get_context_switch_count();
  uint64 start_time = rtime(); 
  forktest();
  uint64 end_ticks = uptime();
  uint64 end_context_switches = get_context_switch_count();
  uint64 end_time = rtime();
  
  uint64 total_ticks = end_ticks - start_ticks;
  uint64 total_context_switches = end_context_switches - start_context_switches;
  uint64 total_time = end_time - start_time;
  
  print("ticks: ");
  printint(1, total_ticks, 10, 0);
  print("\n");
  print("context switches: ");
  printint(1, total_context_switches, 10, 0);
  print("\n");
  print("time: ");
  printint(1, total_time, 10, 0);
  print("\n");
  
  exit(0);
}
