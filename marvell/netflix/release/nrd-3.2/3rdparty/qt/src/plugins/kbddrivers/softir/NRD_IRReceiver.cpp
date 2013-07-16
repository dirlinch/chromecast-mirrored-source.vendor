/*
*
* This program is free software; you can redistribute it and/or modify it
* under the terms and conditions of the GNU Lesser General Public License,
* version 2.1, as published by the Free Software Foundation.
*
* This program is distributed in the hope it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
* more details.
*
* You should have received a copy of the GNU Lesser General Public License along with
* this program; if not, write to the Free Software Foundation, Inc., 
* 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
*
*/

#ifndef _NRD_IRRECEIVER_CPP
#define _NRD_IRRECEIVER_CPP

#include <QKeyEvent>
#include <errno.h>
#include <qobject.h>
#include <stdio.h>
#include <sys/timeb.h>

#include "qkbdsoftir.h"

#include "NRD_IRReceiver.h"

#define TEMP_BUF_LENGTH   (1024)

#define NRD_KEY_PARA_FUNC      "func"
#define NRD_KEY_PARA_IRCODE    "ircode"
#define NRD_KEY_PARA_IRNUMBER  "irnumber"
#define NRD_KEY_PARA_IRDEVICE  "irdevice"
#define NRD_KEY_PARA_CECCODE   "ceccode"
#define NRD_KEY_PARA_QTKEY     "qtkey"
#define NRD_KEY_PARA_ISKEYPAD  "iskeypad"
#define NRD_KEY_PARA_ISLOWERCASE  "islowercase"

#define NRD_PIC_GTV_KEY                   2
#define NRD_QT_KEY                        3

#define NRD_VK_TABLE_END			(-1)

class KeyMap : public QHash<int, Qt::Key>
{
public:
    KeyMap();
};

Q_GLOBAL_STATIC(KeyMap, keymap);

