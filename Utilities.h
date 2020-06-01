#ifndef SMASH__UTILITIES_H_
#define SMASH__UTILITIES_H_

#include "Commands.h"

const std::string WHITESPACE = " \n\r\t\f\v";


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
		a_vOut.push_back("\n");
	}
}

bool _isBackgroundComamnd(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

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

bool checkIfStrIsNum(string str){
	for(uint i=0 ; i <= str.size()-1 ; i++)
	{
		if((str[i] < '0' || str[i] > '9') && str[i] != '-')
		{
			return false;
		}
	}
	return true;
}

bool isRedirectionCommand(const string str) 
{
     return str.find('>') != string::npos;
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

#endif //SMASH__UTILITIES_H_
