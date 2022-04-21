/* PPXRC: Example requester program to drive PPXS... */
/* This source creates a Guardian or OSS requester program.
Both drive either the Guardian or OSS version of the example server. */
#include <stdio.h> nolist
#include <string.h> nolist
#include <stdarg.h> nolist
#include <derror.h> nolist /* from ZGUARD, for FE... literals */
#include <zsysc.h> nolist /* from ZSYSDEFS */
#include <cextdecs.h(DEBUG, FILE_OPEN_, FILE_READUPDATE64_, \
                    FILE_WRITEREAD64_, PROCESS_STOP_)>
short svFNum; /* server process file number */
char svName[8] = "$"; /* server process file name */
short svNameLen; /* server file name length */
enum {inBufSize = 16};
int main(int argc, char *argv[])
{
    char inBuf[inBufSize]; /* buffer to read from terminal */
    short len; /* length of terminal input */
    short e;
    strcpy(svName+1, argv[1]);
    svNameLen = (short)strlen(svName);
    /* Open server with sync depth 1 */
    e = FILE_OPEN_(svName, svNameLen, &svFNum, , , , 1);
    if (e) return printf("F_O(%s) error %d\n", svName, e), 3;

    puts("Enter data or commands, 1 per line, to be fed to the server.\n"
    "Commands have a lowercase letter in column 1:\n"
    " 'q' causes server and requester to terminate.\n"
    " 's' causes switchover: primary and backup exchange roles.\n"
    " Any other letter is rejected with error 2 (FEINVALOP).\n"
    "Data requests begin with a digit or blank.\n"
    "An empty input sends an empty request;\n"
    " for which the server does nothing but reply.\n"
    "An eof causes the requester to terminate.\n");

    for (;;) { /* main loop */
        enum {readMaxLen = 4};
        char readBuf[readMaxLen+1];
        char *p;
        int countRead;
        fputc(':', stdout);
        p = fgets(inBuf, sizeof inBuf, stdin);
        len = 0;
        if (p) {
            len = (short)strlen(inBuf);
        if (inBuf[len-1] == '\n')
            inBuf[--len] = 0;
        }
        else if (feof(stdin)) {
            break;
        }
        else if (ferror(stdin)) { 
            return 3; 
        }
        printf("Wrote %.*s (length=%d)", len, inBuf, len);
        e = FILE_WRITEREAD64_(svFNum, inBuf, readBuf, len, readMaxLen, &countRead);
        if (e != FEOK) {
            printf("; WRITEREAD error %d\n", e);
            if (e == FEPATHDOWN) return 2;
            if (e == FEEOF) break; /* server's response for 'q' */
            continue;
        }
        if (countRead)
            printf("; read %.*s (length=%d)\n", countRead, readBuf, countRead);
        else
            printf("; read nothing\n");
    } /* forever */
    return 0;
}
