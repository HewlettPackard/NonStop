/* PPXSC: example program for process-pair server (Guardian or OSS) */
/* This source can be compiled as either a Guardian or OSS program. */
#pragma section ppServerName
/* Change this define if process name $PPXS conflicts on your system. */
#define ppServerName "ppxs" /* server process name (following $ or /G/) */

#pragma section rest
#include <crtlns.h> nolist
/* Note: Include 'search "$system.system.crtlnse"' in the ccomp command line
for TNS/E native programs, or ...crltnsx for TNS/x. */
#include <string.h> nolist
#include <stdio.h> nolist
#include <derror.h> nolist /* from ZGUARD, for FE... literals */
#include <zsysc.h> nolist /* from ZSYSDEFS */
#include <cextdecs.h (DEBUG, \
                    FILE_CLOSE_, \
                    FILE_GETRECEIVEINFO_,\
                    FILE_OPEN_,\
                    FILE_GETINFO_,\
                    FILE_READUPDATE64_,\
                    FILE_REPLY64_,\
                    FILE_WRITE64_,\
                    MONITORNET,\
                    MONITORCPUS,\
                    OPENER_LOST_,\
                    PROCESS_DELAY_,\
                    PROCESS_GETINFO_,\
                    PROCESS_GETINFOLIST_,\
                    PROCESS_GETPAIRINFO_,\
                    PROCESS_SETINFO_,\
                    PROCESS_SPAWN_,\
                    PROCESS_STOP_,\
                    PROCESSHANDLE_COMPARE_,\
                    PROCESSHANDLE_GETMINE_,\
                    PROCESSHANDLE_DECOMPOSE_)> nolist
#include <stdlib.h> nolist
#include <stdarg.h> nolist
#include <tdmext.h> nolist
#include <fcntl.h> nolist
#include <unistd.h> nolist
#include <tdmsig.h> nolist
#include <errno.h> nolist

enum { /* application message numbers */
        UPDATE_OUTFILE_OPEN = 1,
        UPDATE_REQUEST = 2,
        UPDATE_OPENER_ENTRY = 3,
        UPDATE_CLOSE = 4,
        SWITCHOVER = 5
};

enum { /* process creation error of interest (from dprcctl.h) */
        PROC_MONCOM = 10 /* cannot communicate with monitor */
};

enum { /* return codes of interest from PROCESS_GETPAIRINFO */
        IS_SINGLE_NAMED = 4, /* target is a single named process */
        IS_PRIMARY = 5, /* calling process is the primary */
        IS_BACKUP = 6, /* calling process is the backup */
        IS_UNNAMED = 7 /* target process is unnamed */
};

enum { /* return code of interest from PROCESSHANDLE_COMPARE_ */
        PHC_IDENTICAL = 2
};

/* processHandle (array form) */
typedef short pHandle[ZSYS_VAL_PHANDLE_WLEN];

/* opener table */
enum {
    maxOpeners = 3 /* illustrates "many" but small enough to fill easily */
};

typedef struct opener_t { /* opener table */
                    pHandle pHandleP; /* opener's primary pHandle */
                    pHandle pHandleB; /* opener's backup pHandle */
                    short fileNum; /* opener's file number */
                    unsigned short switchCount; /* switchOver counter */
                    int syncId; /* requester syncID */
                    int recNum; /* record number (reply content) */
/* reply error code belongs here, but it's always FEOK */
} opener_t;
opener_t opener[maxOpeners];

/* application messages (members of union message_t, below) */
typedef struct { /* output file backup-open state */
            long long filler; /* covers msgNumber; aligns substruct */
            __ns_std_io_file_open_state openInfo; /* from crtlns.h */
} fOpenInfo_t;

typedef struct /* backup state for "transaction" */
{
    long long filler; /* covers msgNumber; aligns substruct */
    __ns_std_io_file_state fileState; /* from crtlns.h */
    int recNum, recVal;
    short reqOpenId;
    int reqSyncId;
} fUpdateInfo_t;

typedef struct { /* switchover message */
    long long filler; /* covers msgNumber; aligns substruct */
    __ns_std_io_file_state fileState; /* from crtlns.h */
    short fault;
} switchover_t;

typedef struct { /* process open/close update message */
    short msgNumber;
    short openId;
    opener_t opener;
} pOpenUpdate_t;

typedef struct { /* process close update message */
    short msgNumber;
    short openId;
    __ns_std_io_file_state fileState; /* from crtlns.h: for close only */
} pCloseUpdate_t;

/*The following union defines message formats used by this program*/
typedef union message_t
{
    char msg[80]; /* text */
    short msgNumber; /* message number */
    /* application messages */
    fOpenInfo_t outFileOpenInfo; /* update outFile open */
    switchover_t switchover; /* switchover state */
    fUpdateInfo_t outFileUpdateInfo; /* update outFile state */
    pOpenUpdate_t openUpdate; /* update process open state */
    pCloseUpdate_t closeUpdate; /* update for a process close */
    /* system messages */
    zsys_ddl_smsg_cpudown_def CPUDown; /* CPU down message */
    zsys_ddl_smsg_cpuup_def CPUUp; /* CPU up message */
    zsys_ddl_smsg_remotecpudown_def rCPUDown;
    zsys_ddl_smsg_nodedown_def nodeDown;
    zsys_ddl_smsg_procdeath_def pDeath; /* process death message */
    zsys_ddl_smsg_open_def pOpen; /* process open message */
    zsys_ddl_smsg_close_def pClose; /* process close message */
} message_t;

