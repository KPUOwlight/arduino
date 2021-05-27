#ifndef __MSG_PARSER_H__
#define __MSG_PARSER_H__

#include <string.h>
#include <map>



//======================================================================================//
typedef std::map <std::string, std::string>  TOKEN_MAP;
//======================================================================================//

class CMsgParser
{
  TOKEN_MAP   m_TokenMap;
  String      m_strType;

  void  parse(char* szMsg,int nLen);
  void  flush();
  


  
public:  
	
	String GetType();
  String GetValue(String strKey);
	
	//-----------------------------------------------------------------//
	CMsgParser(char* Msg, int nLen);
	~CMsgParser();
	//-----------------------------------------------------------------//

};

#endif//__MSG_PARSER_H__
