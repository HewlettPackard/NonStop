/* PPFTI: example program for process-pair server using NSFTI library */

/* Start the program as a name OSS process */

#ifdef __cplusplus
   extern "C" {
#endif

#include <stdio.h> nolist
#include <stdarg.h> nolist

#include <derror.h> nolist /* from ZGUARD, for FE... literals */
#include <zsysc> nolist /* from ZSYSDEFS */
#include <cextdecs.h> nolist 

enum { /* return code of interest from PROCESSHANDLE_COMPARE_ */
        PHC_IDENTICAL = 2
};

/*The following union defines message formats used by this program*/
typedef union message_t
{
    char msg[80]; /* text */
    short msgNumber; /* message number */
    /* system messages */
    zsys_ddl_smsg_cpudown_def CPUDown; /* CPU down message */
    zsys_ddl_smsg_cpuup_def CPUUp; /* CPU up message */
    zsys_ddl_smsg_remotecpudown_def rCPUDown;
    zsys_ddl_smsg_nodedown_def nodeDown;
    zsys_ddl_smsg_procdeath_def pDeath; /* process death message */
    zsys_ddl_smsg_open_def pOpen; /* process open message */
    zsys_ddl_smsg_close_def pClose; /* process close message */
} message_t;


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
    short me[10];
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

/* function to read from $RECEIVE and characterize the input */
short receive(void)
{
    short e = FILE_READUPDATE64_(receiveFileNum, (char *) &msgRead, 
                                   (short) sizeof (message_t), &countRead);
    if (e == FEOK || e == FESYSMESS) {
    short ee = FILE_GETRECEIVEINFO_((short*)&receiveInfo);
    if (ee) oops("F_GRI", ee);
    }
    return e;
}

#ifdef __cplusplus
   }
#endif

#include <nsfti/ftilib.h>
using namespace std;

using namespace nsfti;

shared_ptr<Map<std::string,long>> stateMap;
shared_ptr<FTILib> ftiLibInstance;

const std::string maxOid("MaxOid");
const std::string statemapKeyname("stateMap");

/* handle a close request (system message ZSYS_VAL_SMSG_CLOSE, -104) */
void doClose(void)
{
    short openId = receiveInfo.z_openlabel;
    /* TODO: To clean the openID table map */
    emptyReply(FEOK);
} /* doClose */

/* handle an open request (system message ZSYS_VAL_SMSG_OPEN, -103) */
void doOpen(void)
{
    short e;

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
    
    /* new open */
    long openId; 
    try {
        openId = stateMap->get(maxOid);
        openId++;
    }
    catch (std::exception& e) {
        openId = 0;
    }
    stateMap->insert(maxOid, openId);

    /* TODO: Reuse openID & track open information, to avoid duplicate requests */

    zsys_ddl_smsg_open_reply_def reply;
    reply.z_msgnumber = ZSYS_VAL_SMSG_OPEN;
    reply.z_openid = (short)openId;
    e = FILE_REPLY64_((char*)&reply, (short)sizeof reply, , FEOK);
    if (e) IOErr("REPLY(openID)", -1, e, 9999);
   
} /* doOpen */

