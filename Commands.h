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
	
// TODO: Add your data members
 public:
	Command(const char* cmd_line){ }
	virtual ~Command(){}
	virtual void execute() = 0;
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
 public:
	BuiltInCommand(const char* cmd_line):Command(cmd_line){}
	virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
 public:
    const char* c_cmd_line;
	ExternalCommand(const char* cmd_line):Command(cmd_line){c_cmd_line = cmd_line;}
	~ExternalCommand() {}
	void execute() override;
};
/*
class PipeCommand : public Command {
  // TODO: Add your data members
 public:
  PipeCommand(const char* cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};
*/

/*
class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
  explicit RedirectionCommand(const char* cmd_line);
  virtual ~RedirectionCommand() {}
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};
*/
class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members public:
  public:
	const char* c_cmd_line;
	ChangeDirCommand(const char* cmd_line):BuiltInCommand(cmd_line){ c_cmd_line = cmd_line; }
	virtual ~ChangeDirCommand() {}
	void execute() override ;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
	GetCurrDirCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
	~GetCurrDirCommand() {}
	void execute() override ;
};

class ShowPidCommand : public BuiltInCommand {
 public:
	ShowPidCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
	~ShowPidCommand() {}
	void execute() override ;
};

class CommandsHistory {
 protected:
 // class CommandHistoryEntry {
	  // TODO: Add your data members
     // Add vector storing last commands here!
 // };
 // TODO: Add your data members
 public:
	CommandsHistory();
	~CommandsHistory() {}
	void addRecord(const char* cmd_line); //{ vector->pushBack(str(cmd_line)); }
	void printHistory();
};

class HistoryCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
	HistoryCommand(const char* cmd_line, CommandsHistory* history):BuiltInCommand(cmd_line){}
	~HistoryCommand() {}
	void execute() override;
};


typedef enum{
        eJobStatus_Stopped,
        eJobStatus_Background,
        eJobStatus_Foreground,
        eJobStatus_Finished //Have no idea why you need this one, you shall just remove the job instead of changing to it
} EJobStatus;


struct JobEntry {
	// TODO: Add your data members
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

	JobsList(){ m_pvJobs = new vector<JobEntry>; }
	~JobsList() { delete m_pvJobs; }
	void addJob(string a_strCommand, int a_nPid, EJobStatus a_status); // when you add it's always not in foreground, otherwise it's a bug.
	void addJob(JobEntry job);
	void addJobToForeground(string a_strCommand, int a_nPid); 
	void applyToAll(void (*a_pfun)(int));
 //--------will add above to smash------
	void printJobsList (); // prints info on jobs from min job id to max from JobsList //Nope, you just pass print one job command to apply to list
	bool removeJobByPlace(int a_viJobs);
	void printJobByPlace(int a_viJobs);// goes to node and prints info
	void printJobBeforeQuit(JobEntry job);
	void removeFinishedJobs();
	void updateJobstatusByPlace(int a_viJobs);
	void updateJobstatusByPID(EJobStatus a_nstatus,pid_t a_nPID);
	JobEntry* getJobById(int a_jobId);// if job is not in list returns NULL
	void removeJobById(int a_jobId);
	void removeJobByPID(int pid);
	void killAllJobs();
	JobEntry* getLastJob();// if no jobs in list 
	int getLastJobId();
	JobEntry* getForegroundJob();
	JobEntry* getLastStoppedJob();
 // TODO: Add extra methods or modify exisitng ones as needed
	vector<JobEntry>* m_pvJobs;
	int LastStopped=-1; //-1 on initialization or when no last stopped
private:
    // TODO: Add your data members
	int getIndexById(int a_jobId); 
  //int maxJobId=0; // you don't need this, since the list is sorted
	
};

//-----------------------JobsList Class ends here---------------------
//--------------------------------------------------------------------


class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
	const char* c_cmd_line;
 public:
	JobsCommand(const char* cmd_line):BuiltInCommand(cmd_line){ c_cmd_line = cmd_line; }
	~JobsCommand() {}
	void execute() override;
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
  //    JobsList* m_pJobsList;
    const char* c_cmd_line;
 public:
	KillCommand(const char* cmd_line):BuiltInCommand(cmd_line){ c_cmd_line = cmd_line; }
	~KillCommand() {}
	void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members    
 public:
	ForegroundCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
	~ForegroundCommand() {}
	void execute() override { cout << "FGCommand" << endl; }
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members 
    const char* c_cmd_line;  
 public:
	BackgroundCommand(const char* cmd_line):BuiltInCommand(cmd_line){c_cmd_line = cmd_line;}
	~BackgroundCommand() {}
	void execute() override ; 
};

class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
	const char* c_cmd_line;
 public:
	QuitCommand(const char* cmd_line):BuiltInCommand(cmd_line){ c_cmd_line = cmd_line; }
	~QuitCommand() {}
	void execute() override;
};

// TODO: should it really inhirit from BuiltInCommand ?
class CopyCommand : public BuiltInCommand {
 public:
	CopyCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
	~CopyCommand(){};
	void execute() override;
};

// TODO: add more classes if needed 
// maybe chprompt , timeout ?

class ChpromptCommand : public BuiltInCommand {
 public:
	const char* c_cmd_line;
	ChpromptCommand(const char* cmd_line):BuiltInCommand(cmd_line){ c_cmd_line = cmd_line; }
	~ChpromptCommand() {}
	void execute() override;
	
};



class SmallShell {
 private:
	string m_sPrompt = "smash>";
	const string mc_sDefPrompt="smash>";
	string m_sLastWDir;
	
	SmallShell();
    Command *CreateCommand(const char* cmd_line);
 public:
	JobsList* m_pJobsList;
	static JobEntry *m_pForeground;
	SmallShell(SmallShell const&)      = delete; // disable copy ctor
	void operator=(SmallShell const&)  = delete; // disable = operator
	static SmallShell& getInstance() // make SmallShell singleton
	{
		static SmallShell instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	void SetPrompt (string a_sPrompt);
	string GetPrompt() { return m_sPrompt; }
	void SetLWD(string a_sLastWDir);
	string GetLWD(){ return m_sLastWDir; }
	~SmallShell(){ delete m_pJobsList; }
	void executeCommand(const char* cmd_line);
	JobsList* GetJobList();
  // TODO: add extra methods as needed
};

#endif //SMASH_COMMAND_H_

