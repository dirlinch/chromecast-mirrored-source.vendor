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

#ifndef _NRD_IRRECEIVER_H
#define _NRD_IRRECEIVER_H

#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>

#define IR_PIPE 		"/tmp/pir"
#define OPEN_SLEEP		3

#define MAX_STRING	        80
#define IR_MESSAGE_LENGTH       4

#define INVALID_KEY		(0xffff)
#define NRD_VK_TABLE_END			(-1)

#define NRD_REPEAT_KEY_TIME	100000   // 150 msec


class IRReceiverCallback
{
public:
    virtual ~IRReceiverCallback() {}
    virtual void processIREvent(const int nKeyCode, const int unicode, const unsigned char modifier, bool checkFroDrop, void * pContext) = 0;
};

class IRReceiver
{
public:
    IRReceiver();
    ~IRReceiver();
    bool Init(IRReceiverCallback* callback, void* pContext);
    void Deinit();
    void* ReadEvent();
    int  GetFd() { return m_fd; }

protected:
    // clenaup all resources
    void cleanUp(int signum);
    int pipeSetup(char* pipeName);
    void SendKey(const int key, const int unicode, const unsigned char modifier, bool checkForDrop);
    int QtCodeToUnicode( unsigned int keyCode , bool isLowercase);
private:
    void * m_context;
    IRReceiverCallback* m_callback;

    char m_pipeName[MAX_STRING];
    int m_fd;
};

#endif