/* Process a system message. Returns true if this is backup taking over. */
int processSysMsg(void)
{
    long openId = 0;
    switch (msgRead.msgNumber) {
    case ZSYS_VAL_SMSG_REMOTECPUDOWN:
    case ZSYS_VAL_SMSG_NODEDOWN:
    case ZSYS_VAL_SMSG_CPUDOWN:
        /* TODO: How to handle CPU?? */
//        return (msgRead.msgNumber == ZSYS_VAL_SMSG_CPUDOWN) ? checkPair() : 0;
        return 0;
    case ZSYS_VAL_SMSG_CPUUP:
        emptyReply(FEOK);
        /* Dumy request so that the library can restart the backup process */
        openId = stateMap->get(maxOid);
        return 0;
    case ZSYS_VAL_SMSG_PROCDEATH:
        emptyReply(FEOK);
        return 0;
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



/* This function processes the text or empty request message in msgRead;
 * it represents the "application business logic" of the server.
 */
int processRequest(void)
{
    short e;
    char repBuf[12];
    int repLen;

    shared_ptr<Map<std::string, long>> rec;
    shared_ptr<MapFactory> factory = ftiLibInstance->getMapFactory();
    const std::string keynames[] = {"RecNum", "RecVal", "ReqOpenId", "ReqSyncId"}; 
    const std::string datumkey ="DatumKey";

    reqSyncId = -1; /* irrelevant until set in normalRequest */
    msgRead.msg[countRead] = 0;

    switch (msgRead.msg[0]) {
    case 0: /* empty request */
        break; /* to empty reply */
    case ' ': case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9': /* normal data request */
        goto normalRequest;
    case 's': /* switchover */ {
        /* NSFTI library does not support manual switch over */
        emptyReply(FEINVALOP);
    }
    case 'q': /* quit */
        /* Stops primary and backup processes */
        ftiLibInstance->shutdown(EXIT_SUCCESS);
    default: /* unrecognized */
        emptyReply(FEINVALOP);
        return 0;
    } /* switch */

    emptyReply(FEOK);
    return 0;

normalRequest:
    outRecVal = atoi((char*)&msgRead);
    try {
        outRecNum = stateMap->get(datumkey);
    }
    catch (std::exception& e) {
        outRecNum = -1;
    }
    ++outRecNum;

    stateMap->insert(datumkey, outRecNum);
    std::string recname = "rec" + std::to_string(receiveInfo.z_openlabel);
    
	try {
		rec = factory->createNamedMap<std::string, long>(recname);
	} catch (char *err) {
		lprintf("After map create failed %s", err);
        emptyReply(FEINVALOP);
	}

	try {
		rec->insert(keynames[0], outRecNum);
		rec->insert(keynames[1], outRecVal);
		rec->insert(keynames[2], receiveInfo.z_openlabel);
		rec->insert(keynames[3], receiveInfo.z_syncid);
	} catch(std::exception& e)  {
			lprintf("Unable to inserted new record %s", e.what());
    }

    repLen = sprintf(repBuf, "%d", outRecNum);
    e = FILE_REPLY64_(repBuf, (short)repLen, , , FEOK);
    if (e) IOErr("REPLY(FEOK)", -1, e, 9999);
    return 0;
} /* processRequest */

/* This function runs forever in the primary. It consists of a loop that reads
 * and processes input from $RECEIVE. 
 */
void primaryProcessing(void)
{
    short e;
    lprintf(" Primary processing");

    shared_ptr<MapFactory> factory = ftiLibInstance->getMapFactory();
    try {
		stateMap = factory->createNamedMap<std::string, long>(statemapKeyname);
	} catch (char *err) {
		lprintf("After map create failed %s", err);
		exit(EXIT_FAILURE);
	}

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

int main(int argc, char *argv[], char *env[])
{
    short e, d, len, attx;
    setMe();
    openHomeTerm();


    shared_ptr<nsfti::GlobalConfig> nsftiConfig = FTILib::getGlobalConfig();
    /* Set back-up cpu configuration */
    nsftiConfig->setCPU(otherCPU);

    nsftiConfig->setProgramArgs(argc, argv);

    try {
        /* initialize the NSFTILib */
        std::shared_ptr<FTILib> initRet = FTILib::initialize();
        if (!initRet) {
            lprintf("NSFTI library initialization failed.");
            exit(EXIT_FAILURE);
        }
    } catch (char *err) {
        lprintf("After init, init failed: %s", err);
        exit(EXIT_FAILURE);
    }

    /* Get the NSFTI instance */
    ftiLibInstance = FTILib::getInstance();

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
    primaryProcessing();
}
