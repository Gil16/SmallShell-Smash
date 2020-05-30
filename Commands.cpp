#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <sys/types.h>
#include <iomanip>
#include "Commands.h"
#include <sys/stat.h>
#include <fcntl.h>


using namespace std;

JobEntry *SmallShell::m_pForeground;

const std::string WHITESPACE = " \n\r\t\f\v";

#define BUFFSIZE 8192

/////////////////////

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
	m_pForeground = nullptr;
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
 

static bool isRedirectionCommand(const string str) {
    if (str.find('>') == string::npos)	// end of string
    {
        return false;
    }
    else
    {
        return (str[str.find('>')] == '>');
	}
}

int isPipeSign(vector<string> vcmd)
{
	for(uint i=0 ; i < vcmd.size() ; i++)
	{
		if(vcmd[i].compare("|") == 0)
		{
			return 0;
		}
		else if(vcmd[i].compare("|&") == 0)
		{
			return 1;
		}
	}
	return -1;
}

/*
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command* SmallShell::CreateCommand(const char* cmd_line) {
	string cmd_s = string(cmd_line);
	vector<string> args;
	split(cmd_s, args);
    if (args.size() == 0)
    {
        //error handling here
        return nullptr;
    }
    if(isPipeSign(args) == 0 || isPipeSign(args) == 1)
    {
		return new PipeCommand(cmd_line);
	}
    if(isRedirectionCommand(cmd_s))
    {
		return new RedirectionCommand(cmd_line);
	}
	if (args[0].compare("chprompt") == 0)	// bug!! chprompt& wont work and its fine, should ignore &
    {
		return new ChpromptCommand(cmd_line);
	}
	else if (args[0].compare("showpid") == 0)
    {
		return new ShowPidCommand(cmd_line);
	}
	else if (args[0].compare("pwd") == 0) 
    {
		return new GetCurrDirCommand(cmd_line);
    }
	else if (args[0].compare("cd") == 0)
    {
		return new ChangeDirCommand(cmd_line);
	}
//starting here still unimplemented commands, just for skeleton use
//jobs in process- in lab.cpp edited before beeing put here and into header	
	else if(args[0].compare("jobs") == 0)  //potential bug
    {   
		return new JobsCommand(cmd_line);
    }
	else if(args[0].compare("kill") == 0)
    {
		return new KillCommand(cmd_line);
	}
	else if(args[0].compare("fg") == 0)	// needs to be changed
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
		{
			return new CopyCommand(cmd_line, args[1], args[2]);
		}
		else 
		{
			cerr << "smash error: cp: invalid arguments" << endl;
		}
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
       string cursor_Prompt = args[1];
       cursor_Prompt.push_back('>');
	   smash.SetPrompt(cursor_Prompt);
    }
    else
    {
		smash.SetPrompt("smash> ");
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
	vector<string> args;
	split (c_cmd_line, args);	
	SmallShell& smash = SmallShell::getInstance();
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
		JobEntry* job = jobs_list->getJobById(jobSeqNum);
		jobs_list->removeJobById(jobSeqNum);
		{
			cerr << "smash error: kill: job-id " << jobSeqNum << " does not exist" << endl;
			return;
		}
		kill(job->PID, sigNum);
		cout << "signal number " << sigNum << " was sent to pid " << job->PID << endl;
	}
}

void ForegroundCommand::execute(){
	vector<string> s_cmd;
	split(c_cmd_line, s_cmd);
	SmallShell& smash = SmallShell::getInstance();
	JobsList* jobs_list = smash.GetJobList();
	jobs_list->removeFinishedJobs();
	if(s_cmd.size() == 1)
	{
	    if(jobs_list->m_pvJobs->size() == 0)
	    {
		    cout << "smash error: fg: jobs list is empty" << endl;
		    return;
	    }
	    JobEntry* ptempJE = jobs_list->getJobById (jobs_list->getLastJobId());
	    SmallShell::m_pForeground = new JobEntry(ptempJE->nId, ptempJE->PID, ptempJE->sCommand, eJobStatus_Foreground, ptempJE->time_started);
	    jobs_list->removeJobByPID(ptempJE->PID);
	    cout << smash.m_pForeground->sCommand << ":" << smash.m_pForeground->PID << endl;
	    kill(ptempJE->PID,SIGCONT);
	    int status;
	    waitpid(ptempJE->PID, &status, 0);
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
			cerr << "smash error: fg: invalid arguments" << ia.what() << endl;
			return;
		}
		JobEntry* ptempJE = jobs_list->getJobById(stoi(s_cmd[1]));
		if(jobs_list->m_pvJobs->size() < 1 || ptempJE == nullptr)   //empty list or no job with jobid
	    {
		    cout << "smash error: bg: job-id " << s_cmd[1] << " does not exist" << endl;
		    return;
	    }
	    else 
	    {
			SmallShell::m_pForeground = new JobEntry(ptempJE->nId, ptempJE->PID, ptempJE->sCommand, eJobStatus_Foreground, ptempJE->time_started);
	        jobs_list->removeJobByPID(smash.m_pForeground->PID);
	        cout << smash.m_pForeground->sCommand << ":" << smash.m_pForeground->PID << endl;
	        kill(smash.m_pForeground->PID,SIGCONT);
	        int status;
	        waitpid(ptempJE->PID, &status, 0);
	        return;
		}
	}
	else
	{
		cout << "smash error: bg: invalid arguments" << endl;
		return;
	}
}

void BackgroundCommand::execute(){
	vector<string> s_cmd;
	split(c_cmd_line, s_cmd);
	SmallShell& smash = SmallShell::getInstance();
	JobsList* jobs_list = smash.GetJobList();
	jobs_list->removeFinishedJobs();
	if(s_cmd.size() == 1)
	{
		JobEntry* ptempJE = jobs_list->getLastStoppedJob();
		if(ptempJE != nullptr){
			cout << ptempJE->sCommand << ":" << ptempJE->PID << endl;
	        ptempJE->status = eJobStatus_Background;
	        kill(ptempJE->PID,SIGCONT);
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
			cerr << "smash error: bg: invalid arguments" << ia.what() << endl;
			return;
		}
	    JobEntry* ptempJE = jobs_list->getJobById(stoi(s_cmd[1]));
	    if(jobs_list->m_pvJobs->size() < 1 || ptempJE == nullptr)   //empty list or no job with jobid
	    {
		    cout << "smash error: bg: job-id " << s_cmd[1] << " does not exist" << endl;
	    }
	    else if( ptempJE->status != eJobStatus_Stopped)
	    {
		    cout << "smash error: bg: job-id " << s_cmd[1] << " is already running in the background" << endl;
	    }
	    else
	    {
	        cout << ptempJE->sCommand << ":" << ptempJE->PID << endl;
	        ptempJE->status = eJobStatus_Background;
	        jobs_list->LastStopped = -1;
	        kill(ptempJE->PID,SIGCONT);
	        return;
	    }
    }
    else
    {
		cout << "smash error: bg: invalid arguments" << endl;
		return;
	}
}

void ExternalCommand::execute(){
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
			execv(args[0], args);
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
			execv(args[0], args);
			exit(0);
		} else {
			jobs_list->addJobToForeground(temp_cmd, pid);
			int status;
			waitpid(pid, &status, WUNTRACED);
		}
	}
	delete [] temp_cmd;
}

void JobsList::printJobBeforeQuit(JobEntry job)
{
	cout << job.PID << ": " << job.sCommand << endl;
}

void JobsList::killAllJobs()
{
	cout << "smash: sending SIGKILL signal to " << m_pvJobs->size() << " jobs:" << endl;
	for(uint i=0 ; i < m_pvJobs->size() ; i++)
	{
		kill(m_pvJobs->at(i).PID, SIGKILL);
		printJobBeforeQuit(m_pvJobs->at(i));
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
		// remove finished jobs
		jobs_list->killAllJobs();  // kill all unfinished jobs
	}
	exit(0);
}

void CopyCommand::execute()
{
	int fd_old = open(old_file.c_str(), O_RDONLY);
    int fd_new = open(new_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644); // I want to change it, not the group/others 644
    
    if(fd_old == -1 || fd_new == -1)
    {
		cerr << "smash error: open failed" <<  endl;
		return;
	}
	char buf[BUFFSIZE];
	pid_t pid = fork();	 // should be added to the job list?
	if (pid == 0) 
	{
		setpgrp();
		while(true)
		{
			ssize_t c_r_count = read(fd_old, buf, BUFFSIZE);
			if(c_r_count == 0) // EOF, finished reading
			{
				break;
			}
			else if(c_r_count == -1)
			{
				cerr << "smash error: read failed" << endl;
				return;
			}
			ssize_t c_w_count = write(fd_new, buf, c_r_count);
			if(c_w_count == -1)
			{
				cerr << "smash error: write failed" << endl;
				return;
			}
		}
		int c_fd_old = close(fd_old);
		int c_fd_new = close(fd_new);
		if(c_fd_old == -1 || c_fd_new == -1)
		{
			cerr << "smash error: close failed" << endl;
			return;
		}
		cout << "smash: " << old_file << " was copied to " << new_file << endl;
		cout << "smash> ";
		exit(0);
	} 
}

int redirectOrAppend(vector<string> vcmd)
{
	if( vcmd[vcmd.size()-2] == ">" )
		return 0 ;
	else if( vcmd[vcmd.size()-2] == ">>" )
		return 1;
	else 
		return -1;
}

void RedirectionCommand::execute()
{
	vector<string> s_cmd;
	split(c_cmd_line, s_cmd);
	SmallShell& smash = SmallShell::getInstance();

	char* temp_cmd = new char[strlen(c_cmd_line)+1];
	strcpy(temp_cmd, c_cmd_line);	
	char *token;
	int resROA = redirectOrAppend(s_cmd);
	if(resROA == 0)
	{
		token = strtok(temp_cmd, ">");
	} 
	else if(resROA == 1)
	{
		token = strtok(temp_cmd, ">>");
	} // else err
	auto cmd = smash.CreateCommand(token);
	
	int stdout_copy = dup(1);
	int stderr_copy = dup(2);
	close(1);
	close(2);
	
	int fd_open;
	if(resROA == 0)
	{
		fd_open = open(s_cmd[s_cmd.size()-1].c_str(), O_CREAT | O_WRONLY | O_TRUNC , 0644 ); // (0)(user)(group)(others)
	}
	else if(resROA == 1)
	{
		fd_open = open(s_cmd[s_cmd.size()-1].c_str(), O_CREAT | O_WRONLY | O_APPEND , 0644 );
	}
	if(fd_open == -1) 
	{ 
		cout << "smash error: open failed" << endl;
	}
	cmd->execute();
		
	int fd_close = close(fd_open);
	if(fd_close == -1)
	{
		cout << "smash error: close failed" << endl;
	}
	if(dup2(stdout_copy, 1) == -1)
	{
		cerr << "smash error: dup2 failed" << endl;
	}
	if(dup2(stderr_copy, 2) == -1)
	{
		cerr << "smash error: dup2 failed" << endl;
	}
	if(close(stdout_copy) == -1)
	{
		cerr << "smash error: close failed" << endl;
	}
	if(close(stderr_copy) == -1)
	{
		cerr << "smash error: close failed" << endl;
	}
//	
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
	if(s_tempcom2.find_first_of("&") == 0 ) {
		s_tempcom2.erase(0,1);
	}

	if(isPipeSign(s_cmd) == 0)
	{
		int pid = fork();
		if(pid == 0)	// son
		{
			setpgrp();
			int stdin_copy = dup(0);
			int stdout_copy = dup(1);
			auto cmd1 = smash.CreateCommand(s_tempcom1.c_str());
			if(close(pl[0]) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
			if(dup2(pl[1], 1) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			cmd1->execute();
			if(dup2(stdin_copy, 0) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			if(dup2(stdout_copy, 1) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			if(close(stdin_copy) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
			if(close(stdout_copy) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
		}
		else // parent
		{
			int stdin_copy = dup(0);
			int stdout_copy = dup(1);
			auto cmd2 = smash.CreateCommand(s_tempcom2.c_str());
			if(close(pl[1]) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
			if(dup2(pl[0], 0) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			cmd2->execute();		
			if(dup2(stdin_copy, 0) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			if(dup2(stdout_copy, 1) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			if(close(stdin_copy) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
			if(close(stdout_copy) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
		}
	}
	else if(isPipeSign(s_cmd) == 1)
	{
		int pid = fork();
		if(pid == 0)	// son
		{
			setpgrp();
			int stdin_copy = dup(0);
			int stderr_copy = dup(2);
			auto cmd1 = smash.CreateCommand(s_tempcom1.c_str());
			if(close(pl[0]) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
			if(dup2(pl[1], 2) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			cmd1->execute();
			if(dup2(stdin_copy, 0) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			if(dup2(stderr_copy, 2) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			if(close(stdin_copy) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
			if(close(stderr_copy) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
		}
		else // parent
		{
			int stdin_copy = dup(0);
			int stderr_copy = dup(2);
			auto cmd2 = smash.CreateCommand(s_tempcom2.c_str());
			if(close(pl[1]) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
			if(dup2(pl[0], 0) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			cmd2->execute();		
			if(dup2(stdin_copy, 0) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			if(dup2(stderr_copy, 2) == -1)
			{
				cerr << "smash error: dup2 failed" << endl;
			}
			if(close(stdin_copy) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
			if(close(stderr_copy) == -1)
			{
				cerr << "smash error: close failed" << endl;
			}
		}
	}
}


// a_nJobId=-1 ,a_bForeground=false in case job brought first time to list,and has no Jid assigned
// a_nJobId>0, a_bForeground=true if fg command issued on job and job
// will be removed from vector and added to m_pForeground
// if it was brought second time after fg command will be used 
// relevant JobId from a_bForeground, a_bForeground=false and added to vector m_pvJobs' tail

int JobsList::getLastJobId() {
	int max = 0;
    for (uint i=0 ; i < m_pvJobs->size() ; i++) 
    {
		JobEntry job = m_pvJobs->at(i);
        if (job.nId > max)
        {
			max = job.nId;
        }
    }
    return max;
}

void JobsList::addJob(string a_strCommand, int a_nPid, EJobStatus a_status)
{
	removeFinishedJobs();
	int njobId;
	if(m_pvJobs->size() == 0) 
	{
		njobId = 1;
	}
	else
	{
		njobId = (m_pvJobs->back().nId)+1;
	}
	JobEntry a_sJEtemp {.nId = njobId, .PID = a_nPid, .sCommand = a_strCommand, .status = a_status, .time_started = time(NULL) }; // change to new JobEntry(..) ?
	m_pvJobs->push_back(a_sJEtemp);
	//JobsList.maxJobId++; //buggy
}

void JobsList::addJob(JobEntry job)
{
	removeFinishedJobs();
	if(job.nId != -1)	// was on the job list
	{
		m_pvJobs->push_back(job);
	}
	else  // first time on the job list
	{
		job.nId = getLastJobId() + 1;
		m_pvJobs->push_back(job);
	}
}


void JobsList::addJobToForeground(string a_strCommand, int a_nPid) //sets jobId to -1 since it was never on joblist
{
	SmallShell::m_pForeground = new JobEntry(-1, a_nPid, a_strCommand, eJobStatus_Foreground, time(NULL));
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

void JobsList::printJobsList()  //To add to class list   ///Have no idea what are you trying to do here, just use aplly
{
	
	if(m_pvJobs->size() == 0)
	{
		cout << "List is empty, nothing to print" << endl;
	}
	else
	{	
		removeFinishedJobs();
		for(uint j=0 ; j < m_pvJobs->size() ; j++)
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
	if(m_pvJobs->at(a_viJobs).status == eJobStatus_Stopped)
	{
		cout << " (stopped)" << endl;
	}
	if(m_pvJobs->at(a_viJobs).status == eJobStatus_Background)
	{
		cout << endl;
	}
	else
	{
	//	cout << "something wrong:" << " status is-" << m_pvJobs->at(a_viJobs).status << endl; //debugging
	}
}

void JobsList::updateJobstatusByPlace(int a_viJobs)
{
	int wstatus;
	pid_t w = waitpid(m_pvJobs->at(a_viJobs).PID, &wstatus, WNOHANG);
    if(w == -1) 
    {
         perror("waitpid");
    }
    if(w > 0)
    {
		if(WIFEXITED(wstatus))
		{
		//	printf("exited, status=%d\n", WEXITSTATUS(wstatus));
			m_pvJobs->at(a_viJobs).status = eJobStatus_Finished;
		} else if(WIFSIGNALED(wstatus)) {
		//	printf("killed by signal %d\n", WTERMSIG(wstatus));
			m_pvJobs->at(a_viJobs).status = eJobStatus_Finished;
		} else if(WIFSTOPPED(wstatus)) {
		//	printf("stopped by signal %d\n", WSTOPSIG(wstatus));
			m_pvJobs->at(a_viJobs).status = eJobStatus_Stopped;
		} else if(WIFCONTINUED(wstatus)) {
		//	printf("continued\n");
			m_pvJobs->at(a_viJobs).status = eJobStatus_Background;
		} 
    }
    if (w == 0)
    {
	//	cout<<"Nothing changed for" << w <<endl;
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
		// cout << "Found zombie job in node " << a_viJobs << " and erased!" << endl;
		return true;
	}
	else
	{
		// cout << "Job in node " << a_viJobs << " is alive." << endl;
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

JobEntry* JobsList::getJobById(int a_jobId)
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
	//	cout << "removed " << a_jobId << endl;
//		if(m_pvJobs.size() == 0) 
//           {maxJobId=JobsList.m_pForeground.JobId;} // it's a bug, just don't use maxJobId
	}
}

void JobsList::removeJobByPID(int pid)
{
	for(uint i=0 ; i < m_pvJobs->size() ; i++)
	{
		if(m_pvJobs->at(i).PID == pid)
		{
			m_pvJobs->erase(m_pvJobs->begin()+i);
		}
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

JobEntry* JobsList::getLastJob()
{
	if(m_pvJobs->size() != 0)
	{
		return &(m_pvJobs->at(m_pvJobs->size() - 1));
    }
    return NULL;
}

JobEntry* JobsList::getForegroundJob()
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

JobEntry* JobsList::getLastStoppedJob()// LastStopped needs to be updated by handler
{
	if (LastStopped < 0){
	    return nullptr;
	}
	else{
	    return getJobById(LastStopped);
    }
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