KeyMap::KeyMap()
{
    insert(0x8             , Qt::Key_Backspace);
    insert(0x9             , Qt::Key_Tab);
    insert(0xD             , Qt::Key_Return);
    insert(0x1B            , Qt::Key_Escape);
    insert(0x80            , Qt::Key_Enter);
    insert(0x82            , Qt::Key_Delete);
    insert(0x87            , Qt::Key_Left);
    insert(0x89            , Qt::Key_Right);
    insert(0x88            , Qt::Key_Up);
    insert(0x8A            , Qt::Key_Down);
    insert(0x81            , Qt::Key_Insert);
    insert(0x85            , Qt::Key_Home);
    insert(0x86            , Qt::Key_End);
    insert(0x8B            , Qt::Key_PageUp);
    insert(0x8C            , Qt::Key_PageDown);
    insert(0x84            , Qt::Key_Print);
    insert(0x90            , Qt::Key_F1);
    insert(0x91            , Qt::Key_F2);
    insert(0x92            , Qt::Key_F3);
    insert(0x93            , Qt::Key_F4);
    insert(0x94            , Qt::Key_F5);
    insert(0x95            , Qt::Key_F6);
    insert(0x96            , Qt::Key_F7);
    insert(0x97            , Qt::Key_F8);
    insert(0x98            , Qt::Key_F9);
    insert(0x99            , Qt::Key_F10);
    insert(0x9A            , Qt::Key_F11);
    insert(0x9B            , Qt::Key_F12);

    insert(0x8D            , Qt::Key_CapsLock);
    insert(0x8E            , Qt::Key_NumLock);
    insert(0x8F            , Qt::Key_ScrollLock);
    insert(0x9C            , Qt::Key_Space);
    insert(0x9D            , Qt::Key_Exclam);
    insert(0x9E            , Qt::Key_QuoteDbl);
    insert(0x9F            , Qt::Key_NumberSign);
    insert(0xA0            , Qt::Key_Dollar);
    insert(0xA1            , Qt::Key_Percent);
    insert(0xA2            , Qt::Key_Ampersand);
    insert(0xA3            , Qt::Key_Apostrophe);
    insert(0xA4            , Qt::Key_ParenLeft);
    insert(0xA5            , Qt::Key_ParenRight);
    insert(0xA6            , Qt::Key_Asterisk);
    insert(0xA7            , Qt::Key_Plus);
    insert(0xA8            , Qt::Key_Comma);
    insert(0xA9            , Qt::Key_Minus);
    insert(0xAA            , Qt::Key_Period);
    insert(0xAB            , Qt::Key_Slash);
    insert(0x30            , Qt::Key_0);
    insert(0x31            , Qt::Key_1);
    insert(0x32            , Qt::Key_2);
    insert(0x33            , Qt::Key_3);
    insert(0x34            , Qt::Key_4);
    insert(0x35            , Qt::Key_5);
    insert(0x36            , Qt::Key_6);
    insert(0x37            , Qt::Key_7);
    insert(0x38            , Qt::Key_8);
    insert(0x39            , Qt::Key_9);
    insert(0x3A            , Qt::Key_Colon);
    insert(0x3B            , Qt::Key_Semicolon);
    insert(0x3C            , Qt::Key_Less);
    insert(0x3D            , Qt::Key_Equal);
    insert(0x3E            , Qt::Key_Greater);
    insert(0x3F            , Qt::Key_Question);
    insert(0x40            , Qt::Key_At);
    insert(0x41            , Qt::Key_A);
    insert(0x42            , Qt::Key_B);
    insert(0x43            , Qt::Key_C);
    insert(0x44            , Qt::Key_D);
    insert(0x45            , Qt::Key_E);
    insert(0x46            , Qt::Key_F);
    insert(0x47            , Qt::Key_G);
    insert(0x48            , Qt::Key_H);
    insert(0x49            , Qt::Key_I);
    insert(0x4A            , Qt::Key_J);
    insert(0x4B            , Qt::Key_K);
    insert(0x4C            , Qt::Key_L);
    insert(0x4D            , Qt::Key_M);
    insert(0x4E            , Qt::Key_N);
    insert(0x4F            , Qt::Key_O);
    insert(0x50            , Qt::Key_P);
    insert(0x51            , Qt::Key_Q);
    insert(0x52            , Qt::Key_R);
    insert(0x53            , Qt::Key_S);
    insert(0x54            , Qt::Key_T);
    insert(0x55            , Qt::Key_U);
    insert(0x56            , Qt::Key_V);
    insert(0x57            , Qt::Key_W);
    insert(0x58            , Qt::Key_X);
    insert(0x59            , Qt::Key_Y);
    insert(0x5A            , Qt::Key_Z);
    insert(0x5B            , Qt::Key_BracketLeft);
    insert(0x5C            , Qt::Key_Backslash);
    insert(0x5D            , Qt::Key_BracketRight);
    insert(0x5E            , Qt::Key_AsciiCircum);
    insert(0x5F            , Qt::Key_Underscore);
    insert(0x60            , Qt::Key_At);
    insert(0x61            , Qt::Key_A);
    insert(0x62            , Qt::Key_B);
    insert(0x63            , Qt::Key_C);
    insert(0x64            , Qt::Key_D);
    insert(0x65            , Qt::Key_E);
    insert(0x66            , Qt::Key_F);
    insert(0x67            , Qt::Key_G);
    insert(0x68            , Qt::Key_H);
    insert(0x69            , Qt::Key_I);
    insert(0x6A            , Qt::Key_J);
    insert(0x6B            , Qt::Key_K);
    insert(0x6C            , Qt::Key_L);
    insert(0x6D            , Qt::Key_M);
    insert(0x6E            , Qt::Key_N);
    insert(0x6F            , Qt::Key_O);
    insert(0x70            , Qt::Key_P);
    insert(0x71            , Qt::Key_Q);
    insert(0x72            , Qt::Key_R);
    insert(0x73            , Qt::Key_S);
    insert(0x74            , Qt::Key_T);
    insert(0x75            , Qt::Key_U);
    insert(0x76            , Qt::Key_V);
    insert(0x77            , Qt::Key_W);
    insert(0x78            , Qt::Key_X);
    insert(0x79            , Qt::Key_Y);
    insert(0x7A            , Qt::Key_Z);
    insert(0x7B            , Qt::Key_BraceLeft);
    insert(0x7C            , Qt::Key_Bar);
    insert(0x7D            , Qt::Key_BraceRight);
    insert(0x7E            , Qt::Key_AsciiTilde);

    insert(0xE0            , Qt::Key_MediaPlay);
    insert(0xE1            , Qt::Key_MediaPause);
    insert(0xE2            , Qt::Key_MediaStop);
    insert(0xE3            , Qt::Key_AudioRewind);
    insert(0xE4            , Qt::Key_AudioForward);

    insert(0xE5            , Qt::Key_MediaPrevious);
    insert(0xE6            , Qt::Key_MediaNext);
    insert(0xE7            , Qt::Key_Close);
    insert(0xE8            , Qt::Key_PowerOff);
    //insert(DIKS_SUBTITLE              , Qt::Key_Subtitle);
    //insert(DIKS_TITLE                 , Qt::Key_MenuKB);
}


