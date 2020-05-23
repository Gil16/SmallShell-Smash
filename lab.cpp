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
  pid_t LastStopped;
// TODO: Add your data members
public:
 JobsList()
 {
     m_pvJobs = new vector ... ; // TODO
     memset(&m_pForeground, 0, sizeof(m_pForeground));
 };
 ~JobsList()
 {
     delete m_pvJobs;
 };

 void addJob(string a_strCommand, int a_nPid, int a_nstatusJobs, int a_nJobId=-1, bool a_bForeground = false);
 void applyToAll(void (*a_pfun)(int));
 //--------will add above to smash------
 void removeJobByPlace(int a_viJobs);// goes to node in vector and checks.If finished-removes.
 bool printJobByPlace(int a_viJobs);// goes to node and prints info
 void removeFinishedJobs();
 void updateJobstatusByPlace(int a_viJobs);// goes to node in vector and updates status
 void updateJobstatusById(int a_nstatus,pid_t a_nPID);
 JobEntry * getJobById(int a_jobId);// if job is not in list returns NULL
 void removeJobById(int a_jobId);
 JobEntry * getLastJob();// if no jobs in list 
 JobEntry * getForegroundJob();
 JobEntry *getLastStoppedJob();
 // TODO: Add extra methods or modify exisitng ones as needed
};

// a_nJobId=-1 ,a_bForeground=false in case job brought first time to list,and has no Jid assigned
// a_nJobId>0, a_bForeground=true if fg command issued on job and job
// will be removed from vector and added to m_pForeground
// if it was brought second time after fg command will be used 
// relevant JobId from a_bForeground, a_bForeground=false and added to vector m_pvJobs' tail

void JobsList::addJob(string a_strCommand, int a_nPid,int a_nstatusJobs, int a_nJobId=-1, bool a_bForeground = false)
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

void JobsList::applyToAll(void (*a_pfun)(int))
{
	for(int i=0;i<JobsList->m_pvJobs.size();i++)
	{
		a_pfun(i);
	}
}

void JobsList::updateJobstatusByPlace (int a_viJobs)
{
	
	pid_t w = waitpid(m_pvJobs[a_viJobs].PID, &wstatus, WNOHANG);
    if (w == -1) 
    {
         perror("waitpid");
    }
    if ( w>0 )
    {
       if (WIFEXITED(wstatus))
       {
           printf("exited, status=%d\n", WEXITSTATUS(wstatus));
           JobsList->m_pvJobs[a_viJobs].statusJobs=-1;
       } else if (WIFSIGNALED(wstatus)) {
           printf("killed by signal %d\n", WTERMSIG(wstatus));
           JobsList->m_pvJobs[a_viJobs].statusJobs=-1;
       } else if (WIFSTOPPED(wstatus)) {
           printf("stopped by signal %d\n", WSTOPSIG(wstatus));
           JobsList->m_pvJobs[a_viJobs].statusJobs=0;
       } else if (WIFCONTINUED(wstatus)) {
           printf("continued\n");
           JobsList->m_pvJobs[a_viJobs].statusJobs=1;
       } 
    }
    if (w==0)
    {
		cout<<"Nothing changed for" << w <<endl;
	}
}
void JobsList::updateJobstatusById(int a_nstatus,pid_t a_nPID)
{
	for(int i=0;i<JobsList->m_pvJobs.size();i++)
	{
	    if(JobsList->m_pvJobs[i].PID==a_nPID)
	        {
	         JobsList->m_pvJobs[i].statusJobs=a_nstatus;
	        }
    }
    cout<< " Process to update is not found"<<endl;//for debugging
}
bool JobsList::removeJobByPlace(int a_viJobs)
{
	if(JobsList->m_pvJobs[a_viJobs].statusJobs<0)
	{
		JobsList->m_pvJobs.erase( a_viJobs);
		cout<<"Found zombie job in node "<<a_viJobs<<" and erased!"<<endl;
		if(JobsList->m_pvJobs.size()==0) {maxJobId=JobsList.m_pForeground.JobId;}
		return true;
	}
	else
	{
		cout<<"Job in node "<<a_viJobs<<" is alive."<<endl;
		return false;
	}
}

void JobsList::removeFinishedJobs()
{
	if(JobsList->m_pvJobs.size()>0)
	{ 
	  for (int i=0;i<JobsList->m_pvJobs.size();i++)
	    {
			if(removeJobByPlace(i)) 
			{
				i--;
			}
	    }
    }
    if(JobsList->m_pvJobs.size()==0) 
    {
		maxJobId=JobsList.m_pForeground.JobId;
	}
}

JobEntry* JobsList::getJobById(int a_jobId)
{
	for (int i=0;i<JobsList->m_pvJobs.size();i++)
	{
		if(JobsList->m_pvJobs[i].JobId==a_jobId)
		{
			return &(JobsList->m_pvJobs[i]);
		}
	}
	cout<< "Job with JobId"<<a_jobId<<"is not found"<<endl;
	return NULL;
}

void JobsList::removeJobById(int a_jobId)
{
	for (int i=0;i<JobsList->m_pvJobs.size();i++)
	{
	    if(JobsList->m_pvJobs[i].JobId==a_jobId)
		   {
			   JobsList->m_pvJobs.erase(i);
			   cout<<"removed "<<a_jobId<<endl;
			   if(JobsList->m_pvJobs.size()==0) {maxJobId=JobsList.m_pForeground.JobId;}
			   break;
		   }
	}
}

JobEntry* JobsList::getLastJob()
{
	if(maxJobId>0)
	{
	   return getJobById(maxJobId);
    }
    return NULL;
}

JobEntry* JobsList::getForegroundJob();
{
	return &(JobsList.m_pForeground);
}

JobEntry* JobsList::getLastStoppedJob();
{
	return getJobById(LastStopped);
}

struct JobEntry {
   // TODO: Add your data members
   int JobId;
   pid_t PID;
   string sJobsCommand;
   int statusJobs; // 0-stopped,1-backgrounded,2-foregrounded,-1-finished
   int time_started;
};






int main () {
	

getchar ();
return 0;
}
