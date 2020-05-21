#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
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
string _ltrim(const std::string& s)
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
    while (1)
		{
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
     SetPrompt("");
 }

 void SmallShell::SetPrompt (string a_sPrompt)
 {
     m_sPrompt = a_sPrompt.empty() ? mc_sDefPrompt : a_sPrompt;
 };
 void SmallShell::SetLWD (string a_sLastWDir)
 {
	 m_sLastWDir = a_sLastWDir;
 }
/*
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
	// For example:

  string cmd_s = string(cmd_line);
  vector<string> args;
  split(cmd_s, args);
  cout<<args.size()<<endl;
  getchar();
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
  else if (cmd_s.find("jobs") == 0)
    {
	   JobsList FakeJob;
       JobsList* pFakeJob;
       pFakeJob = &FakeJob;
      
		return new JobsCommand (cmd_line, pFakeJob);
    }
  else if (cmd_s.find("kill") == 0)
    {
		JobsList FakeJob;
        JobsList* pFakeJob;
        pFakeJob = &FakeJob;
        
		return new KillCommand (cmd_line,pFakeJob);
	}
  else if (cmd_s.find("fg") == 0)
    {
		JobsList FakeJob;
       JobsList* pFakeJob;
       pFakeJob = &FakeJob;
       
		return new ForegroundCommand (cmd_line,pFakeJob);
	}

  else if (cmd_s.find("bg") == 0)
    {
		JobsList FakeJob;
       JobsList* pFakeJob;
       pFakeJob = &FakeJob;
      
		return new BackgroundCommand (cmd_line,pFakeJob);
	}
	
  else if (cmd_s.find("quit") == 0)
    {
		JobsList FakeJob;
       JobsList* pFakeJob;
       pFakeJob = &FakeJob;
       
		return new QuitCommand (cmd_line,pFakeJob);
	}
  else {
    return new ExternalCommand(cmd_line);
  }
  
  return nullptr;
}

void ChpromptCommand::execute()
{
	std::cout<<"ChpromptCommand"<<std::endl;
	vector<string> args;
	split (c_cmd_line, args);	
	getchar();
	SmallShell& smash=SmallShell::getInstance();
	getchar();
	if ( args.size() > 1 )
	{
       string cursor_Prompt=args[1];
       cursor_Prompt.push_back('>');
	   smash.SetPrompt(cursor_Prompt);
    }
    else
    {
		cout<<"else in ChpromtCommand"<<endl;
		getchar();
	   smash.SetPrompt("smash>");
	}
}

void ShowPidCommand::execute()
{
	cout<<"ShowPidCommand!"<<endl;
	cout<<"smash pid is "<<getpid()<<endl;
}

void GetCurrDirCommand::execute ()
{
	char* ca_CurrDir;
	ca_CurrDir= get_current_dir_name();
	cout<<endl<<ca_CurrDir<<endl;
	cout<<"in PWD exec"<<endl;
	free(ca_CurrDir);
} 

void ChangeDirCommand::execute()
{
	std::cout<<"ChangeDirCommand"<<std::endl;
	vector<string> args;
	split (c_cmd_line, args);	
	getchar();
	SmallShell& smash=SmallShell::getInstance();
	getchar();
	if(args.size()==1){cout<<smash.GetPrompt();}
	else if(args.size()==2)
	{
		char* ca_CurrDir;
	    ca_CurrDir= get_current_dir_name();
	    
	    if(args[1]=="-")
	    {
			string sdestDir=smash.GetLWD();
			if(sdestDir.size()==0) {cerr<<"smash error: cd: OLDPWD not set"<<endl;}
			else
			{
				chdir(sdestDir.c_str());
				smash.SetLWD(string(ca_CurrDir));
			}
		}
		else if(args[1]=="..")
		{
			string tempstr=string(ca_CurrDir);
			size_t nLastSlash=tempstr.find_last_of("/");
			string sdestDir=tempstr.substr(0,nLastSlash);
			cout<<sdestDir<<endl;
			chdir(sdestDir.c_str());
			smash.SetLWD(string(ca_CurrDir));
		}
		else
		{
			size_t nFirstSlash=args[1].find_first_of("/");
			//absolute path
			if(nFirstSlash==0)
			{
				string tempstr=args[1];
				chdir(tempstr.c_str());
				smash.SetLWD(string(ca_CurrDir));
			}
			//relative path
			else
			{
				string tempstr1=args[1].insert(0,"/");
				string tempstr2=tempstr1.insert(0,ca_CurrDir);
				chdir(tempstr2.c_str());
				smash.SetLWD(string(ca_CurrDir));
			}
		}
		
	}
    else if(args.size()>2)
    {
		cerr<<"smash error: cd: too many arguments"<<endl;
	}
}

void SmallShell::executeCommand(const char *cmd_line) {
  // TODO: Add your implementation here
  // for example:
  
  Command* cmd = CreateCommand(cmd_line);
  if( cmd != nullptr ) 
  {
  cmd->execute();
  delete cmd;
  }
  // Please note that you must fork smash process for some commands (e.g., external commands....)
}

