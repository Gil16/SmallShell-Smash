#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <sys/types.h>
#include <iomanip>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h> 

#include "Commands.h"
#include "Utilities.h"

using namespace std;

#define BUFFSIZE 8192

JobEntry *SmallShell::m_pForeground;


SmallShell::SmallShell()
{
	m_pJobsList = new JobsList();
	m_pForeground = nullptr;
	smash_pid = getpid();
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


// Creates and returns a pointer to Command class which matches the given command line (cmd_line)
Command* SmallShell::CreateCommand(const char* cmd_line) {
	string cmd_s = string(cmd_line);
	vector<string> args;
	split(cmd_s, args);
	
	size_t pos_pipe_com = cmd_s.find_first_of("|");
	m_pJobsList->removeFinishedJobs();
	for(uint j=0 ; j < m_pJobsList->m_pvJobs.size() ; j++)
	{
			m_pJobsList->updateJobstatusByPlace(j);
	}
	
	bool is_bg_cmd = _isBackgroundComamnd(cmd_line);
	char *no_us_cmd_line = new char[strlen(cmd_line)+1];
	strcpy(no_us_cmd_line, cmd_line);	
	if(is_bg_cmd)
	{
		_removeBackgroundSign(no_us_cmd_line);
	}
	vector<string> no_us_args;
	split(no_us_cmd_line, no_us_args);
	
    if (args.size() == 0)
    {
        return nullptr;
    }
    if(pos_pipe_com != string::npos)
    {
		return new PipeCommand(cmd_line);
	}
    if(isRedirectionCommand(cmd_s))
    {
		return new RedirectionCommand(cmd_line);
	}
	
	if (no_us_args[0].compare("chprompt") == 0)
    {
		return new ChpromptCommand(no_us_cmd_line);
	}
	else if (no_us_args[0].compare("showpid") == 0)
    {
		return new ShowPidCommand(no_us_cmd_line);
	}
	else if (no_us_args[0].compare("pwd") == 0) 
    {
		return new GetCurrDirCommand(no_us_cmd_line);
    }
	else if (no_us_args[0].compare("cd") == 0)
    {
		return new ChangeDirCommand(no_us_cmd_line);
	}
	else if(no_us_args[0].compare("jobs") == 0)
    {   
		return new JobsCommand(no_us_cmd_line);
    }
	else if(no_us_args[0].compare("kill") == 0)
    {
		return new KillCommand(no_us_cmd_line);
	}
	else if(no_us_args[0].compare("ls") == 0)
    {
		return new LsCommand(no_us_cmd_line);
	}
	else if(args[0].compare("fg") == 0)
    {
		return new ForegroundCommand(cmd_line);
	}
	else if(args[0].compare("bg") == 0)
    {
		return new BackgroundCommand(cmd_line);
	}
	else if(args[0].compare("quit") == 0)
    {
		return new QuitCommand(cmd_line);
	}
	else if(args[0].compare("cp") == 0)
	{
		if(args.size() >= 3)
			return new CopyCommand(cmd_line, args[1], args[2]);
		else 
			cerr << "smash error: cp: invalid arguments" << endl;
	}
	else if(args[0].compare("timeout") == 0)
    {
		return new TimeoutCommand(cmd_line);
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
	split(c_cmd_line, args);
	SmallShell& smash = SmallShell::getInstance();
	if (args.size() > 1)
	{
       string cursor_Prompt = args[1];
       cursor_Prompt.push_back('>');
       cursor_Prompt.push_back(' ');
	   smash.SetPrompt(cursor_Prompt);
    }
    else
    {
		smash.SetPrompt("smash> ");
	}
}

void ShowPidCommand::execute()
{
	SmallShell& smash = SmallShell::getInstance();
	cout << "smash pid is " << smash.smash_pid << endl;
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
	vector<string> args;
	split (c_cmd_line, args);	
	SmallShell& smash = SmallShell::getInstance();
	if(args.size() == 1)
	{
		cout << smash.GetPrompt(); 
	}
	else if(args.size() == 2)
	{
		char* ca_CurrDir;
	    ca_CurrDir = get_current_dir_name();
	    if(args[1] == "-")
	    {
			string sdestDir = smash.GetLWD();
			if(sdestDir.size() == 0) 
			{ 
				cout << "smash error: cd: OLDPWD not set" << endl; 
			}
			else
			{
				chdir(sdestDir.c_str());
				smash.SetLWD(string(ca_CurrDir));
			}
		}
		else if(args[1] == "..")
		{
			string tempstr = string(ca_CurrDir);
			size_t nLastSlash = tempstr.find_last_of("/");
			string sdestDir = tempstr.substr(0, nLastSlash);
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
				int res = chdir(tempstr.c_str());
				if(res == -1)
				{
					perror("smash error: chdir failed");
				} 
				else
				{
					smash.SetLWD(string(ca_CurrDir));
				}
			}
			//relative path
			else
			{
				string tempstr1 = args[1].insert(0, "/");
				string tempstr2 = tempstr1.insert(0, ca_CurrDir);
				int res = chdir(tempstr2.c_str());
				if(res == -1)
				{
					perror("smash error: chdir failed");
				}
				else
				{
					smash.SetLWD(string(ca_CurrDir));
				}
			}
		}
		
	}
    else if(args.size() > 2)
    {
		cout << "smash error: cd: too many arguments" << endl;
	}
}

void JobsCommand::execute()
{
	SmallShell& smash = SmallShell::getInstance();
	JobsList* jobs_list = smash.GetJobList();
	jobs_list->printJobsList();
}

void KillCommand::execute()
{
	vector<string> args;
	split (c_cmd_line, args);	
	SmallShell& smash = SmallShell::getInstance();
	JobsList* jobs_list = smash.GetJobList();
	if(args.size() != 3 || args[1].substr(0,1) != "-")
	{
		cout << "smash error: kill: invalid arguments" << endl;
		return;
	}
	string sigStr = args[1].substr(1, args[1].size());
	if(!checkIfStrIsNum(sigStr) || !checkIfStrIsNum(args[2]))
	{
		cout << "smash error: kill: invalid arguments" << endl;
		return;
	}
	
	uint jobSeqNum = stoi(args[2]);
	uint sigNum = stoi(sigStr);
	JobEntry* job = jobs_list->getJobById(jobSeqNum);
	if(!job)
	{
		cout << "smash error: kill: job-id " << args[2] << " does not exist" << endl;
		return;
	}
	if(sigNum < 0 || sigNum > 31 || jobSeqNum < 1)
	{
		cout << "smash error: kill failed: Invalid argument" << endl;
		return;
	}
	kill(job->PID, sigNum);
	if(sigNum == 9)
    {
		jobs_list->removeJobById(jobSeqNum);
	}
	cout << "signal number " << sigStr << " was sent to pid " << job->PID << endl;
}

void LsCommand::execute()
{
	struct dirent **namelist;
	int i,n;

    n = scandir(".", &namelist, 0, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        for (i = 0; i < n; i++) {
            printf("%s\n", namelist[i]->d_name);
            free(namelist[i]);
            }
        }
    free(namelist);  
}

void ForegroundCommand::execute()
{
	vector<string> s_cmd;
	split(c_cmd_line, s_cmd);
	SmallShell& smash = SmallShell::getInstance();
	JobsList* jobs_list = smash.GetJobList();
	jobs_list->removeFinishedJobs();
	if(s_cmd.size() == 1)
	{
	    if(jobs_list->m_pvJobs.size() == 0)
	    {
		    cout << "smash error: fg: jobs list is empty" << endl;
		    return;
	    }
		JobEntry* ptempJE = jobs_list->getJobById (jobs_list->getLastJobId());
	    SmallShell::m_pForeground = new JobEntry(ptempJE->nId, ptempJE->PID, ptempJE->sCommand, eJobStatus_Foreground, ptempJE->time_started);
	    jobs_list->removeJobByPID(ptempJE->PID);
	    
	    cout << smash.m_pForeground->sCommand << " : " << smash.m_pForeground->PID << endl;
	    int resKill = kill(ptempJE->PID,SIGCONT);
		if(resKill == -1)
		{
			perror("smash error: kill failed");
		}
	    
	    int status;
	    waitpid(ptempJE->PID, &status, WUNTRACED);
	    smash.m_pForeground = nullptr;
	    return;
	}
	else if (s_cmd.size() == 2)
	{
	    try
	    {
		    stoi(s_cmd[1]);
		}
		catch(const std::invalid_argument& ia)
		{
			cout << "smash error: fg: invalid arguments" << endl;
			return;
		}
		
		JobEntry* ptempJE = jobs_list->getJobById(stoi(s_cmd[1]));
		if(jobs_list->m_pvJobs.size() < 1 || ptempJE == nullptr)   //empty list or no job with jobid
	    {
		    cout << "smash error: fg: job-id " << s_cmd[1] << " does not exist" << endl;
		    return;
	    }
	    else 
	    {
	        cout << ptempJE->sCommand << " : " << ptempJE->PID << endl;
			int resKill = kill(ptempJE->PID,SIGCONT);
			if(resKill == -1)
			{
				perror("smash error: kill failed");
			}
			
			jobs_list->removeJobByPID(ptempJE->PID);
	        SmallShell::m_pForeground = ptempJE;
	        
	        int status;
	        waitpid(ptempJE->PID, &status, WUNTRACED);
	        SmallShell::m_pForeground = nullptr;
	        return;
		}
	}
	else
	{
		cout << "smash error: fg: invalid arguments" << endl;
		return;
	}
}

void BackgroundCommand::execute()
{
	vector<string> s_cmd;
	split(c_cmd_line, s_cmd);
	SmallShell& smash = SmallShell::getInstance();
	JobsList* jobs_list = smash.GetJobList();
	jobs_list->removeFinishedJobs();
	if(s_cmd.size() == 1)
	{
		JobEntry* ptempJE = jobs_list->getLastStoppedJob();
		if(ptempJE != nullptr){
			cout << ptempJE->sCommand << " : " << ptempJE->PID << endl;
	        int resKill = kill(ptempJE->PID,SIGCONT);
	        if(resKill == -1)
			{
				perror("smash error: kill failed");
			}
	        
	        ptempJE->status = eJobStatus_Background;
	        jobs_list->LastStopped = -1;
	        return;
		}
		else
		{
			cout << "smash error: bg: there is no stopped jobs to resume" << endl;
		    return;
		}
	}
	else if(s_cmd.size() == 2)
	{
		try
		{
			stoi(s_cmd[1]);
		}
		catch(const std::invalid_argument& ia)
		{
			cout << "smash error: bg: invalid arguments" << endl;
			return;
		}
		
	    JobEntry* ptempJE = jobs_list->getJobById(stoi(s_cmd[1]));
	    if(jobs_list->m_pvJobs.size() < 1 || ptempJE == nullptr)   //empty list or no job with jobid
	    {
		    cout << "smash error: bg: job-id " << s_cmd[1] << " does not exist" << endl;
	    }
	    else if( ptempJE->status != eJobStatus_Stopped)
	    {
		    cout << "smash error: bg: job-id " << s_cmd[1] << " is already running in the background" << endl;
	    }
	    else
	    {
	        cout << ptempJE->sCommand << " : " << ptempJE->PID << endl;
	        ptempJE->status = eJobStatus_Background;
	        jobs_list->LastStopped = -1;
	        int resKill = kill(ptempJE->PID,SIGCONT);
	        if(resKill == -1)
			{
				perror("smash error: kill failed");
			}
	        return;
	    }
    }
    else
    {
		cout << "smash error: bg: invalid arguments" << endl;
		return;
	}
}

void ExternalCommand::execute()
{
	vector<string> s_cmd;
	split(c_cmd_line, s_cmd);
	SmallShell& smash = SmallShell::getInstance();
	JobsList* jobs_list = smash.GetJobList();
	char* temp_cmd = new char[strlen(c_cmd_line)+1];
	strcpy(temp_cmd, c_cmd_line);	
	string last_string = s_cmd.back();
	char last_letter = last_string.back();
	if(last_letter == '&'){
		pid_t pid = fork();
		if (pid == 0) {
			setpgrp();
            temp_cmd[strlen(temp_cmd)-1] = '\0';
			char* args[] = {(char*)"/bin/bash", (char*)"-c", temp_cmd, NULL};
			if(execv(args[0], args) == -1)
			{
				perror("smash error: execv failed");
			}
			exit(0);
		} 
		else {
			jobs_list->addJob(temp_cmd, pid, eJobStatus_Background);
		}
	} 
	else 
	{
		pid_t pid = fork();
		if (pid == 0) {
			setpgrp();
			char* args[] = {(char*)"/bin/bash", (char*)"-c", temp_cmd, NULL};
			if(execv(args[0], args) == -1)
			{
				perror("smash error: execv failed");
			}
			exit(0);
		} else {
			jobs_list->addJobToForeground(temp_cmd, pid);
			int status;
			waitpid(pid, &status, WUNTRACED);
			smash.m_pForeground = nullptr;
		}
	}
	delete[] temp_cmd;
}

void JobsList::printJobBeforeQuit(JobEntry* job)
{
	cout << job->PID << ": " << job->sCommand << endl;
}

void JobsList::killAllJobs()
{
	cout << "smash: sending SIGKILL signal to " << m_pvJobs.size() << " jobs:" << endl;
	for(uint i=0 ; i < m_pvJobs.size() ; i++)
	{
		kill(m_pvJobs.at(i)->PID, SIGKILL);
		printJobBeforeQuit(m_pvJobs.at(i));
	}
}

void QuitCommand::execute()
{
	vector<string> s_cmd;
	split(c_cmd_line, s_cmd);
	SmallShell& smash = SmallShell::getInstance();
	JobsList* jobs_list = smash.GetJobList();
	
	if(s_cmd.size() > 1 && s_cmd[1] == "kill")
	{
		jobs_list->removeFinishedJobs();
		jobs_list->killAllJobs();  // kill all unfinished jobs
	}
	exit(0);
}

void CopyCommand::execute()
{
	int fd_old = open(old_file.c_str(), O_RDONLY);
    int fd_new = open(new_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd_old == -1 || fd_new == -1)
    {
		perror("smash error: open failed");
		return;
	}
	char buf[BUFFSIZE];
	pid_t pid = fork();
	if (pid == 0) 
	{
		setpgrp();
		while(true)
		{
			ssize_t c_r_count = read(fd_old, &buf, BUFFSIZE);
			if(c_r_count == 0) // EOF, finished reading
			{
				break;
			}
			else if(c_r_count == -1)
			{
				perror("smash error: read failed");
			}
			
			ssize_t c_w_count = write(fd_new, &buf, c_r_count);
			if(c_w_count == -1)
			{
				perror("smash error: write failed");
			}
		}
		
		int c_fd_old = close(fd_old);
		int c_fd_new = close(fd_new);
		if(c_fd_old == -1 || c_fd_new == -1)
		{
			perror("smash error: close failed");
			return;
		}
		cout << "smash: " << old_file << " was copied to " << new_file << endl;
		exit(0);
	}
	int wstatus;
	waitpid(pid, &wstatus, WUNTRACED);
}

int redirectOrAppend(vector<string> vcmd)
{
	if(vcmd[vcmd.size()-2] == ">")
		return 0;
	else if(vcmd[vcmd.size()-2] == ">>")
		return 1;
	else 
		return -1;
}

void RedirectionCommand::execute()
{
	vector<string> sec_com;
	SmallShell& smash = SmallShell::getInstance();

	char *temp_cmd = new char[strlen(c_cmd_line)+1];
	strcpy(temp_cmd, c_cmd_line);	
	
	string s_temp_cmd ( c_cmd_line );
	size_t pos_sec_com = s_temp_cmd.find_first_of(">");
	string s_tempcom1 = s_temp_cmd.substr(0, pos_sec_com);
	string s_tempcom2 = s_temp_cmd.substr(pos_sec_com+1, s_temp_cmd.size()+1);
	int resROA = 0;  // case of redirection
	
	if(s_tempcom2.find_first_of(">") == 0)
    {
		s_tempcom2.erase(0,1);
		resROA = 1;  // case of append
	}
	split(s_tempcom2.c_str(), sec_com);
	auto cmd = smash.CreateCommand(s_tempcom1.c_str());
	
	int fd_open;
	if(resROA == 0)
	{
		fd_open = open(sec_com[0].c_str(), O_CREAT | O_WRONLY | O_TRUNC , 0644);
	}
	else if(resROA == 1)
	{
		fd_open = open(sec_com[0].c_str(), O_CREAT | O_WRONLY | O_APPEND , 0644);
	}
	if(fd_open == -1) 
	{ 
		perror("smash error: open failed");
		return;
	}
	
	int stdout_copy = dup(1);
	int stderr_copy = dup(2);
	if(stdout_copy == -1 || stderr_copy == -1) 
	{ 
		perror("smash error: dup failed");
	}
	if(dup2(fd_open, 1) == -1)
	{
		perror("smash error: dup2 failed");
	}


	if(close(fd_open) == -1) 
	{ 
		perror("smash error: close failed");
	}

	cmd->execute();
		
	if(dup2(stderr_copy, 2) == -1)
	{
		perror("smash error: dup2 failed");
	}
	if(dup2(stdout_copy, 1) == -1)
	{
		perror("smash error: dup2 failed");
	}
	if(close(stdout_copy) == -1)
	{
		perror("smash error: close failed");
	}
	if(close(stderr_copy) == -1)
	{
		perror("smash error: close failed");
	}
}

void PipeCommand::execute()
{
	vector<string> s_cmd;
	split(c_cmd_line, s_cmd);
	SmallShell& smash = SmallShell::getInstance();
	int pl[2];
	pipe(pl);

	string s_temp_cmd ( c_cmd_line );
	size_t pos_sec_com = s_temp_cmd.find_first_of("|");
	string s_tempcom1 = s_temp_cmd.substr(0, pos_sec_com);
	string s_tempcom2 = s_temp_cmd.substr(pos_sec_com+1, s_temp_cmd.size()+1);
	
	auto cmd1 = smash.CreateCommand(s_tempcom1.c_str());
	auto cmd2 = smash.CreateCommand(s_tempcom2.c_str());
	
	int isPipeSign = 0;
	if(s_tempcom2.find_first_of("&") == 0 ) {
		s_tempcom2.erase(0,1);
		isPipeSign = 1;
	}

	if(isPipeSign == 0)
	{
		int pid = fork();
		if(pid == 0)	// son
		{
			setpgrp();
			if(close(pl[1]) == -1)
			{
				perror("smash error: close failed");
			}
			int stdin_copy = dup(0);
			if(stdin_copy == -1)
			{
				perror("smash error: dup failed");
			}
			if(dup2(pl[0], 0) == -1) 
			{
				perror("smash error: dup2 failed");
			}
			cmd2->execute();	 
			if(close(pl[0]) == -1) 
			{
				perror("smash error: close failed");
			}
			if(close(stdin_copy) == -1) 
			{
				perror("smash error: close failed");
			}
			exit(0);
		}
		else // parent
		{
			if(close(pl[0]) == -1) 
			{
				perror("smash error: close failed");
			}
			int stdout_copy = dup(1); 
			if(stdout_copy == 1)
			{
				perror("smash error: dup failed");
			}
			if(dup2(pl[1], 1) == -1) 
			{
				perror("smash error: dup2 failed");
			}
			if(close(pl[1]) == -1) 
			{
				perror("smash error: close failed");
			}
			cmd1->execute();  
			if(dup2(stdout_copy, 1) == -1) 
			{
				perror("smash error: dup2 failed");
			}
			if(close(stdout_copy) == -1) 
			{
				perror("smash error: close failed");
			}
			int status;
			waitpid(pid, &status, WUNTRACED);
		}
	}
	else if(isPipeSign == 1)
	{
		int pid = fork();
		if(pid == 0)	// son
		{
			setpgrp();
			if(close(pl[1]) == -1) 
			{
				perror("smash error: close failed");
			}
			int stdin_copy = dup(0); 
			if(stdin_copy == -1) 
			{
				perror("smash error: dup failed");
			}
			if(dup2(pl[0], 0) == -1) 
			{
				perror("smash error: dup2 failed");
			}
			cmd2->execute();	 
			if(close(pl[0]) == -1) 
			{
				perror("smash error: close failed");
			}
			if(close(stdin_copy) == -1)
			{
				perror("smash error: close failed");
			}
			exit(0);
		}
		else // parent
		{
			if(close(pl[0]) == -1)
			{
				perror("smash error: close failed");
			}
			int stderr_copy = dup(2);
			if(stderr_copy == -1)
			{
				perror("smash error: dup failed");
			}
			if(dup2(pl[1], 2) == -1)
			{
				perror("smash error: dup2 failed");
			}
			if(close(pl[1]) == -1) 
			{
				perror("smash error: close failed");
			}
			cmd1->execute();
			if(dup2(stderr_copy, 2) == -1)
			{
				perror("smash error: dup2 failed");
			}
			if(close(stderr_copy) == -1)
			{
				perror("smash error: close failed");
			}
			int status;
			waitpid(pid, &status, WUNTRACED);
		}
	}
}

void TimeoutCommand::execute()
{
	vector<string> args;
	split (c_cmd_line, args);	
	//SmallShell& smash = SmallShell::getInstance();
	//JobsList* jobs_list = smash.GetJobList();
	// ????
}


int JobsList::getLastJobId()
{
	int max = 0;
    for (uint i=0 ; i < m_pvJobs.size() ; i++) 
    {
		JobEntry* job = m_pvJobs.at(i);
        if (job->nId > max)
        {
			max = job->nId;
        }
    }
    return max;
}

void JobsList::addJob(string a_strCommand, int a_nPid, EJobStatus a_status)
{
	removeFinishedJobs();
	int njobId;
	if(m_pvJobs.size() == 0) 
	{
		njobId = 1;
	}
	else
	{
		njobId = (m_pvJobs.back()->nId) + 1;
	}
	JobEntry* a_sJEtemp = new JobEntry(njobId, a_nPid, a_strCommand, a_status, time(NULL));
	m_pvJobs.push_back(a_sJEtemp);
}

void JobsList::addJob(JobEntry* job)
{
	removeFinishedJobs();
	if(job->nId != -1)	// was on the job list
	{
		m_pvJobs.push_back(job);
	}
	else  // first time on the job list
	{
		job->nId = getLastJobId() + 1;
		m_pvJobs.push_back(job);
	}
}

void JobsList::addJobToForeground(string a_strCommand, int a_nPid) // sets jobId to -1 since it was never on joblist
{
	SmallShell::m_pForeground = new JobEntry(-1, a_nPid, a_strCommand, eJobStatus_Foreground, time(NULL), time(NULL));
}

void JobsList::applyToAll(void (*a_pfun)(int))
{
	for(uint i=0 ; i < m_pvJobs.size() ; i++)
	{
        if(m_pvJobs.at(i)->status != eJobStatus_Foreground)
        {
			a_pfun(i);
        }
	}
}

void JobsList::printJobsList()
{
	
	if(m_pvJobs.size() != 0)
	{
		removeFinishedJobs();
		for(uint j=0 ; j < m_pvJobs.size() ; j++)
		{
			updateJobstatusByPlace(j);
			printJobByPlace(j);
		}
   }
}

void JobsList::printJobByPlace(int a_viJobs)
{
	cout << "[" << m_pvJobs.at(a_viJobs)->nId << "] ";
	cout << m_pvJobs.at(a_viJobs)->sCommand;
	cout << " : " << m_pvJobs.at(a_viJobs)->PID << " ";
	
	time_t tactual_time = time(NULL);
	auto elapsed_time = difftime(m_pvJobs.at(a_viJobs)->is_stopped ? m_pvJobs.at(a_viJobs)->time_ended : tactual_time, m_pvJobs.at(a_viJobs)->time_started);
	cout << elapsed_time << " secs";
	
	if(m_pvJobs.at(a_viJobs)->status == eJobStatus_Stopped)
	{
		cout << " (stopped)" << endl;
	}
	else if(m_pvJobs.at(a_viJobs)->status == eJobStatus_Background)
	{
		cout << endl;
	}
}

void JobsList::updateJobstatusByPlace(int a_viJobs)
{
	int wstatus;
	pid_t w = waitpid(m_pvJobs.at(a_viJobs)->PID, &wstatus, WNOHANG | WUNTRACED);
    if(w == -1) 
    {
         perror("smash error: waitpid failed");
    }
    else if(w > 0)
    {
		if(WIFEXITED(wstatus))
		{
			m_pvJobs.at(a_viJobs)->status = eJobStatus_Finished;
		} 
		else if(WIFSIGNALED(wstatus))
		{
			m_pvJobs.at(a_viJobs)->status = eJobStatus_Finished;
		} 
		else if(WIFSTOPPED(wstatus))
		{
			m_pvJobs.at(a_viJobs)->status = eJobStatus_Stopped;
		} 
		else if(WIFCONTINUED(wstatus))
		{
			m_pvJobs.at(a_viJobs)->status = eJobStatus_Background;
		} 
    }
}

void JobsList::updateJobstatusByPID(EJobStatus a_nstatus, pid_t a_nPID)
{
	for(uint i=0 ; i < m_pvJobs.size() ; i++)
	{
	    if(m_pvJobs.at(i)->PID == a_nPID)
	    {
	         m_pvJobs.at(i)->status = a_nstatus;
             return;
	    }
    }
}

bool JobsList::removeJobByPlace(int a_viJobs)
{
	if(m_pvJobs.at(a_viJobs)->status == eJobStatus_Finished)
	{
		m_pvJobs.erase(m_pvJobs.begin() + a_viJobs);
		return true;
	}
	else
	{
		return false;
	}
}

void JobsList::removeFinishedJobs()
{
	if(m_pvJobs.size() > 0)
	{ 
		for(uint i=0 ; i < m_pvJobs.size() ; i++)
	    {
			if(removeJobByPlace(i))
			{
				i--;
			}
	    }
    }
}

JobEntry* JobsList::getJobById(int a_jobId)
{
	int i = getIndexById(a_jobId);
    if(i >= 0)
    {
		return m_pvJobs.at(i);
	}
	return NULL;
}

JobEntry* JobsList::getJobByPID(int pid)
{
	for(uint i=0 ; i < m_pvJobs.size() ; i++)
	{
		if(m_pvJobs.at(i)->PID == pid)
		{
			return m_pvJobs.at(i);
		}
	}
	return NULL;
}

void JobsList::removeJobById(int a_jobId)
{
    int i = getIndexById(a_jobId);
    if(i >= 0)
    {
		m_pvJobs.erase(m_pvJobs.begin() + i);
		return;
	}
}

void JobsList::removeJobByPID(int pid)
{
	for(uint i=0 ; i < m_pvJobs.size() ; i++)
	{
		if(m_pvJobs.at(i)->PID == pid)
		{
			m_pvJobs.erase(m_pvJobs.begin() + i);
			return;
		}
	}
}

int JobsList::getIndexById(int a_jobId)
{
	for (uint i=0 ; i < m_pvJobs.size() ; i++)
	{
	    if(m_pvJobs.operator[](i)->nId == a_jobId)
		{
				return i;
		}
	}
    return -1;
}

JobEntry* JobsList::getLastJob()
{
	if(m_pvJobs.size() != 0)
	{
		return m_pvJobs.at(m_pvJobs.size() - 1);
    }
    return NULL;
}

JobEntry* JobsList::getForegroundJob()
{
	if(m_pvJobs.size() == 0)
	{
		return NULL;
    }
	for (uint i=0 ; i < m_pvJobs.size() ; i++)
	{
		if(m_pvJobs.at(i)->status == eJobStatus_Foreground) 
		{
			return m_pvJobs.at(i);
		}
	}
	return NULL;
}

JobEntry* JobsList::getLastStoppedJob()
{
	auto i = m_pvJobs.end();
    while (i != m_pvJobs.begin()) {
		--i;

    if ((*i)->status == eJobStatus_Stopped) {
			return *i;
		}
    }
    return nullptr;
}

// -----here functions of JobsList end-------


void SmallShell::executeCommand(const char *cmd_line) {
	Command* cmd = CreateCommand(cmd_line);
	if(cmd != nullptr) 
	{
		cmd->execute();
		delete cmd;
	}
}
