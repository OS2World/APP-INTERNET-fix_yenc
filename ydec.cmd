/* This is REXX! */

/* (c) Bart Green */
/* a quick function to fixup a yenc file and decode it using ydec-os2 */
/* Update 09-30-2002,  (Re-written after disk crash) */
/* Update 02-09-2004   Removed last var in the .dec parse to allow spaces in file names */
/*                      Also had to strip out the leading space(s) from the fpath       */

/*'@ECHO OFF'*/
Call RxFuncAdd 'SysLoadFuncs', 'REXXUTIL', 'SysLoadFuncs'
Call SysLoadFuncs
call RxFuncAdd 'SysCls', 'RexxUtil', 'SysCls'

myrc = 0

arg filename junk

say "Processing file: '"filename"'"

lastdot = LASTPOS('.', filename)
t_name = SUBSTR(filename, 1, lastdot)

o_name = t_name"$$$"
/* say "Lastpos = "lastdot */
say "Output file: '"o_name"'"


if o_name = filename then
  Do
    say ""
    say "ydec:Error - Input and output files must have different names."
    say "             The $$$ extension is reserved for the output filename."
    call beep 300,250
    call beep 392,250
    call beep 300,250
    exit
  End


'Fix_Yenc "'filename'" "'o_name'" -debug '
if RC \= 0 then
  do
    say ""
    say "Fixup process completed with errors"
    call beep 392,250
    exit
  end

/* Run decoder (with debug option '/a') */
/* 'ydec-os2 "'o_name'" /a' */
'ydec-os2 "'o_name'"'


/* Now purge all the .dec files of length = 0 */

 Found.0 = 0
 num_deleted = 0
 fdate = ""
 ftime = ""
 fsize = ""
 fattr = ""
 fpath = ""
 Call SysFileTree '*.dec','Found','F'     /* Search for files,  Return all info */

 if Found.0 > 0 then
   Do
   say "Cleaning up .dec files, "Found.0" Files found."
     Do cur_file = 1 To Found.0
         /* Info format
           12:15:89  12:01a     494942  A-HRS  C:\path\filename.ext
           12:15:89   2:00p      23221  -----  C:\path\filename3.ext
         */
       /* parse var Found.cur_file fdate ftime fsize fattr fpath junk */
       /* Removed junk var from end to allow for files with spaces in the name */
       parse var Found.cur_file fdate ftime fsize fattr fpath
       fpath2 = STRIP(fpath,'L')
       if fsize = 0 then
         Do
           rc = SysFileDelete(fpath2)
       /*    say 'Debug - DEL "'fpath2'"'  */
           if rc = 0 then
             Do
             num_deleted = num_deleted + 1
             End
         End
     End
   say "  "num_deleted" Files Deleted."
   End