#define filePos(fs) (*(long long*)&fs) /* cheat: 1st word is position */
message_t msgRead; /* message read from $RECEIVE */
int countRead; /* characters read */
/* following valid if msgRead with error 0 or 6 */
zsys_ddl_receiveinformation_def receiveInfo; /* info re msgRead */
char receiveFileName[] = "$RECEIVE";
short receiveFileNum; /* File number for $RECEIVE (it's zero, of course) */
/*Maximum disk file-name length (plus 1 to allow for NUL terminator)*/
enum {MAXNAMELEN=36};
short myCPU, myPIN, otherCPU,
who = 0 /* PROCESS_GETPAIRINFO_ value characterizing this process */;
unsigned short pgid;
char myProcessName[ZSYS_VAL_LEN_FILENAME+1];
/* File number of the backup process; used by the primary
process to send state information to the backup */
short backupFileNum = -1;
pHandle backupPHandle;
pHandle primaryPHandle;

/* Fatal termination routine:
In this pedagogical program, everything unanticipated is fatal.
A real-world application might attempt recovery in some cases. */
void die(void)
{
    DEBUG();
    PROCESS_STOP_(, 1 /*both*/, 1 /*abend*/);
}

/* Function to open home term for diagnostic logging
(Use of Guardian home term simplifies OSS pair sharing terminal.) */
short logFileNum;
void openHomeTerm(void)
{
    char fn[48];
    short fnl, e = PROCESS_GETINFO_(,,,,,,fn,(short)sizeof(fn),&fnl);
    if (e) die();
    e = FILE_OPEN_(fn, fnl, &logFileNum);
    if (e) die();
}

static const char whoID[] = "-???SPBU";
/* Output-logging function */
static void lprintf (const char *format, ...)
{
    va_list args;
    char buf[400];
    int len1, len;
    short e;
    len1 = sprintf(buf, "(%d,%d)%c", myCPU, myPIN, whoID[who]);
    va_start(args, format);
    len = vsprintf(buf+len1, format, args);
    if (len == 0 || (len+=len1) > sizeof buf) die();
    e = FILE_WRITE64_(logFileNum, buf, len);
    if (e) die();
    va_end(args);
}

/* File pointer for the output file ("database of record" for "application") */
FILE *outFile;
/* State maintained by backup (in addition to outFile state) */
int outRecNum = -1; /* record counter (serial number) */
int outRecVal; /* record value of request (arbitrary, not unique) */
short reqOpenId; /* openId of sender of request */
int reqSyncId = -1; /* syncId of request (copied to/from opener value);
>= 0 when it and reqOpenID and outRecVal are valid
and when (in backup) write is pending */
unsigned short switchovers = 0; /* Count of backup <=> primary switchovers */

/* Error routine for "simple" fatal Guardian procedure errors */
void oops(char *txt, long e)
{
    lprintf(" %s error %d", txt, e);
    die();
}

void setMe(void)
{
    pHandle me;
    short e, l;
    e = PROCESSHANDLE_GETMINE_(me);
    if (e) oops("PH_GM", e);
    e = PROCESSHANDLE_DECOMPOSE_(me, &myCPU, &myPIN, , , , ,
    myProcessName, (short)sizeof(myProcessName),
    &l);
    if (e) oops("PG_DEC(me)", e);
    myProcessName[l] = 0;
    otherCPU = (short)(myCPU ^ 1); /* our canonical partner */
}

/* helper functions to format display of messages from $RECEIVE */

/* Display I/O errors */
void IOErr(char *txt, /* identifying text */
            short fnum, /* file number, or -1 if irrelevant */
            int e, /* error code */
            int debug) /* if (e < debug) DEBUG */
{
    char buf[80];
    int i = sprintf(buf, " %s", txt);
    if (fnum >= 0)
    i += sprintf(buf+i, "(%d)", fnum);
    i += sprintf(buf+i, " error=%d", e);
    lprintf("%s", buf);
    if (debug > e) DEBUG();
} /* IOErr */

/* Send simple reply */
void emptyReply(short FE)
{
    short e = FILE_REPLY64_(, , , , FE);
    if (e) {
        char txt[12];
        sprintf(txt, "REPLY(%d)", FE);
        IOErr(txt, -1, e, 9999);
    }
}

/* This function tests for existence of the backup process */
int backupExists(void)
{
    short e = PROCESS_GETPAIRINFO_();
    if (e == IS_PRIMARY) /* we're primary, so there's a backup */
    return 1;
    if (e != IS_SINGLE_NAMED) oops("P_GPI(backup)", e);
    who = IS_SINGLE_NAMED;
    return 0;
}

