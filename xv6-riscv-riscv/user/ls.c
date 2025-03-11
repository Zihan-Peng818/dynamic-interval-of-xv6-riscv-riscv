#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %d\n", fmtname(path), st.type, st.ino, (int) st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, (int) st.size);
    }
    break;
  }
  close(fd);
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
main(int argc, char *argv[])
{
  int i;


  uint64 start_ticks = uptime();
  uint64 start_context_switches = get_context_switch_count();
  uint64 start_time = rtime(); 


  if(argc < 2){
    uint64 start_ticks = uptime();
    uint64 start_context_switches = get_context_switch_count();
    uint64 start_time = rtime(); 
    ls(".");
    uint64 end_ticks = uptime();
    uint64 end_context_switches = get_context_switch_count();
    uint64 end_time = rtime();
    uint64 total_ticks = end_ticks - start_ticks;
    uint64 total_context_switches = end_context_switches - start_context_switches;
    uint64 total_time = end_time - start_time;
    fprintf(1,"ticks: ");
    printint(1, total_ticks, 10, 0);
    fprintf(1,"\n");
    fprintf(1,"context switches: ");
    printint(1, total_context_switches, 10, 0);
    fprintf(1,"\n");
    fprintf(1,"time: ");
    printint(1, total_time, 10, 0);
    fprintf(1,"\n");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  uint64 end_ticks = uptime();
  uint64 end_context_switches = get_context_switch_count();
  uint64 end_time = rtime();
  uint64 total_ticks = end_ticks - start_ticks;
  uint64 total_context_switches = end_context_switches - start_context_switches;
  uint64 total_time = end_time - start_time;
  fprintf(1,"ticks: ");
  printint(1, total_ticks, 10, 0);
  fprintf(1,"\n");
  fprintf(1,"context switches: ");
  printint(1, total_context_switches, 10, 0);
  fprintf(1,"\n");
  fprintf(1,"time: ");
  printint(1, total_time, 10, 0);
  fprintf(1,"\n");

  exit(0);
}
