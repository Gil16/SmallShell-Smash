#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
	cout << "smash: got ctrl-Z" << endl;
	SmallShell& smash = SmallShell::getInstance();
	JobsList::JobEntry* foreground = smash.m_pJobsList->getForegroundJob();
	if(!foreground) {
		return;
	}
	smash.m_pJobsList->addJob(foreground->sCommand, foreground->nId, foreground->status); // add to the jobs list
	kill(foreground->PID, SIGSTOP);	
	smash.m_pJobsList->updateJobstatusByPID(JobsList::EJobStatus::eJobStatus_Stopped, foreground->PID); // stopped
	cout << "smash: process " << foreground->PID << " was stopped" << endl;
}

void ctrlCHandler(int sig_num) {
	cout << "smash: got ctrl-C" << endl;
	SmallShell& smash = SmallShell::getInstance();
	JobsList::JobEntry* foreground = smash.m_pJobsList->getForegroundJob();
	if(!foreground) {
		return;
	}
	kill(foreground->PID, SIGKILL);	
	cout << "smash: process " << foreground->PID << " was killed" << endl;
}