#ifdef _OSS_TARGET
/* Start an OSS process */
char **argvp;
char **envp;
/* Launch a process from this program */
/* nameOpt = ZSYS_VAL_PCREATOPT_CALLERSNAME to launch OSS backup,
ZSYS_VAL_PCREATOPT_NAMEINCALL to launch OSS primary */
short doSpawn(short nameOpt, short *detail, short cpu, pHandle ph)
{
    int pid;
    char sn[9] = "/G/";
    process_extension_def pe = DEFAULT_PROCESS_EXTENSION;
    process_extension_results_def pr = DEFAULT_PROCESS_EXTENSION_RESULTS;
    strcpy(sn+3, ppServerName);
    pe.pe_process_name = sn;
    pe.pe_name_options = nameOpt;
    pid = PROCESS_SPAWN_(argvp[0],, argvp, envp,,,&pe,&pr); /*Application could be modified to PROCESS_SPAWN64_*/
    if (pid < 0) {
        lprintf(" PSPAWN: PCError=%d,%d errno=%d: %s",pr.pr_TPCerror, 
                                            pr.pr_TPCdetail, pr.pr_errno, 
                                            strerror(pr.pr_errno));
    }
    else {
        short cpu, pin;
        short e = PROCESSHANDLE_DECOMPOSE_ (pr.pr_phandle, &cpu, &pin);
    }
    if (ph) memcpy(ph, pr.pr_phandle, sizeof(pHandle));
    *detail = pr.pr_TPCdetail;
    return pr.pr_TPCerror;
} /* doSpawn */
#endif

/* If primary, tell backup about open/close status change.
Also used to pass open status to new backup. */
void updateOpenClose(short openId)
{
    opener_t *o = &opener[openId];
    char clients[22];
    int cx = 0;
    short e, cCPU, cPIN;
    clients[0] = 0;
    if (o->pHandleP[0] != -1) {
    e = PROCESSHANDLE_DECOMPOSE_(o->pHandleP, &cCPU, &cPIN);
    if (e) oops("PH_DEC(RP)", e);
        cx = sprintf(clients, " P=%d,%d", cCPU, cPIN);
    }
    if (o->pHandleB[0] != -1) {
        e = PROCESSHANDLE_DECOMPOSE_(o->pHandleB, &cCPU, &cPIN);
        if (e) oops("PH_DEC(RB)", e);
        sprintf(clients+cx, " B=%d,%d", cCPU, cPIN);
    }
    if (who == IS_PRIMARY) { /* Send opener info to backup */
        short e;
        message_t message;
        message.msgNumber = UPDATE_OPENER_ENTRY;
        message.openUpdate.openId = openId;
        memcpy(&message.openUpdate.opener, o, sizeof(opener_t));
        e = FILE_WRITE64_(backupFileNum,
        message.msg, (short)sizeof message.openUpdate);
        if (e) IOErr("WRITE pOpenUpdate", backupFileNum, e, -1);
    }
} /* updateOpenClose */

/* If primary, tell backup about close, and update outfile state
in case the last write was to this file. The file state in the
UPDATE_REQUEST was before that write (which has now occurred);
the backup uses the opener entry to repeat that write. */
void updateClose(short openId)
{
    if (who == IS_PRIMARY) { /* Send opener info to backup */
        short e;
        long error;
        message_t message;
        message.msgNumber = UPDATE_CLOSE;
        message.closeUpdate.openId = openId;
        error = __ns_fget_file_state (outFile, &message.closeUpdate.fileState);
        if (error) oops("fget_file (for close)", error);
        e = FILE_WRITE64_(backupFileNum, message.msg, (short)sizeof message.closeUpdate);
        if (e) IOErr("WRITE pCloseUpdate", backupFileNum, e, -1);
    }
} /* updateClose */

/* This function is called by the primary process to update the
state of the backup process for each "transaction."
It creates and sends a message to the backup process.
Returns true if backup exists. */
int updateBackup(void)
{
    short e;
    long error;
    message_t message;
    /* Create update message */
    message.msgNumber = UPDATE_REQUEST;
    error = __ns_fget_file_state (outFile,
    &message.outFileUpdateInfo.fileState);
    if (error) oops("fget_file", error);
    message.outFileUpdateInfo.recNum = outRecNum;
    message.outFileUpdateInfo.recVal = outRecVal;
    message.outFileUpdateInfo.reqSyncId = reqSyncId;
    message.outFileUpdateInfo.reqOpenId = reqOpenId;
    /* Send update message to backup */
    for (;;) {
        e = FILE_WRITE64_(backupFileNum,
        message.msg, (short)sizeof message.outFileUpdateInfo);
        if (e == FEOK)
        return 1; /* success */
        IOErr("WRITE", backupFileNum, e, -1);
        /* Check for existence of backup */
        if (!backupExists())
        return 0; /* caller must recreate backup */
        PROCESS_DELAY_(100000); /* wait briefly and retry the write */
    }
} /* updateBackup */

