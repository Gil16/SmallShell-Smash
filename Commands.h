#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)
#define HISTORY_MAX_RECORDS (50)

class JobsList;


class Command {
 public:
	const char* c_cmd_line;
	Command(const char* cmd_line) { c_cmd_line = cmd_line; }
	virtual ~Command() {}
	virtual void execute() = 0;
};

class BuiltInCommand : public Command {
 public:
	BuiltInCommand(const char* cmd_line) : Command(cmd_line) {}
	virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
 public:
	ExternalCommand(const char* cmd_line) : Command(cmd_line) {}
	~ExternalCommand() {}
	void execute() override;
};

class PipeCommand : public Command {
 public:
	PipeCommand(const char* cmd_line) : Command(cmd_line) {}
	virtual ~PipeCommand() {}
	void execute() override;
};

class RedirectionCommand : public Command {
 public:
	explicit RedirectionCommand(const char* cmd_line) : Command(cmd_line) {}
	virtual ~RedirectionCommand() {}
	void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
  public:
	ChangeDirCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
	virtual ~ChangeDirCommand() {}
	void execute() override ;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
	GetCurrDirCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
	~GetCurrDirCommand() {}
	void execute() override ;
};

class ShowPidCommand : public BuiltInCommand {
 public:
	ShowPidCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
	~ShowPidCommand() {}
	void execute() override ;
};

typedef enum {
        eJobStatus_Stopped,
        eJobStatus_Background,
        eJobStatus_Foreground,
        eJobStatus_Finished
} EJobStatus;


struct JobEntry {
	int nId;
	pid_t PID;
	string sCommand;
	EJobStatus status; // 0-stopped,1-backgrounded,2-foregrounded,3-finished
	time_t time_started;
	   
	JobEntry(int nid, pid_t pid, string scommand, EJobStatus stat, time_t t_started)
	{
		nId = nid;
		PID = pid;
		sCommand = scommand;
		status = stat;
		time_started = t_started;
	}
};


//-------------------------------JobsList Class-----------------------
//--------------------------------------------------------------------

class JobsList {
public: 

	JobsList() : m_pvJobs() {}
	~JobsList() = default;
	void addJob(string a_strCommand, int a_nPid, EJobStatus a_status); // when you add it's always not in foreground
	void addJob(JobEntry* job);
	void addJobToForeground(string a_strCommand, int a_nPid); 
	void applyToAll(void (*a_pfun)(int));

	void printJobsList ();
	bool removeJobByPlace(int a_viJobs);
	void printJobByPlace(int a_viJobs);
	void printJobBeforeQuit(JobEntry* job);
	void removeFinishedJobs();
	void updateJobstatusByPlace(int a_viJobs);
	void updateJobstatusByPID(EJobStatus a_nstatus,pid_t a_nPID);
	JobEntry* getJobById(int a_jobId);
	JobEntry* getJobByPID(int pid);
	void removeJobById(int a_jobId);
	void removeJobByPID(int pid);
	void killAllJobs();
	JobEntry* getLastJob();
	int getLastJobId();
	JobEntry* getForegroundJob();
	JobEntry* getLastStoppedJob();

	vector<JobEntry*> m_pvJobs;
	int LastStopped = -1; // -1 on initialization or when no last stopped
private:
	int getIndexById(int a_jobId); 
};

//-----------------------JobsList Class ends here---------------------
//--------------------------------------------------------------------

class ChpromptCommand : public BuiltInCommand {
 public:
	ChpromptCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
	~ChpromptCommand() {}
	void execute() override;
	
};

class JobsCommand : public BuiltInCommand {
 public:
	JobsCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
	~JobsCommand() {}
	void execute() override;
};

class KillCommand : public BuiltInCommand {
 public:
	KillCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
	~KillCommand() {}
	void execute() override;
};

class ForegroundCommand : public BuiltInCommand {  
 public:
	ForegroundCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
	~ForegroundCommand() {}
	void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
 public:
	BackgroundCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
	~BackgroundCommand() {}
	void execute() override;
};

class QuitCommand : public BuiltInCommand {
 public:
	QuitCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
	~QuitCommand() {}
	void execute() override;
};

class CopyCommand : public BuiltInCommand {
	string old_file;
	string new_file;
 public:
	CopyCommand(const char* cmd_line, string s_file1, string s_file2) : BuiltInCommand(cmd_line)
	{
		old_file = s_file1; 
		new_file = s_file2;
	}
	~CopyCommand() {};
	void execute() override;
};

class LsCommand : public BuiltInCommand {
 public:
	LsCommand(const char* cmd_line) : BuiltInCommand(cmd_line) {}
	~LsCommand() {}
	void execute() override;
};



class SmallShell {
 private:
	string m_sPrompt = "smash> ";
	const string mc_sDefPrompt = "smash> ";
	string m_sLastWDir;
	
	SmallShell();
 public:
	static JobEntry *m_pForeground;
	JobsList *m_pJobsList;
	pid_t smash_pid;
	
	
	Command *CreateCommand(const char* cmd_line);
	SmallShell(SmallShell const&)      = delete; // disable copy ctor
	void operator=(SmallShell const&)  = delete; // disable = operator
	~SmallShell() { delete m_pJobsList; }
	static SmallShell& getInstance() // SmallShell singleton
	{
		static SmallShell instance;
		return instance;
	}
	
	void SetPrompt(string a_sPrompt);
	string GetPrompt() { return m_sPrompt; }
	void SetLWD(string a_sLastWDir);
	string GetLWD() { return m_sLastWDir; }
	
	void executeCommand(const char* cmd_line);
	JobsList* GetJobList();
};

#endif //SMASH_COMMAND_H_