//*******************************
//IRReceiver implementation
//
//***********************************
// clenaup all resources
void IRReceiver::cleanUp( int sig_num )
{
    Q_UNUSED(sig_num);
    
    if (m_fd != -1)
    {
        close(m_fd);
        m_fd = -1;
    }
/*
    if (m_codes != NULL)
    {
        keyMapCleanup();
        m_codes = NULL;
    }
*/
}

// setup pipe to read IR input
// return -1 when error
int IRReceiver::pipeSetup(char* pipeName)
{
    int retVal = 0;
    unsigned char buf[TEMP_BUF_LENGTH];
    long flags;

    int rc = mkfifo(pipeName, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH);
    if (rc < 0 && errno != EEXIST)
    {
        qWarning("Could not create pipe: %s", strerror(errno));
        retVal = -1;
    }

    m_fd = open(pipeName, O_RDWR | O_NONBLOCK);
    printf("pipe open %d",m_fd);
    if (m_fd < 0)
    {
        qWarning("Could not open server pipe: %s", strerror(errno));
        retVal = -1;
    }

    if (retVal == 0)
    {
        // flush the pipe -- note any error will break the loop, not just EAGAIN
        while (read(m_fd, (void*)buf, (size_t)IR_MESSAGE_LENGTH) > 0)    ;

        flags = fcntl(m_fd, F_GETFL, 0);
        if (flags == -1)
        {
            qWarning("SoftIR: bad flags");
            retVal = -1;
        }
        else
        {
            if (fcntl(m_fd, F_SETFL, flags&(~O_NONBLOCK)) == -1)
                qWarning("SoftIR: bad noblock set: %s", strerror(errno));
        }
    }

    return retVal;
}



IRReceiver::IRReceiver()
{
    m_context = NULL;
    m_callback = NULL;

    memset( m_pipeName, 0, MAX_STRING );
    strncpy( m_pipeName, IR_PIPE, strlen(IR_PIPE) );
    m_fd = -1;
//    m_codes = NULL;
}

IRReceiver::~IRReceiver()
{
    Deinit();
}

void IRReceiver::SendKey(const int key, const int unicode, const unsigned char modifier, bool checkForDrop)
{
    if (m_callback != NULL)
    {
//        timeb tb;
//	ftime(&tb);
//	int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
   
//        printf("send key event with key %x, unicode %x, modifier %d at %d\n",key, unicode, modifier,nCount);
        m_callback->processIREvent(key, unicode, modifier, checkForDrop, m_context);
    }
}