/* This function starts the backup process on otherCPU, thus making the calling
process the primary of a process pair; it opens the backup for interprocess
communication and sends it current state. If otherCPU is unavailable,
it returns. */
void initializeBackup(void)
{
    char processName [MAXNAMELEN];
    short processNameLen;
    long error;
    short e, d, openId;
    message_t message;
    int firstWho;
    if (backupFileNum > 0)
        FILE_CLOSE_(backupFileNum);
top:
    firstWho = who = PROCESS_GETPAIRINFO_(, , , , , backupPHandle);
    if (who == IS_PRIMARY) /* we're already a process pair: switching */
        goto openBackup;
    else if (who != IS_SINGLE_NAMED) 
        oops("P_GPI(backup)", who);

    /* Start the backup process */
#ifdef _OSS_TARGET
    e = doSpawn (ZSYS_VAL_PCREATOPT_CALLERSNAME /* create backup */,
    &d, otherCPU, backupPHandle);
#else
    error = __ns_start_backup (&d, otherCPU, backupPHandle);
    if (error < 0) {
        lprintf(" backup initialization failed: errno=%d: %s",
        errno, strerror(errno));
        die();
    }
    e = (short)error;
#endif
    if (e) {
        lprintf(" backup creation error %d,%d", e, d);
        if (e == PROC_MONCOM && d == FEPATHDOWN) /* no backup CPU: */
        return; /* cannot communicate with monitor in backup CPU: path down */
        die();
    }

openBackup:
    who = IS_PRIMARY; /* successful launch, or switch */
    /* Get the process name of the backup process */
    e = PROCESSHANDLE_DECOMPOSE_ (backupPHandle,
                                    /*cpu, pin*/,,
                                    /*nodenumber*/,
                                    /*nodename:nmax, nlen*/,,,
                                    processName,
                                    MAXNAMELEN,
                                    &processNameLen);
    if (e) oops("PH_DEC(backup)", e);
    processName[processNameLen] = 0;
    /* Open backup process for interprocess communication */
    e = FILE_OPEN_(processName, processNameLen, &backupFileNum);
    if (e) {
        lprintf(" F_O(backup) error %d", e);
        if (e == FEPATHDOWN) goto top;
        else die();
    }
    
    if (firstWho == IS_PRIMARY) return; /* completing switchover */

    /* Create outFile open update message */
    message.msgNumber = UPDATE_OUTFILE_OPEN;
    error = __ns_fget_file_open_state (outFile, 
                                        &message.outFileOpenInfo.openInfo);
    if (error) oops("fget_open", error);

    /* Send update messages to backup */
    e = FILE_WRITE64_(backupFileNum, message.msg, 
                                    (short)sizeof message.outFileOpenInfo);
    if (e) IOErr("WRITE", backupFileNum, e, 0);
    for (openId = 0; openId < maxOpeners; ++openId)
        if (opener[openId].fileNum >= 0)
            updateOpenClose(openId);
    
    /* Give backup primary's current (perhaps initial) state */
    if (!updateBackup())
        goto top;
} /* initialize backup */

/* function to read from $RECEIVE and characterize the input */
short receive(void)
{
    short e = FILE_READUPDATE64_(receiveFileNum,
    (char *) &msgRead,
    (short) sizeof (message_t),
    &countRead);
    if (e == FEOK || e == FESYSMESS) {
    short ee = FILE_GETRECEIVEINFO_((short*)&receiveInfo);
    if (ee) oops("F_GRI", ee);
    }
    return e;
}

/* handle an open request (system message ZSYS_VAL_SMSG_OPEN, -103) */
void doOpen(void)
{
    short e, openId;
    opener_t *o;
    if (who == IS_BACKUP && 
        PROCESSHANDLE_COMPARE_((short*)&receiveInfo.z_sender, primaryPHandle) ==
                                                            PHC_IDENTICAL) {
        emptyReply(FEOK);
        return;
    }

    /* As an example of a security check, require opener to have
    same (Guardian) group ID as this server */
    if ((unsigned short)msgRead.pOpen.z_paid >> 8 != pgid) {
        emptyReply(FESECVIOL);
        return;
    }
    if (msgRead.pOpen.z_syncdepth > 1) {
        emptyReply(FETOOMANY);
        return;
    }
    if (msgRead.pOpen.z_primary_fnum > 0) {
        /* backup open (from requester's backup */
        if (receiveInfo.z_filenum == msgRead.pOpen.z_primary_fnum)
        for (openId = 0; openId < maxOpeners; ++ openId) {
        o = &opener[openId];
            if (o->fileNum == msgRead.pOpen.z_primary_fnum /* match */
                && PROCESSHANDLE_COMPARE_((short*)&msgRead.pOpen.z_primary,
                                            o->pHandleP) == PHC_IDENTICAL) {
                memcpy(o->pHandleB, &receiveInfo.z_sender, sizeof(pHandle));
                o->switchCount = switchovers;
                goto finishOpen;
            } /* match */
        } /* for */
        emptyReply(FENOTOPEN);
    } 
    else {
        /* new open */
        openId = -1;
        { short oI = maxOpeners;
            while (--oI >= 0) {
                o = &opener[oI];
                if (o->fileNum < 0)
                    openId = oI; /* available slot */
                else if (PROCESSHANDLE_COMPARE_(o->pHandleP,
                        receiveInfo.z_sender.u_z_data.z_word)== PHC_IDENTICAL) {
                    openId = oI; goto finishOpen;
                }
            }
        } /* scope of oI */
        if (openId >= 0) { /* found a free slot */
            o = &opener[openId];
            memcpy(o->pHandleP, &receiveInfo.z_sender, sizeof(pHandle));
            o->fileNum = receiveInfo.z_filenum;
            o->syncId = -1; /* don't confuse future input with past */
            o->switchCount = switchovers;
            finishOpen:
            updateOpenClose(openId);
            { 
                zsys_ddl_smsg_open_reply_def reply;
                int n = msgRead.pOpen.z_opener_name.zlen;
                if (n && who == IS_PRIMARY) {
                    char *p = &msgRead.msg[msgRead.pOpen.z_opener_name.zoffset];
                    while ((n--) > 4 && *(p++) != '$') ;
                }
                reply.z_msgnumber = ZSYS_VAL_SMSG_OPEN;
                reply.z_openid = openId;
                e = FILE_REPLY64_((char*)&reply, (short)sizeof reply, , FEOK);
                if (e) IOErr("REPLY(openID)", -1, e, 9999);
            } /* local block */
        } else { /* no free slot */
            emptyReply(FEINUSE);
        }
    } /* new open */
} /* doOpen */

