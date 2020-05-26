#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <sys/types.h>
#include <iomanip>
#include "Commands.h"

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

#define EXEC(path, arg) \
  execvp((path), (arg));
//unused
string _ltrim(const std::string& s)  // 1st part of parse
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}
//unused
string _rtrim(const std::string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
  return _rtrim(_ltrim(s));
}
// used instead of Parse
void split(const string& str, vector<string>& a_vOut)
{
    size_t nBeginWord = str.find_first_not_of(WHITESPACE);
    if( nBeginWord !=std::string::npos)
    {
		while (1) {
		if (nBeginWord == std::string::npos)
			break;
		size_t nEndWord = str.find_first_of(WHITESPACE, nBeginWord);
		if (nEndWord == std::string::npos)
		{
			a_vOut.push_back(str.substr(nBeginWord));
			break;
		}
		else
		{
			a_vOut.push_back(str.substr(nBeginWord, nEndWord-nBeginWord));
		}
		nBeginWord = str.find_first_not_of(WHITESPACE, nEndWord);
		} 
    } 
    else
	{
		a_vOut.push_back("chprompt");
	}
}

//unused by me
int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  istringstream iss(_trim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;
  FUNC_EXIT()
}

//unused by me for now
bool _isBackgroundComamnd(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

//unused by me for now
void _removeBackgroundSign(char* cmd_line) {
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h 

//SmallShell::SmallShell() {
// TODO: add your implementation
//}
//
//SmallShell::~SmallShell() {
// TODO: add your implementation

SmallShell::SmallShell()
{
	m_pJobsList = new JobsList();
    SetPrompt("");
}

void SmallShell::SetPrompt(string a_sPrompt)
{
    m_sPrompt = a_sPrompt.empty() ? mc_sDefPrompt : a_sPrompt;
}
 
void SmallShell::SetLWD(string a_sLastWDir)
{
	m_sLastWDir = a_sLastWDir;
}
 
JobsList* SmallShell::GetJobList()
{
	return m_pJobsList;
}
 
/*
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
	string cmd_s = string(cmd_line);
	vector<string> args;
	split(cmd_s, args);
    if (args.size() == 0)
    {
        //error handling here
        return nullptr;
    }
	if (args[0].compare("chprompt")==0)
	//if no command issued split will issue chprompt command on her own
    {
		return new ChpromptCommand (cmd_line);
	}
	else if (args[0].compare("showpid") == 0)
    {
		return new ShowPidCommand (cmd_line);
	}
	else if (args[0].compare("pwd") == 0) 
    {
		return new GetCurrDirCommand(cmd_line);
    }
	else if (args[0].compare("cd") == 0)
    {
		return new ChangeDirCommand (cmd_line);
	}
//starting here still unimplemented commands, just for skeleton use
//jobs in process- in lab.cpp edited before beeing put here and into header	
	else if(args[0].compare("jobs") == 0)  //potential bug
    {   
		return new JobsCommand (cmd_line);
    }
	else if(args[0].compare("kill") == 0)
    {
		return new KillCommand (cmd_line);
	}
	else if(cmd_s.find("fg") == 0)	// needs to be changed
    {
		return new ForegroundCommand (cmd_line);
	}
	else if(cmd_s.find("bg") == 0)
    {
		return new BackgroundCommand (cmd_line);
	}
	else if(cmd_s.find("quit") == 0)
    {
		return new QuitCommand (cmd_line);
	}
	else 
	{
		return new ExternalCommand(cmd_line);
	}
	return nullptr;
}

void ChpromptCommand::execute()
{
	vector<string> args;
	split (c_cmd_line, args);	
	SmallShell& smash = SmallShell::getInstance();
	if (args.size() > 1)
	{
       string cursor_Prompt=args[1];
       cursor_Prompt.push_back('>');
	   smash.SetPrompt(cursor_Prompt);
    }
    else
    {
		smash.SetPrompt("smash>");
	}
}

void ShowPidCommand::execute()
{
	cout << "smash pid is " << getpid() << endl;
}

void GetCurrDirCommand::execute ()
{
	char* ca_CurrDir;
	ca_CurrDir = get_current_dir_name();
	cout << ca_CurrDir << endl;
	free(ca_CurrDir);
} 

void ChangeDirCommand::execute()
{
	cout << "ChangeDirCommand" << endl;
	vector<string> args;
	split (c_cmd_line, args);	
	getchar();
	SmallShell& smash = SmallShell::getInstance();
	getchar();
	if(args.size()==1)
	{
		 cout << smash.GetPrompt(); 
	}
	else if(args.size()==2)
	{
		char* ca_CurrDir;
	    ca_CurrDir = get_current_dir_name();
	    if(args[1] == "-")
	    {
			string sdestDir = smash.GetLWD();
			if(sdestDir.size() == 0) { cerr << "smash error: cd: OLDPWD not set" << endl; }
			else
			{
				chdir(sdestDir.c_str());
				smash.SetLWD(string(ca_CurrDir));
			}
		}
		else if(args[1]=="..")
		{
			string tempstr = string(ca_CurrDir);
			size_t nLastSlash = tempstr.find_last_of("/");
			string sdestDir = tempstr.substr(0,nLastSlash);
			cout << sdestDir << endl;
			chdir(sdestDir.c_str());
			smash.SetLWD(string(ca_CurrDir));
		}
		else
		{
			size_t nFirstSlash = args[1].find_first_of("/");
			//absolute path
			if(nFirstSlash == 0)
			{
				string tempstr = args[1];
				chdir(tempstr.c_str());
				smash.SetLWD(string(ca_CurrDir));
			}
			//relative path
			else
			{
				string tempstr1 = args[1].insert(0,"/");
				string tempstr2 = tempstr1.insert(0,ca_CurrDir);
				chdir(tempstr2.c_str());
				smash.SetLWD(string(ca_CurrDir));
			}
		}
		
	}
    else if(args.size()>2)
    {
		cerr << "smash error: cd: too many arguments" << endl;
	}
}

static bool checkIfStrIsNum(string str){
	for(uint i=0 ; i <= str.size()-1 ; i++)
	{
		if(!(str[i] >= '0' || str[i] <= '9'))
		{
			return false;
		}
	}
	return true;
}



void JobsCommand::execute()
{
	cout << "JobsCommand" << endl; //debugging
	SmallShell& smash = SmallShell::getInstance();
	JobsList* jobs_list = smash.GetJobList();
	jobs_list->printJobsList();
}



void KillCommand::execute()
{
	cout << "KillCommand" << endl; //debugging
	vector<string> args;
	split (c_cmd_line, args);	
	getchar(); //debugging
	SmallShell& smash = SmallShell::getInstance();
	JobsList* jobs_list = smash.GetJobList();
	if(args.size() != 3 || args[1].substr(0,1) != "-")
	{
		cerr << "smash error: kill: invalid arguments" << endl;
		return;
	}
	string sigStr = args[1].substr(1, args[1].size());
	if(!checkIfStrIsNum(sigStr) || !checkIfStrIsNum(args[2]))
	{
		cerr << "smash error: kill: invalid arguments" << endl;
		return;
	}
	uint jobSeqNum = stoi(args[2]);
	uint sigNum = stoi(sigStr);
	if(sigNum < 0 || sigNum > 31 || jobSeqNum < 1)
	{
		cerr << "smash error: kill: invalid arguments" << endl;
		return;
	}
	else
	{
		JobsList::JobEntry* job = jobs_list->getJobById(jobSeqNum);
		jobs_list->removeJobById(jobSeqNum);
		{
			cerr << "smash error: kill: job-id " << jobSeqNum << " does not exist" << endl;
			return;
		}
		kill(job->PID, sigNum);
		cout << "signal number " << sigNum << " was sent to pid " << job->PID << endl;
	}
}

void ExternalCommand::execute(){
	cout << "ExternalCommand" << endl; //debugging
	
	vector<string> s_cmd;
	split (c_cmd_line, s_cmd);
	SmallShell& smash = SmallShell::getInstance();
	cout<<" after shell smash: "<<s_cmd[0]<<endl;
	JobsList* jobs_list= smash.GetJobList();
	char* temp_cmd=new char[strlen(c_cmd_line)+1];
	cout<<"before strcpy :"<<c_cmd_line<<endl;
	strcpy(temp_cmd, c_cmd_line);	
	string last_string=s_cmd.back();
	char last_letter=last_string.back();
	if(last_letter == '&'){
		pid_t pid = fork();
		if (pid == 0) {
			setpgrp();
			pid_t childPID=getpid();
			printf("if: child pid = %d\n", childPID);
			char* args[] = {(char*)"/bin/bash", (char*)"-c", temp_cmd, NULL}; // args[0] might be just "bash"
			execv(args[0], args);
		} 
		else 
		{
			jobs_list->addJob(s_cmd[0], pid, JobsList::eJobStatus_Background);
			printf("if: parent pid = %d\n", getpid());
		}
	} 
	else 
	{
		pid_t pid = fork();
		if (pid == 0) {
			setpgrp();
			pid_t childPID=getpid();
			printf("else: child pid = %d\n", childPID);
			char* args[] = {(char*)"/bin/bash", (char*)"-c", temp_cmd, 0}; // args[0] might be just "bash"
			execv(args[0], args);
		} else {
			jobs_list->addJobToForeground(s_cmd[0], pid);
			wait(NULL);
			printf("else: parent pid = %d\n", getpid());
		}
	}
	delete [] temp_cmd;
}


// a_nJobId=-1 ,a_bForeground=false in case job brought first time to list,and has no Jid assigned
// a_nJobId>0, a_bForeground=true if fg command issued on job and job
// will be removed from vector and added to m_pForeground
// if it was brought second time after fg command will be used 
// relevant JobId from a_bForeground, a_bForeground=false and added to vector m_pvJobs' tail

void JobsList::addJob(string a_strCommand, int a_nPid, EJobStatus a_status)
{
	int njobId;
	if(m_pvJobs->size() == 0) 
	{
		njobId=1;
		cout<< "adding first job"<<endl;
	}
	else
	{
		njobId = (m_pvJobs->back().nId)+1;
	}
	JobsList::JobEntry a_sJEtemp {.nId = njobId, .PID = a_nPid, .sCommand = a_strCommand, .status = a_status, .time_started = time(NULL) };
	cout<<"before pushing back"<<endl;
	m_pvJobs->push_back(a_sJEtemp);
	cout<<m_pvJobs->at(0).nId<<" my size is:"<<m_pvJobs->size()<<endl;
	//JobsList.maxJobId++; //buggy
}

void JobsList::addJobToForeground(string a_strCommand, int a_nPid) //sets jobId to -1 since it was never on joblist
{
   m_pForeground = {.nId = -1, .PID = a_nPid, .sCommand = a_strCommand, .status = eJobStatus_Foreground, .time_started = time(NULL) };	
}

void JobsList::applyToAll(void (*a_pfun)(int))
{
	for(uint i=0 ; i < m_pvJobs->size() ; i++)
	{
        if(m_pvJobs->at(i).status != eJobStatus_Foreground)
        {
			a_pfun(i);
        }
	}
}

void JobsList::printJobsList ()  //To add to class list   ///Have no idea what are you trying to do here, just use aplly
{
	
	if(m_pvJobs->size()==0)
	{
		cout<<"List is empty, nothing to print"<<endl;
	}
	else
	{	
	   for(uint j=0;j< m_pvJobs->size();j++)
	   {
		  updateJobstatusByPlace(j);
	      printJobByPlace(j);
       }
   }
}


void JobsList::printJobByPlace(int a_viJobs)
{
	cout << "[" << m_pvJobs->at(a_viJobs).nId << "] ";
	cout << m_pvJobs->at(a_viJobs).sCommand;
	cout << ": " << m_pvJobs->at(a_viJobs).PID << " ";
	time_t tactual_time = time(NULL);
	double elapsed_time = difftime(tactual_time,m_pvJobs->at(a_viJobs).time_started);
	cout << elapsed_time << " secs";
	cout<<endl;//debugging
	cout<<m_pvJobs->at(a_viJobs).status<<endl; //debugging
	if(m_pvJobs->at(a_viJobs).status == eJobStatus_Stopped)
	{
		cout<<" (stopped)"<<endl;
	}
	if(m_pvJobs->at(a_viJobs).status == eJobStatus_Background)
	{
		cout << endl;
	}
	else
	{
		cout << "something wrong:" << " status is-" << m_pvJobs->at(a_viJobs).status << endl; //debugging
	}
}

void JobsList::updateJobstatusByPlace(int a_viJobs)
{
	int wstatus;
	cout<<"Entering update function"<<endl;
	pid_t w = waitpid(m_pvJobs->at(a_viJobs).PID, &wstatus, WNOHANG);
    if(w == -1) 
    {
         perror("waitpid");
    }
    if(w > 0)
    {
		if(WIFEXITED(wstatus))
		{
			printf("exited, status=%d\n", WEXITSTATUS(wstatus));  //problem is here
			m_pvJobs->at(a_viJobs).status = eJobStatus_Finished;
		} else if(WIFSIGNALED(wstatus)) {
			printf("killed by signal %d\n", WTERMSIG(wstatus));
			m_pvJobs->at(a_viJobs).status = eJobStatus_Finished;
		} else if(WIFSTOPPED(wstatus)) {
			printf("stopped by signal %d\n", WSTOPSIG(wstatus));
			m_pvJobs->at(a_viJobs).status = eJobStatus_Stopped;
		} else if(WIFCONTINUED(wstatus)) {
			printf("continued\n");
			m_pvJobs->at(a_viJobs).status = eJobStatus_Background;
		} 
    }
    if (w == 0)
    {
		cout<<"Nothing changed for" << w <<endl;
	}
}

void JobsList::updateJobstatusByPID(EJobStatus a_nstatus, pid_t a_nPID)
{
	for(uint i=0 ; i < m_pvJobs->size() ; i++)
	{
	    if(m_pvJobs->at(i).PID == a_nPID)
	        {
	         m_pvJobs->at(i).status = a_nstatus;
             return;
	        }
    }
    cout << " Process to update is not found" << endl; //for debugging
}

bool JobsList::removeJobByPlace(int a_viJobs)
{
	if(m_pvJobs->at(a_viJobs).status == eJobStatus_Finished) // i think you should do poll here to check if the job is alive. Who updates the status?
	{
		m_pvJobs->erase(m_pvJobs->begin()+a_viJobs);
		cout << "Found zombie job in node " << a_viJobs << " and erased!" << endl;
		return true;
	}
	else
	{
		cout << "Job in node " << a_viJobs << " is alive." << endl;
		return false;
	}
}

void JobsList::removeFinishedJobs()
{
	if(m_pvJobs->size() > 0)
	{ 
		for(uint i=0 ; i < m_pvJobs->size() ; i++)
	    {
			if(removeJobByPlace(i)) //expect problems here. when you delete from list its size changes. Google how to do it properly
			{
				i--;
			}
	    }
    }
}

JobsList::JobEntry* JobsList::getJobById(int a_jobId)
{
	int i = getIndexById(a_jobId);
    if (i >= 0)
    {
		return &(m_pvJobs->at(i));
	}
	cout << "Job with JobId" << a_jobId << "is not found" << endl;
	return NULL;
}

void JobsList::removeJobById(int a_jobId)
{
    int i = getIndexById(a_jobId);
    if (i >= 0)
    {
		m_pvJobs->erase(m_pvJobs->begin()+i);
		cout << "removed " << a_jobId << endl;
//		if(m_pvJobs.size() == 0) 
//           {maxJobId=JobsList.m_pForeground.JobId;} // it's a bug, just don't use maxJobId
	}
}

int JobsList::getIndexById(int a_jobId)
{
	for (uint i=0 ; i < m_pvJobs->size() ; i++)
	{
	    if(m_pvJobs->operator[](i).nId == a_jobId)
		   {
				return i;
		   }
	}
    return -1;
}

JobsList::JobEntry* JobsList::getLastJob()
{
	if(m_pvJobs->size() != 0)
	{
		return &(m_pvJobs->at(m_pvJobs->size() - 1));
    }
    return NULL;
}

JobsList::JobEntry* JobsList::getForegroundJob()
{
	if(m_pvJobs->size() == 0)
	{
		return NULL;
    }
	for (uint i=0 ; i < m_pvJobs->size() ; i++)
	{
		if(m_pvJobs->at(i).status == eJobStatus_Foreground)
		return &(m_pvJobs->at(i));
	}
	return NULL;
}

JobsList::JobEntry* JobsList::getLastStoppedJob()// LastStopped needs to be updated by handler
{
	return getJobById(LastStopped);
}
// -----here functions of JobsList end-------



void SmallShell::executeCommand(const char *cmd_line) {
	// TODO: Add your implementation here
	// for example:
	Command* cmd = CreateCommand(cmd_line);
	if(cmd != nullptr) 
	{
		cmd->execute();
		delete cmd;
	}
  // Please note that you must fork smash process for some commands (e.g., external commands....)
}

