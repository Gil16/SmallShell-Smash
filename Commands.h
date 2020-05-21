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
  ExternalCommand(const char* cmd_line):Command (cmd_line){}
   ~ExternalCommand() {}
  void execute() override {std::cout<<"ExternalCommand"<<std::endl;};
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

class JobsList;


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
 // class JobEntry {
   // TODO: Add your data members
 // };
 // TODO: Add your data members
 public:
  JobsList(){}
  ~JobsList(){}
  //void addJob(Command* cmd, bool isStopped = false);
  //void printJobsList();
  //void killAllJobs();
  //void removeFinishedJobs();
 // JobEntry * getJobById(int jobId);
  //void removeJobById(int jobId);
  //JobEntry * getLastJob(int* lastJobId);
  //JobEntry *getLastStoppedJob(int *jobId);
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  JobsCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){}
  ~JobsCommand() {}
  void execute() override {cout<<"JobsCommand"<<endl;};
};



class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  KillCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){}
  ~KillCommand() {}
  void execute() override {cout<<"KillCommand"<<endl;};
};


class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){}
  ~ForegroundCommand() {}
  void execute() override {cout<<"FGCommand"<<endl;};
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  BackgroundCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){}
   ~BackgroundCommand() {}
  void execute() override {cout<<"BGCommand executed"<<endl;};
};

class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
public:
  QuitCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line){}
   ~QuitCommand() {}
  void execute() override {cout<<"QuitCommand!"<<endl;};
};
// TODO: should it really inhirit from BuiltInCommand ?
class CopyCommand : public BuiltInCommand {
 public:
  CopyCommand(const char* cmd_line):BuiltInCommand(cmd_line){}
   ~CopyCommand() {};
  void execute() override ;
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

  SmallShell();
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
  string GetPrompt() {return m_sPrompt;};
  void SetLWD(string a_sLastWDir);
  string GetLWD(){return m_sLastWDir;};
  ~SmallShell(){}
  void executeCommand(const char* cmd_line);
  // TODO: add extra methods as needed
};

#endif //SMASH_COMMAND_H_