int checkPair(); /* forward */

/* handle a close request (system message ZSYS_VAL_SMSG_CLOSE, -104) */
void doClose(void)
{
    short openId = receiveInfo.z_openlabel;
    if ((unsigned)openId >= maxOpeners) {
    /* If we're the backup, this might be the primary, but it's not safe
    to take over until we get the death message, so we don't checkPair(). */
    } else {
        short P, B, gone = 0;
        opener_t *o = &opener[openId];
        P = PROCESSHANDLE_COMPARE_((short*)&receiveInfo.z_sender, 
                                                (short*)&o->pHandleP);
        B = PROCESSHANDLE_COMPARE_((short*)&receiveInfo.z_sender,
                                                (short*)&o->pHandleB);
        if (P == PHC_IDENTICAL) /* requester primary gone */
            if (o->pHandleB[0] != -1) { /* backup remains: promote backup */
                memcpy(o->pHandleP, o->pHandleB, sizeof(pHandle));
                memset(o->pHandleB, -1, sizeof(pHandle));
            } else gone = 1;
        else if (B == PHC_IDENTICAL) /* requester backup gone */
            if (o->pHandleP[0] != -1) { /* primary remains */
                memset(o->pHandleB, -1, sizeof(pHandle));
            } else gone = 1;
        
        if (gone) {
            memset(o, -1, sizeof(opener)); /* free the entry */
            updateClose(openId);
        } else
            updateOpenClose(openId); /* o has changed, but not freed */
        updateOpenClose(openId);
    }
    emptyReply(FEOK);
} /* doClose */

/* Handle a failure message */
void purgeOpeners(void)
{ 
    /* Primary and backup each get the message and call this function.
     * There can't be a stale Open/Close update message "in transit" because
     * writes to the backup are waited. 
     */
    enum { /* expected return codes from OPENER_LOST_() */
    searchCompleted = 0,
    lostPrimary = 4,
    lostBackup = 5,
    lostOpener = 6};
    short openId = -1;
    for (;;) {
        short e = OPENER_LOST_(msgRead.msg, (short)countRead, (short*)opener, 
                                &openId, maxOpeners,
                                (short)(sizeof(opener_t)/sizeof(short)));
        switch (e) {
        case searchCompleted:
            emptyReply(FEOK);
            return;
        case lostOpener:
            opener[openId].fileNum = -1; /* free the entry */
        case lostPrimary:
        case lostBackup:
            continue;
        default:
            oops("OPENER_LOST", e);
        } /* switch */
    } /* forever */
} /* purgeOpeners */

/* Check the status of the pair (after a process death or other calamaty
message). Returns true if this is backup taking over. */
int checkPair(void)
{
    short e = PROCESS_GETPAIRINFO_();
    if (e == IS_SINGLE_NAMED)
    if (who == IS_BACKUP) {
        who = IS_SINGLE_NAMED;
        if (outRecNum < 0) {
            lprintf(" *** Double failure: backup took over before initialized");
            PROCESS_STOP_(, , 1 /*abend*/);
        }
        return 1; /* takeover */
    } else if (who == IS_PRIMARY) {
        who = IS_SINGLE_NAMED;
        initializeBackup();
    }
    return 0;
}

/* Process a system message. Returns true if this is backup taking over. */
int processSysMsg(void)
{
    switch (msgRead.msgNumber) {
    case ZSYS_VAL_SMSG_REMOTECPUDOWN:
    case ZSYS_VAL_SMSG_NODEDOWN:
    case ZSYS_VAL_SMSG_CPUDOWN:
        purgeOpeners(); /* includes reply */
        return (msgRead.msgNumber == ZSYS_VAL_SMSG_CPUDOWN) ? checkPair() : 0;
    case ZSYS_VAL_SMSG_CPUUP:
        emptyReply(FEOK);
        if (who == IS_SINGLE_NAMED && msgRead.CPUUp.z_cpunumber == otherCPU)
        initializeBackup(); /* our partner CPU showed up */
        return 0;
    case ZSYS_VAL_SMSG_PROCDEATH:
        emptyReply(FEOK);
        return checkPair();
    case ZSYS_VAL_SMSG_OPEN:
        doOpen(); /* includes reply */
        break;
    case ZSYS_VAL_SMSG_CLOSE:
        doClose(); /* includes reply */
        break;
    default: /* uninteresting */
        emptyReply(FEOK);
    } /* switch */
    return 0;
} /* processSysMsg */

