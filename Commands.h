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
  Command(const char* cmd_line){ cout << "Command class constracted"<<endl;}
  virtual ~Command(){ cout << "Command class destroyed"<<endl;}
  virtual void execute() = 0;
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line):Command(cmd_line){cout<<"BIC class constracted"<<endl;}
  virtual ~BuiltInCommand() {cout<<"BIC class destroyed"<<endl;}
};

class ExternalCommand : public Command {
 public:
	const char* c_cmd_line;
	JobsList* jobs_list;
	ExternalCommand(const char* cmd_line, JobsList* jobs):Command(cmd_line){c_cmd_line = cmd_line; jobs_list = jobs;}
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
  
   ChangeDirCommand(const char* cmd_line):BuiltInCommand(cmd_line){c_cmd_line=cmd_line;cout<<"ChangeDir class constracted"<<endl;}
   
   virtual ~ChangeDirCommand() {cout<<"ChangeDir class destroyed"<<endl;}
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
 // };
 // TODO: Add your data members
 public:
  CommandsHistory();
  ~CommandsHistory() {}
  //void addRecord(const char* cmd_line);
  void printHistory();
};

class HistoryCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  HistoryCommand(const char* cmd_line, CommandsHistory* history):BuiltInCommand(cmd_line){}
   ~HistoryCommand() {}
  void execute() override;
};

class JobsList {
public: 
	class JobEntry {
	   // TODO: Add your data members
	   int JobId;
	   pid_t PID;
	   string sJobsCommand;
	   int statusJobs; // 0-stopped,1-backgrounded,2-foregrounded,-1-finished
	   int time_started;
};
// TODO: Add your data members

  vector<JobEntry> * m_pvJobs;
  JobEntry* m_pForeground;
  int maxJobId=0;
  pid_t LastStopped;
  JobsList(){
     m_pvJobs = new vector <JobEntry> ; // TODO
     memset(&m_pForeground, 0, sizeof(m_pForeground));
  }
  ~JobsList()
  {
     delete m_pvJobs;
  }

 void addJob(string a_strCommand, int a_nPid, int a_nstatusJobs, int a_nJobId=-1, bool a_bForeground = false);
 void applyToAll(void (*a_pfun)(int));
 //--------will add above to smash------
 void printJobsList (); // prints info on jobs from min job id to max from JobsList
 void removeJobByPlace(int a_viJobs);// goes to node in vector and checks.If finished-removes.
 void printJobByPlace(int a_viJobs);// goes to node and prints info
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


class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  JobsCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){}
  ~JobsCommand() {}
  void execute() override {cout<<"JobsCommand"<<endl;}
};



class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
	const char* c_cmd_line;
	JobsList* jobs_list;
	KillCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){c_cmd_line=cmd_line; jobs_list=jobs;}
	~KillCommand() {}
	void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){}
  ~ForegroundCommand() {}
  void execute() override {cout<<"FGCommand"<<endl;}
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  BackgroundCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){}
   ~BackgroundCommand() {}
  void execute() override {cout<<"BGCommand executed"<<endl;}
};

class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
public:
  QuitCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){}
   ~QuitCommand() {}
  void execute() override {cout<<"QuitCommand!"<<endl;}
};
// TODO: should it really inhirit from BuiltInCommand ?
class CopyCommand : public BuiltInCommand {
 public:
  CopyCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
   ~CopyCommand() {};
  void execute() override;
};

// TODO: add more classes if needed 
// maybe chprompt , timeout ?

class ChpromptCommand : public BuiltInCommand {
	public:
	const char* c_cmd_line;
	
	ChpromptCommand (const char* cmd_line):BuiltInCommand(cmd_line){c_cmd_line=cmd_line;}
	~ChpromptCommand () {}
	void execute () override ;
	
};



class SmallShell {
 private:
  string m_sPrompt = "smash>";
  const string mc_sDefPrompt="smash>";
  string m_sLastWDir;
  JobsList* jobs_list;

  SmallShell() { *jobs_list = JobsList();}
 public:
  Command *CreateCommand(const char* cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  void SetPrompt (string a_sPrompt);
  string GetPrompt() {return m_sPrompt;}
  void SetLWD(string a_sLastWDir);
  string GetLWD(){return m_sLastWDir;}
  ~SmallShell(){}
  void executeCommand(const char* cmd_line);
  // TODO: add extra methods as needed
};

#endif //SMASH_COMMAND_H_

