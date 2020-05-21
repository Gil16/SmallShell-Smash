#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include <time.h>
//#include "Commands.h"

using namespace std;

const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY()  \
  cerr << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cerr << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

#define DEBUG_PRINT cerr << "DEBUG: "

class JobsList {
  vector<JobEntry> * m_pvJobs;
  JobEntry m_pForeground;
  int maxJobId=0;
// TODO: Add your data members
public:
 JobsList()
 {
     m_pvJobs = new vector ... ;
     memset(&m_pForeground, 0, sizeof(m_pForeground));
 };
 ~JobsList()
 {
     delete m_pvJobs;
 };

 void addJob(string a_strCommand, int a_nPid, int a_nstatusJobs, int a_nJobId=-1, bool a_bForeground = false);
 void applyToAll(void (*a_pfun)(JobEntry));
 void removeJobByPlace(int a_viJobs);// goes to node in vector and checks.If finished-removes.
 void printJobByPlace(int a_viJobs);// goes to node and prints info
 void removeFinishedJobs();
 JobEntry * getJobById(int a_jobId);
 void removeJobById(int a_jobId);
 JobEntry * getLastJob();
 JobEntry * getForegroundJob();
 JobEntry *getLastStoppedJob();
 // TODO: Add extra methods or modify exisitng ones as needed
};

// a_nJobId=-1 ,a_bForeground=false in case job brought first time to list,and has no Jid assigned
// a_nJobId>0, a_bForeground=true if fg command issued on job and job
// will be removed from vector and added to m_pForeground
// if it was brought second time after fg command will be used 
// relevant JobId from a_bForeground, a_bForeground=false and added to vector m_pvJobs' tail

JobsList.addJob(string a_strCommand, int a_nPid,int a_nstatusJobs, int a_nJobId=-1, bool a_bForeground = false)
{
	if (a_bForeground==true) // this option used only during fg command
	{		
		JobsList.m_pForeground {.JobId=a_nJobId,.PID=a_nPid,.sJobsCommand=a_strCommand,.statusJobs=2,.time_started=0};
	}
	else
	{
		if(a_nJobId>0)// back from foreground to list
		{
			JobsList->m_pvJobs.push_back(JobsList.m_pForeground);
			int last = JobsList->m_pvJobs.size() -1;
			JobsList->m_pvJobs[last].statusJobs=a_nstatusJobs;
			JobsList->m_pvJobs[last].time_started=time(NULL);
			memset(&m_pForeground, 0, sizeof(m_pForeground));//desinfect
		}
		else // first time added to list
		{
			JobEntry a_sJEtemp {.JobId=JobsList.maxJobId+1,.PID=a_nPid,.sJobsCommand=a_strCommand,.statusJobs=,.time_started=time(NULL)};
			JobList->m_pvJobs.push_back(a_sJEtemp);
			JobsList.maxJobId++;
		}
	}
}

JobsList.applyToAll(void (*a_pfun)(int))
{
	for(int i=0;i<JobsList->m_pvJobs.size();i++)
	{
		a_pfun(i);
	}
}

void removeJobByPlace(int a_viJobs)
{
	
}


struct JobEntry {
   // TODO: Add your data members
   int JobId;
   pid_t PID;
   string sJobsCommand;
   int statusJobs; // 0-stopped,1-backgrounded,2-foregrounded
   int time_started;
    };






int main () {
	

getchar ();
return 0;
}