/* This function writes the next record to outFile. It's normally called from
processRequest(), but is also used by the backup process at takeover. */
void writeOutFile(void)
{
    int E;
    char *failer = "fflush(outFile)"; /* tentative, pessimistic */
    /* write to file and flush the write */
    errno = -1; /* don't trust fprintf to set errno */
    E = fprintf(outFile, "%5d %-5d [%d:%d] (%d,%d)%c\n",
                                    outRecNum, outRecVal, reqOpenId, reqSyncId,
                                    myCPU, myPIN, whoID[who]);
    if (E < 0) failer = "fprintf(outFile)";
    else {
        /* The following code copes with two issues:
        1: Occasionlly, shortly after a takeover,
        fflush(outFile) returns an error with errno == FEFILEFULL.
        Retry that error (once) after a delay.
        2: Once the stream has an error, subsequent fprintf() calls fail,
        so when the fflush() retry is successful, clear the error state. */
        int retry = 2;
        while (--retry >= 0) {
            errno = -1;
            E = fflush(outFile);
            if (E == 0) break;
            if (errno == FEFILEFULL) {
                PROCESS_DELAY_(1000000);
            }
        }
        if (retry < 1 && E == 0) /* fflush() retry succeded */
            clearerr(outFile); /* clear error state so fprintf() can work */
    }
    if (E < 0)
        IOErr(failer, -1, errno, 9999);
} /* writeOutFile */

/* This function processes the text or empty request message in msgRead;
it represents the "application business logic" of the server.
It is normally called in the primary, but can be called from the backup
if the primary dies and the backup gets the request before takeover.
Returns 0 normally, 1 if sucessful switchover. */
int processRequest(void)
{
    short e;
    char repBuf[12];
    int repLen;
    opener_t *o;
    reqSyncId = -1; /* irrelevant until set in normalRequest */
    msgRead.msg[countRead] = 0;
    o = &opener[receiveInfo.z_openlabel];
    if (PROCESSHANDLE_COMPARE_((short*)&receiveInfo.z_sender, o->pHandleP) != PHC_IDENTICAL &&
        PROCESSHANDLE_COMPARE_((short*)&receiveInfo.z_sender, o->pHandleB) != PHC_IDENTICAL) {
        /* Can occur if a requester has an open to a defunct earlier process
        with the same name as this server */
        emptyReply(FEWRONGID);
        return 0;
    }
    switch (msgRead.msg[0]) {
    case 0: /* empty request */
        break; /* to empty reply */
    case ' ': case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': /* normal data request */
        goto normalRequest;
    case 's': /* switchover */ {
        short e;
        long error;
        message_t message;
        /* Create switchover message */
        message.msgNumber = SWITCHOVER;
        error = __ns_fget_file_state (outFile, &message.switchover.fileState);
        if (error) oops("fget_file: switchover", error);
        message.switchover.fault = (msgRead.msg[1] == '9');
        /* Send update message to backup */
        e = FILE_WRITE64_ (backupFileNum, message.msg, (short)sizeof message.switchover);
        if (e) {
            emptyReply(FENOSWITCH); /* abuse error code */
            return 0;
        }
        ++switchovers;
        emptyReply(FEOK);
        return 1; /* switchover */
    }
    case 'q': /* quit */
        emptyReply(FEEOF);
        exit(EXIT_SUCCESS); /* Stops primary and backup processes */
    default: /* unrecognized */
        emptyReply(FEINVALOP);
        return 0;
    } /* switch */

    emptyReply(FEOK);
    return 0;

normalRequest:
    if (receiveInfo.z_syncid == o->syncId) {
        /* This path normally occurs in the backup, but not necessarily. */
        goto reply; /* already written & updated; still needs reply */
    }
    outRecVal = atoi((char*)&msgRead);
    ++outRecNum;
    reqOpenId = receiveInfo.z_openlabel;
    reqSyncId = receiveInfo.z_syncid;
    if (who == IS_PRIMARY)
        if (!updateBackup())
            initializeBackup();
    o->syncId = reqSyncId;
    o->recNum = outRecNum;
    writeOutFile();
    reqSyncId = -1; /* (in backup) prevent another write */
reply:
    repLen = sprintf(repBuf, "%d", o->recNum);
    e = FILE_REPLY64_(repBuf, (short)repLen, , , FEOK);
    if (e) IOErr("REPLY(FEOK)", -1, e, 9999);
    return 0;
} /* processRequest */