static void PrintCurrentTime()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [128];

    memset( buffer, 0, 128 );

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    strftime (buffer,128,"(%H:%M:%S)",timeinfo);

//    qDebug("SoftIR: %s", buffer);
}


int IRReceiver::QtCodeToUnicode( unsigned int keyCode , bool isLowercase)
{
    int unicode = -1;
    
    if( keyCode <= 0x7f )
    {
        unicode = (int)keyCode;
        if( isLowercase == true )
        {
            unicode += 0x20;
        }
    }
    else
    {
        switch(keyCode)
        {
            case Qt::Key_Escape:
                unicode= 0x1b;
                break;
            case Qt::Key_Tab:
                unicode= 0x9;
                break;
            case Qt::Key_Return:
                unicode= 0x1b;
                break;
            case Qt::Key_Delete:
                unicode= 0x1b;
                break;
            default:
                unicode = -1;
                break;
        }
    }

    return unicode;
}

void * IRReceiver::ReadEvent()
{
    ssize_t readRet = 1;
    unsigned char buf[TEMP_BUF_LENGTH] = {0};
    unsigned int curKey;
    bool isKeypad;
    bool isLowercase;
    unsigned char modifier = 0;
    int unicode= -1;
    qWarning("SoftIR: read from pipe");
    readRet = read( m_fd, (void*)buf, (size_t)IR_MESSAGE_LENGTH );
    if ( readRet > 0 )
    {
        bool checkForDrop;
        // buf holds device, number
        //fprintf(stderr, "SoftIR: %x %x %x (%d)\n", buf[0], buf[1],buf[2], readRet);

        curKey = INVALID_KEY;            
        if (buf[0] == NRD_QT_KEY)
        {
            modifier = buf[1];
            curKey = buf[2];
            unicode = buf[3];
            isKeypad = false;
            checkForDrop = false;
        }
        else
        {
            // remove toggle bit for table lookup
            buf[1] &= 0x7F;
            //fprintf(stderr,"Original input 0x%x \n",buf[2]);
            isLowercase = ( (buf[2]>0x60) & (buf[2] < 0x7B) );
            curKey = keymap()->value(buf[2]);
            // set key value to invalid key code

            if (curKey != 0)  
            {	
                // send  key up to framework
                PrintCurrentTime();
                //fprintf(stderr, "SoftIR: sending key = 0x%x\n", (int)curKey);
                if (buf[0] == NRD_PIC_GTV_KEY)
                {
                    //fprintf(stderr,"GTV key 0x%x, mo %x lowercase %d\n",curKey,buf[1],isLowercase);
                    modifier = buf[1];
                }

                unicode = QtCodeToUnicode(curKey,isLowercase);
            }

            checkForDrop = true;
        }
//        timeb tb;
//        ftime(&tb);
//        int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
//        fprintf(stderr, "FromSoftIR: sending unicode = 0x%x keycode = 0x%x modifier=0x%x at %d\n", (int)unicode, (int)curKey, (int)modifier,nCount);
        SendKey((int)curKey, unicode, modifier, checkForDrop);
    }
    else if (readRet == 0)
    {
        qWarning("SoftIR: pipe EOF");
        cleanUp(0);
    }
    else
    {
        qWarning("SoftIR: read error from pipe");
        cleanUp(-1);
    }

    return NULL;
}

bool IRReceiver::Init(IRReceiverCallback* callback, void* pContext)
{
    m_context = pContext;
    m_callback = callback;

    // set up to receive IR codes -- loop here till pipe is ready
    int ret = pipeSetup( m_pipeName );
    if (ret < 0)
    {
        cleanUp(0);
        qWarning("SoftIR: Can not setup pipe \n" );
        return false;
    }

    return true;
}


void IRReceiver::Deinit( )
{
    cleanUp(0);    
}

#endif
