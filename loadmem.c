/* hex2bas --- convert hex checksum format to BASIC         2014-03-25 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YES 1
#define NO  0
#define MAXLINE (256)

#define USR (0x0228)

typedef int address;

unsigned char Mem[256];
address Lowest, Highest;

void loadmem (const char *fname);

int main (int argc, const char *argv[])
{
   int line = 7000;
   int i;
   address addr = USR;
   
   Lowest = 0xffff;
   Highest = 0;
   
   loadmem ("flap.hex");
   
   printf ("Address range is $%04x to $%04x\n", Lowest, Highest);
   
   printf ("%d RESTORE:FORAD=%dTO%d:READBY:POKEAD,BY:NEXT\n",
            line, Lowest, Highest);
   line += 10;
   printf ("%d POKE11,%d:POKE12,%d:RETURN\n",
            line, addr & 0xff, addr >> 8);
   line += 10;
   
   i = 0;
   
   for (addr = Lowest; addr <= Highest; addr++) {
      if (i == 0) {
         printf ("%d DATA", line);
         line += 10;
      }
      
      printf ("%d", Mem[addr - 0x200]);
      i++;
      
      if ((i == 15) || (addr == Highest)) {
         printf ("\n");
         i = 0;
      }
      else {
         printf (",");
      }
   }
   
   return (0);
}


void store (address addr, int byte)
{
   Mem[addr - 0x200] = byte;
   
   if (addr < Lowest)
      Lowest = addr;
      
   if (addr > Highest)
      Highest = addr;
}


/* loadmem --- load memory with ROM data and extracodes */

void loadmem (const char *fname)
{
   FILE *fp;
   char lin[MAXLINE];
   int err = NO;
   int i = 0;
   
   if ((fp = fopen (fname, "r")) == NULL) {
      fputs ("can't open ROM file\n", stderr);
      exit (1);
   }
   
   while (fgets (lin, MAXLINE, fp) != NULL) {
      i++;
      
      if (lin[0] == ';') {
         if (datablock (lin, fname, i) == NO)
            err = YES;
      }
      else {
         fprintf (stderr, "%s: line %d invalid:\n", fname, i);
         fputs (lin, stderr);
         err = YES;
      }
   }
   
   fclose (fp);
   
   if (err) {
      fputs ("Errors in checksum data file prevent simulator from running\n", stderr);
      exit (1);
   }
}


/* datablock --- decode a UK101 format data block & store in memory */

int datablock (lin, fname, l)
char *lin;
char *fname;
int l;
{
   address a;
   char len[3];
   char addr[5];
   char byte[3];
   char chks[5];
   int nchars;
   int slen;
   unsigned int nbytes;
   unsigned int b;
   unsigned int sum1;   /* Checksum read from file */
   unsigned int sum2;   /* Checksum computed from data bytes */
   int i, j;
   
   slen = strlen (lin);

   len[0] = lin[1];
   len[1] = lin[2];
   len[2] = '\0';
   
   addr[0] = lin[3];
   addr[1] = lin[4];
   addr[2] = lin[5];
   addr[3] = lin[6];
   addr[4] = '\0';
   
   if (sscanf (len, "%x", &nbytes) != 1) {
      fprintf (stderr, "%s: invalid length byte (%s) in line %d:\n", fname, len, l);
      fputs (lin, stderr);
      return (NO);
   }

   if (sscanf (addr, "%x", &a) != 1) {
      fprintf (stderr, "%s: invalid address (%s) in line %d:\n", fname, addr, l);
      fputs (lin, stderr);
      return (NO);
   }
   
   nchars = (nbytes * 2) + 12;
   
#ifdef DB
   printf ("addr = %04x, len = %02x, nchars = %d, strlen = %d\n",
                   a,          nbytes,        nchars,      slen);
#endif
   
   if (nchars > slen) {
      fprintf (stderr, "%s: line %d too short:\n", fname, l);
      fputs (lin, stderr);
      return (NO);
   }
   else if (nchars < slen) {
      fprintf (stderr, "%s: line %d too long:\n", fname, l);
      fputs (lin, stderr);
      return (NO);
   }
   
   chks[0] = lin[nchars - 5];
   chks[1] = lin[nchars - 4];
   chks[2] = lin[nchars - 3];
   chks[3] = lin[nchars - 2];
   chks[4] = '\0';
   
   if (sscanf (chks, "%x", &sum1) != 1) {
      fprintf (stderr, "%s: invalid checksum (%s) in line %d:\n", fname, chks, l);
      fputs (lin, stderr);
      return (NO);
   }
   
   sum2 = nbytes;
   sum2 += a & 0xff;
   sum2 += (a >> 8) & 0xff;

   for (i = 0, j = 7; i < nbytes; i++) {
      byte[0] = lin[j++];
      byte[1] = lin[j++];
      byte[2] = '\0';
      
      sscanf (byte, "%x", &b);
      store (a + i, b);
      sum2 += b;
   }
   
   if (sum1 != sum2) {
      fprintf (stderr, "%s: checksum error in line %d: computed %04x, read %s\n", fname, l, sum2, chks);
      fputs (lin, stderr);
      return (NO);
   }
   
   return (YES);
}