/* This function runs forever in the primary. It consists of a loop that reads
and processes input from $RECEIVE. It returns only for switchover. */
void primaryProcessing(void)
{
    short e;
    lprintf(" Primary processing");
    for (;;) {
        /* Read the next message from $RECEIVE with the intention of replying */
        e = receive();
        if (e == FEOK) { /* no error */
            if (countRead == 0 || msgRead.msgNumber > 255 /* text: normal request */) {
                if (processRequest()) /* includes reply */
                    return; /* switchover: become backup */
            } else {
            emptyReply(FEILLEGALREQUEST);
            }
        } /* no error */
        else if (e == FESYSMESS) /* system message */
            processSysMsg(); /* includes reply; doesn't return nonzero for primary */
        else {
            IOErr("READUPDATE", receiveFileNum, e, 9999);
            emptyReply(FEOK);
        }
    } /* forever */
} /* primaryProcessing */

/* This function performs the backup processing:
it reads and processes messages from $RECEIVE.
It returns upon takeover or switchover. */
void backupProcessing(void)
{
    short e;
    long error;
    lprintf(" Backup processing");
    who = PROCESS_GETPAIRINFO_(, , , , primaryPHandle);
    if (who != IS_BACKUP) return; /* try to become the primary */

    /* Infinite loop that receives and processes messages */
    for (;;) {
        /* Read the next message from $RECEIVE with the intention of replying */
        e = receive();
        if (e == FEOK) { /* no error, user message */
        if (countRead == 0 || msgRead.msgNumber > 255) {
            /* surprise! we got a request message (takeover may be imminent) */
            short openId = receiveInfo.z_openlabel;
            if ((unsigned)openId >= maxOpeners) { 
                lprintf(" Invalid openID: %d", openId); die(); 
            }
            if (opener[openId].switchCount != switchovers) {
                /* first request from this source since switchover: reject it */
                emptyReply(FEOWNERSHIP);
                opener[openId].switchCount = switchovers; /* reject just one */
            } else { /* process the message */
                if (reqSyncId >= 0) { /* first, write any pending record */
                    outRecNum = opener[reqOpenId].recNum;
                    writeOutFile();
                    opener[reqOpenId].syncId = reqSyncId;
                    reqSyncId = -1; /* prevent another write */
                }
                if (processRequest()) /* includes reply */
                    die(); /* can't happen */
            }
            continue;
        }
        switch (msgRead.msgNumber) {
        case UPDATE_OUTFILE_OPEN:
            /* Backup open the output file */
            outFile = __ns_backup_fopen(&msgRead.outFileOpenInfo.openInfo);
            if (outFile == NULL) {
                lprintf(" outFile bu open failed; errno=%d: %s",
                errno, strerror(errno));
                die();
            }
            break; /* to reply */
        case UPDATE_REQUEST:
            /* Update output file state and related data */
            error = __ns_fset_file_state (outFile,
            &msgRead.outFileUpdateInfo.fileState);
            if (error) oops("fset_file", error);
            outRecNum = msgRead.outFileUpdateInfo.recNum;
            outRecVal = msgRead.outFileUpdateInfo.recVal;
            reqSyncId = msgRead.outFileUpdateInfo.reqSyncId;
            reqOpenId = msgRead.outFileUpdateInfo.reqOpenId;
            if (reqOpenId >= 0) { /* save data to write/reply if necessary */
                opener[reqOpenId].syncId = reqSyncId;
                opener[reqOpenId].recNum = outRecNum;
            }
            emptyReply(FEOK);
            continue;
        case UPDATE_OPENER_ENTRY:
            memcpy(&opener[msgRead.openUpdate.openId],
            &msgRead.openUpdate.opener, sizeof(opener_t));
            break; /* to reply */
        case UPDATE_CLOSE:
            /* Update file_state: primary writes after updateBackup() call.
            Primary has finished that write. Backup redoing it depends on
            opener state, some of which we're about to clobber. */
            error = __ns_fset_file_state (outFile, &msgRead.closeUpdate.fileState);
            if (error) oops("fset_file (for close)", error);
            memset(&opener[msgRead.closeUpdate.openId], -1, sizeof(opener_t));
            reqSyncId = -1; /* no pending write */
            break;
        case SWITCHOVER: {
            short setVal = 1; /* become primary */
            /* Update output file state and related data */
            error = __ns_fset_file_state (outFile,
            &msgRead.switchover.fileState);
            if (error) oops("fset_file: switchover", error);
            reqSyncId = -1; /* nothing to write */
            e = PROCESS_SETINFO_(, , ZSYS_VAL_PINF_PRIMARY, &setVal, 1);
            if (e) oops("P_SI(primary)", e);
            who = PROCESS_GETPAIRINFO_();
            if (who != IS_PRIMARY && who != IS_SINGLE_NAMED) die();
            emptyReply(FEOK);
            return; /* as primary */
        }
        default:
            die();
        } /* switch */
        emptyReply(FEOK);
        } else if (e == FESYSMESS) { /* system message */
            if (processSysMsg()) return; /* takeover */
        } else {
            emptyReply(FEOK);
            IOErr("READUPDATE", receiveFileNum, e, 9999);
        }
    } /* forever */
} /* backupProcessing */

