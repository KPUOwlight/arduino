#include <Arduino.h>
#include "MsgParser.h"

#define NOT_FOUND -1
#define FIRST -2
#define FOUND 0

//-----------------------------------------------------------------------------------------------//

CMsgParser::CMsgParser(char* Msg, int nLen)
{
  parse(Msg,nLen);
}


CMsgParser::~CMsgParser()
{
	flush();	
}
//-----------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------//
void 
CMsgParser::flush()
{
    
  TOKEN_MAP::iterator iter = m_TokenMap.begin();
  while (iter != m_TokenMap.end())
  {
    m_TokenMap.erase(iter);
    iter =  m_TokenMap.begin();       
  }
  
  
}


void 
CMsgParser::parse(char* szMsg,int nMsgLen)
{
   
   char szKey[200];
   char szValue[200];
   memset(szKey,0x00,sizeof(szKey));
   memset(szValue,0x00,sizeof(szValue));

   int  nKeyLen = 0;
   int  nValueLen = 0;
   int  nValueStart = 0;
   for(int i =0; i < nMsgLen; i++)
   {
      if(szMsg[i] == '|') 
      {
        if( nValueLen == 0)
        { 
           if( nKeyLen != 0)
           {
              m_strType =  szKey;
           }
        }
        else
        {
            for(int i = nKeyLen -1 ; i >= 0; i--)
            {
              if(szKey[i] == ' ') szKey[i]= 0x00;
              else break; 
            }
            for(int i = nValueLen -1 ; i >= 0; i--)
            {
              if(szValue[i] == ' ') szValue[i]= 0x00;
              else break; 
            }
            

            
            std::string strKey  = szKey;
            std::string strValue= szValue;   
            m_TokenMap[strKey] = strValue;
        }

        memset(szKey,0x00,sizeof(szKey));
        memset(szValue,0x00,sizeof(szValue));
        nKeyLen = 0;
        nValueLen = 0;
        nValueStart = 0;
      }
      else if(szMsg[i] == '=')
      {
         nValueStart = 1;
         nValueLen = 0;
      }
     
      else
      {
        if(nValueStart == 0)
        {
          if(nKeyLen == 0 && szMsg[i] == ' ')
          {
            continue;
          }
          szKey[nKeyLen++] = szMsg[i];  
        }
        else
        {
          if(nValueLen == 0 && szMsg[i] == ' ')
          {
            continue;
          }

          szValue[nValueLen++] = szMsg[i]; 
        }
      }
   
   }

}

//-----------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------//
String 
CMsgParser::GetType()
{
  return m_strType;
}


String 
CMsgParser::GetValue(String strKey)
{
  String strReturn = "0";
  
  
  char szKey[200];
  memset(szKey,0x00,sizeof(szKey));
  strKey.toCharArray(szKey,sizeof(szKey));
  
  std::string strKey1  = szKey;
  TOKEN_MAP::iterator iter = m_TokenMap.find( strKey1);
  if (iter != m_TokenMap.end())
  {
    std::string strValue = iter->second;
    strReturn = (char*)strValue.c_str();
  }

  return strReturn;  
}
//-----------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------//
