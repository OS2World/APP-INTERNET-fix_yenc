    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³                                                                     ³*/
    /*³ File Name: Fix_Yenc                                                 ³*/
    /*³                                                                     ³*/
    /*³ Programmer: B. Green  (c) 9-2002                                    ³*/
    /*³                                                                     ³*/
    /*³ Function: This program is a quick build to fixup a yenc encoded     ³*/
    /*³           file as saved from PMiNews.  The first character may be   ³*/
    /*³           incorrect if two dots '.' start a line.                   ³*/
    /*³                                                                     ³*/
    /*³ Hardware: IBM PC/XT/PPC/AT with at least 1 disk or diskette         ³*/
    /*³                                                                     ³*/
    /*³ Software: OS/2 2.X and above                                        ³*/
    /*³                                                                     ³*/
    /*³ Related Files:                                                      ³*/
    /*³                                                                     ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#define FIX_YENC_VERSION "V0.12 10-05-2002"

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³                    ---  Change Summary  ---                         ³*/
    /*³                                                                     ³*/
    /*³  09-30-2002 B. E. Green                                      V0.10  ³*/
    /*³             Attemped to re-create the program after the HD crash    ³*/
    /*³             on my machine.                                          ³*/
    /*³  10-04-2002 B. E. Green                                      V0.11  ³*/
    /*³             Fix any line in block starting with '..'                ³*/
    /*³  10-05-2002 B. E. Green                                      V0.12  ³*/
    /*³             look for =ybegin instead of =ypart to start block       ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

    /*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
    /*³           Include files for 'C' defines and functions               ³*/
    /*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/

#define LINT_ARGS

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <process.h>
#include <time.h>
#include <malloc.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

#define ULONG unsigned long
#define UCHAR unsigned char

#define FALSE (0)
#define TRUE !FALSE



//--------------------------------------------------------------------------------
void StripCrLf(char data[])
  {
  int i   = 0;
  int len = 0;

  len = strlen(data);
  for (i=0; i<len; i++)
    {
    if ((data[i] == '\r') || (data[i] == '\n'))
      {
      data[i] = (char)'\0';
      return;
      }
    }
  }



//--------------------------------------------------------------------------------
int  main(int argc, char *argv[])
  {
  FILE *infile = NULL;
  FILE *outfile = NULL;
  char  tbuff[2048] = "";
  char  fname_in[512] = "";
  char  fname_out[512] = "";
  int   in_block = FALSE;
  int   linecount = 0;
  int   lines_fixed = 0;
  int   part_count = 0;
  int   debug_flag = FALSE;

  if (argc < 3)
    {
    printf("\nError - Syntax error in entry");
    printf("\n      - Fix_Yenc file_to_fix output_file [-debug]");
    exit(30);
    }

  strcpy(fname_in, argv[1]);    // copy file name argument
  strcpy(fname_out, argv[2]);   // copy file name argument
  debug_flag = FALSE;
  if (argc == 4)
    {
    if (strcmpi(argv[3], "-debug") == 0)
      {
      debug_flag = TRUE;
      }
    }

  if ((infile = fopen(fname_in,"r")) == NULL)
    {
    perror("Fix_Yenc - File open fail");
    return(11);
    }

  if ((outfile = fopen(fname_out,"w")) == NULL)
    {
    perror("Fix_Yenc - File open fail");
    fclose(infile);
    return(11);
    }

  printf("\nFix_Yenc Version %s", FIX_YENC_VERSION);
  if (debug_flag)
    {
    printf("\n  Debug Enabled");
    }
  printf("\n  Input file:  \'%s\'", fname_in);
  printf("\n  Output file: \'%s\'", fname_out);

  fgets(tbuff, 2048, infile);
  while (!feof(infile))
    {
    linecount++;
    if (in_block)
      {
      if (strnicmp(tbuff, "=yend", 5) == 0)
        {
        // start of block found,  start processing
        in_block = FALSE;
        fputs(tbuff, outfile);
        }
#if 0  // only fix long lines and filter out the ones with = at 128
      else if (strlen(tbuff) >= 129)
        {
        if ( (tbuff[0] == '.') && (tbuff[1] == '.') && (tbuff[127] != '=') )
          {
          if (debug_flag)
            {
            printf("\n      Line %5d:  Len: %d", linecount, strlen(tbuff) );
            }
          fputs(&tbuff[1], outfile);   // Write out the data skipping first byte
          lines_fixed++;
          }
        else
          {
          fputs(tbuff, outfile);
          }
        }
#else     // try if for all cases in block starting with 2 dots '..'
      else if ( (tbuff[0] == '.') && (tbuff[1] == '.') )
        {
        if (debug_flag)
          {
          printf("\n      Line %5d:  Len: %d", linecount, strlen(tbuff) );
          }
        fputs(&tbuff[1], outfile);   // Write out the data skipping first byte
        lines_fixed++;
        }
#endif
      else
        {
        fputs(tbuff, outfile);
        }
      }
    else
      {
      fputs(tbuff, outfile);
//      if (strnicmp(tbuff, "=ypart", 6) == 0)
      if (strnicmp(tbuff, "=ybegin", 7) == 0)
        {
        // start of block found,  start processing
        in_block = TRUE;
        part_count++;
        if (debug_flag)
          {
          printf("\n    Part %d", part_count);
          }
        }
      }

    fgets(tbuff, 2048, infile);       // get next line
    }

  fclose(infile);
  fclose(outfile);

  printf("\n  Parts Found:     %d", part_count);
  printf("\n  Lines Processed: %d", linecount);
  printf("\n  Lines Fixed:     %d", lines_fixed);
  printf("\n");

  return(0);
  }