/* The main function determines whether it is executing as the primary or
backup process and takes appropriate action. For OSS, it can also be
an unnamed process, which launches the primary and stays around until
it goes away, lending its homeTerm as log collector for the pair. */
int main(int argc, char *argv[], char *env[])
{
    short e, d, len, attx;
    char outNameDefault[] = "ppxsvout";
    char *outName = outNameDefault;
    if (SIGACTION_INIT_((sighandler3_t)SIG_DEBUG))
        PROCESS_STOP_(, 1 /*both*/, 1 /*abend*/);
    memset(opener, -1, sizeof opener);
    setMe();
    openHomeTerm();
    who = PROCESS_GETPAIRINFO_();
    switch (who) {
    case IS_SINGLE_NAMED: /* proto-parent */
    case IS_BACKUP: /* backup */
        break;
    case IS_PRIMARY: /* nonsense*/
        lprintf(" Process can't already be primary!");
        die();
    case IS_UNNAMED: /* unnamed */
#ifdef _OSS_TARGET
        break;
#else
        lprintf(" Server process must be named; requester expects $%s.",
        ppServerName);
        exit(EXIT_FAILURE);
#endif
    default:
        oops("initial P_GPI", who);
        exit(EXIT_FAILURE);
    } /* switch */

    /* Acquire our Process Access Id (for optional security check) */
    attx = ZSYS_VAL_PINF_PROCESS_AID;
    e = PROCESS_GETINFOLIST_(, , , , , &attx, 1, (short*)&pgid, 1, &len, &d);
    if (e) oops("PGIL(PAID)", e);
    pgid >>= 8;

    /* Open $RECEIVE */
    e = FILE_OPEN_ (receiveFileName, (short)(sizeof receiveFileName -1), 
                        &receiveFileNum,/* access */,/* exclusion */,/* nowait */,
                        /* receive depth */1);
    if (e) oops("F_O($RECEIVE)", e);
#ifdef _OSS_TARGET
    argvp = argv;
    envp = env;
    if (who == IS_UNNAMED) { /* initial program to launch server */
        /* There are three reasons for this approach to initializing the OSS
        process pair:
        1: Because PROCESS_SPAWN[64]_ (rather than the shell) launches the primary,
        both halves of the pair have Guardian but not OSS parentage;
        the pair is not subject to termination by a group kill.
        2: Both processes have the same default file numbers for root and cwd,
        rather than inheriting whatever file numbers were in use in the
        shell. (Each cd command moves cwd to a different file number.
        If they're different in the primary and backup, the outFile might
        be opened in the primary using a number not available in the backup.)
        3: This program is a pedagogical exercise with diagnostic output from a
        succession of processes. By sitting quietly and holding its session
        open, the initial process keeps its terminal window available for the
        output of all the other processes. */
        short d;
        e = doSpawn(ZSYS_VAL_PCREATOPT_NAMEINCALL /* create primary */, &d, myCPU, 0 /* discard pHandle */);
        if (e)
            return lprintf(" PSPAWN primary failed: error=%d,%d", e, d), 1;
        else { /* successful spawn */
            for (;;) {
                /* Read the next message from $RECEIVE with the intention
                of replying */
                e = receive();
                if (e == FEOK) { /* no error: user message (unexpected) */
                    emptyReply(FEILLEGALREQUEST);
                    continue;
                } else if (e == FESYSMESS) { /* system message */
                   emptyReply(FEOK);
                    if (msgRead.msgNumber == ZSYS_VAL_SMSG_PROCDEATH) {
                        exit(EXIT_SUCCESS);
                    }
                    else if (msgRead.msgNumber == ZSYS_VAL_SMSG_OPEN) {
                        emptyReply(FEINVALOP);
                        continue;
                    }
                } else IOErr("READUPDATE", receiveFileNum, e, 0);
            } /* forever */
        } /* successful spawn */
    } /* unnamed initial program */
#endif
    MONITORCPUS(-1);
    MONITORNET(1);
    if (who == IS_BACKUP) { /* backup */
switchedToBackup:
        backupProcessing();
        /* returned: takeover or switchover */
        if (backupExists()) goto switchedToPrimary;
        /* Perform any pending write first thing after takeover */
        if (reqSyncId >= 0) {
            outRecNum = opener[reqOpenId].recNum;
            writeOutFile();
            opener[reqOpenId].syncId = reqSyncId;
            reqSyncId = -1; /* prevent another write */
        }
        initializeBackup (); /* Start replacement backup process */
        /* Continue the work of the application */
    } else { /* primary */
        if (argc > 1) {
            if (argc > 2) return lprintf("Too many parameters"), 1;
            outName = argv[1];
        }
        /* Open file with sync depth 1 */
        outFile = __ns_fopen_special (outName, "ab+", 1);
        if (!outFile)
        return lprintf(" outFile open failed: errno=%d:%s",
        errno, strerror(errno)), 1;
        fprintf(outFile, "(%d,%d) opened outFile\n", myCPU, myPIN);
        outRecNum = 0;
switchedToPrimary:
        initializeBackup();
    }
    primaryProcessing();
    e = FILE_CLOSE_(backupFileNum);
    if (e) IOErr("F_C(backup)", backupFileNum, e, 9999);
    who = IS_BACKUP;
    goto switchedToBackup;
}
