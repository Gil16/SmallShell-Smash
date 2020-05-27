#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
	cout << "smash: got ctrl-Z" << endl;
	SmallShell& smash = SmallShell::getInstance();
	JobEntry* foreground = smash.m_pForeground;
	if(!foreground) {
		return;
	}
	kill(foreground->PID, SIGSTOP);	
	smash.m_pJobsList->addJob(foreground->sCommand, foreground->PID, EJobStatus::eJobStatus_Stopped);
	cout << "smash: process " << foreground->PID << " was stopped" << endl;
	smash.m_pForeground = nullptr;
}

void ctrlCHandler(int sig_num) {
	cout << "smash: got ctrl-C" << endl;
	SmallShell& smash = SmallShell::getInstance();
	JobEntry* foreground = smash.m_pForeground;
	if(!foreground) {
		return;
	}
	kill(foreground->PID, SIGKILL);	
	cout << "smash: process " << foreground->PID << " was killed" << endl;
	smash.m_pJobsList->removeJobByPID(foreground->PID);
	delete smash.m_pForeground;
	smash.m_pForeground = nullptr;
}
