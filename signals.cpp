#include <iostream>
#include <signal.h>
#include "signals.h"
#include <time.h>
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
	cout << "smash: got ctrl-Z" << endl;
	SmallShell& smash = SmallShell::getInstance();
	JobEntry* foreground = smash.m_pForeground;
	if(!foreground)
		return;
	foreground->status = EJobStatus::eJobStatus_Stopped;
	foreground->is_stopped = true;
	foreground->time_ended = time(NULL);
	
	smash.m_pJobsList->addJob(foreground);
	kill(foreground->PID, SIGSTOP);	
	cout << "smash: process " << foreground->PID << " was stopped" << endl;
	
	smash.m_pJobsList->LastStopped = smash.m_pJobsList->getJobByPID(foreground->PID)->nId;
	smash.m_pForeground = nullptr;
} 

void ctrlCHandler(int sig_num) {
	cout << "smash: got ctrl-C" << endl;
	SmallShell& smash = SmallShell::getInstance();
	JobEntry* foreground = smash.m_pForeground;
	if(!foreground)
		return;	
	kill(foreground->PID, SIGKILL);	
	cout << "smash: process " << foreground->PID << " was killed" << endl;
	smash.m_pJobsList->removeJobByPID(foreground->PID);
	smash.m_pForeground = nullptr;
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

