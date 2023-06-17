/*MIT License

Copyright (c) 2022 Shahryar Ahmad 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#ifndef LEXER_H_
#define LEXER_H_
#include <vector>
#include <climits>
#include <cfloat>
#include "token.h"
using namespace std;
int32_t hexToInt32(const string&);
int64_t hexToInt64(const string&);
int32_t hexToInt32(const string& s)
{
    int32_t res = 0;
    int32_t p = 1;
    for(int32_t i=s.length()-1;i>=0;i--)
    {
        if(s[i] >= '0' && s[i]<='9')
        {
            res+= (s[i]-48) * p;
        }
        else if(s[i] >= 'a' && s[i]<='z')
        {
            res+= (s[i]-87) * p;
        }
        p<<=4;
    }
    return res;
}
int64_t hexToInt64(const string& s)
{
    int64_t res = 0;
    int64_t p = 1;
    for(int32_t i=s.length()-1;i>=0;i--)
    {
        if(s[i] >= '0' && s[i]<='9')
        {
            res+= (s[i]-48) * p;
        }
        else if(s[i] >= 'a' && s[i]<='z')
        {
            res+= (s[i]-87) * p;
        }
        
        p<<=4;
    }
    return res;
}
string addlnbreaks(string s,bool& hadErr)
{

    unsigned int k = 0;
    bool escaped = false;//check if last char was
    string r = "";
    while(k<s.length())
    {
        if(s[k]=='\\')
        {
            if(escaped)
            {
              escaped = false;
              r+="\\";
            }
            else
              {
              escaped = true;
              }
        }
        else if(escaped)
        {
            if(s[k]=='n')
            {
                r+='\n';
            }
            else if(s[k]=='r')
            {
                r+='\r';
            }
            else if(s[k]=='t')
            {
                r+='\t';
            }
            else if(s[k]=='v')
            {
                r+='\v';
            }
            else if(s[k]=='b')
            {
                r+='\b';
            }
            else if(s[k]=='a')
            {
                r+='\a';
            }
            else if(s[k]=='"')
            {
                r+='"';
            }
            else
            {
                hadErr = true;
                return "Unknown escape character: \\"+s.substr(k,1);
               
            }
            escaped = false;
        }
        else if(!escaped)
        {
            r+=s[k];
        }
        k+=1;
    }
   if(escaped)
   {
       hadErr = true;
       return "Error string contains non terminated escape chars";
   }
	return r;
}
const char* keywords[] = {"var","if","else","while","dowhile","import","return","break","continue","function","nil","for","to","step","foreach","namespace","class","private","public","extends","try","catch","throw","yield","as","gc"};
bool isKeyword(string s)
{
  for(size_t k=0;k<sizeof(keywords)/sizeof(char*);k+=1)
  {
    if(s==(string)keywords[k])
    {
      return true;
    }
  }
  return false;
}
string lstrip(string);
void removeUselessNewlines(vector<Token>&);
extern bool REPL_MODE;
void REPL();
class Lexer
{
private:
  string filename;
  size_t line_num;
  string source_code;
  void lexErr(string type,string msg)
  {
    cout<<"There are syntax errors in the file. Tokenization failed."<<endl;
    cout<<"Try running the program using plutonium"<<endl;
    exit(1);
  }
public:
    vector<Token> generateTokens(string fname,const string& s)
    {
        //fname is current filename
        //it must always be present in files vector
        line_num = 1;
        filename = fname;
        source_code = s;
        size_t k = 0;
        size_t srcLen = s.length();
        char c;
        vector<Token> tokenlist;
        int ln = 1;
        while(k<srcLen)
        {
            c = s[k];
            if(c=='"')
            {
                size_t j = k+1;
                string t;
                bool match;
                bool escaped = false;
                while(j<srcLen)
                {
                    if(s[j]=='"')
                    {
                        if(escaped)
                          escaped = false;
                        else
                        {
                          match = true;
                          break;
                        }
                        t+=s[j];
                    }
                    else if(s[j]=='\\' && !escaped)
                    {
                        escaped = true;
                        t+=s[j];
                    }
                    else if(s[j]=='\n')
                    {
                        t+=s[j];
                        ln+=1;
                    }
                    else
                    {
                        t+=s[j];
                        if(escaped)
                        {
                            escaped=false;
                        }
                    }
                    j+=1;
                }
                if(!match)
                {
                    line_num = ln;
                    lexErr("SyntaxError","Error expected a '\"' at the end!");
                }
                Token i;
                line_num = ln;
                bool hadErr = false;
                i.content = t;
                if(hadErr)
                {
                    line_num = ln;
                    lexErr("SyntaxError",i.content);
                }
                i.type = STRING_TOKEN;
                i.ln = ln;
                tokenlist.push_back(i);
                k = j;
            }
            else if(c=='@')
            {
                if(k==srcLen-1)
                {
                    line_num = ln;
                    lexErr("SyntaxError","Invalid macro name");
                }
                size_t j = k+1;
                string t = "";
                while(j<srcLen)
                {

                    if(!isalpha(s[j]) && !isdigit(s[j]) && s[j]!='_')
                    break;
                    if(j==k+1 && isdigit(s[j]))
                    {
                        line_num = ln;
                        lexErr("SyntaxError","Invalid macro name");
                    }
                    t+=s[j];
                    j+=1;
                }
                Token tok;
                tok.type = MACRO_TOKEN;
                tok.content = "@"+t;
                tok.ln = ln;
                tokenlist.push_back(tok);
                k = j;
                continue;
            }
            else if(c=='#')
            {
                bool multiline = false;
                bool foundEnd = false;
                string content = "#";
                if(k+1 < srcLen && s[k+1]=='-')
                {
                  multiline = true;
                  content+="-";
                  //k+=1;
                }
                size_t j=k+1;
                size_t orgLn = ln;
                for(;j<srcLen;j+=1)
                {
                    if(!multiline && s[j]=='\n')
                    {
                        break;
                    }
                    else if(multiline && s[j] == '-')
                    {
                        if(j+1 < srcLen && s[j+1]=='#')
                        {
                            foundEnd = true;
                            j+=2;
                            content+="-#";
                            break;
                        }
                        content+=s[j];
                    }
                    else if(s[j] == '\n')
                    {
                      content+=s[j];
                      ln++;
                    }
                    else
                    {
                        content+=s[j];
                    }
                }
                if(multiline && !foundEnd)
                {
                  line_num = orgLn;
                  lexErr("SyntaxError","Unable to find the end of multiline comment!");
                }
                Token tok;
                tok.type = COMMENT_TOKEN;
                tok.ln = orgLn;
                tok.content = content;
                tokenlist.push_back(tok);
                k = j-1;
            }
            else if(isdigit(c))
            {
                //hex literal
                if(c=='0' && k!=srcLen-1 && s[k+1]=='x')
                {

                    k+=1;
                    size_t j = k+1;
                    string b;
                    Token i;
                    while(j<srcLen)
                    {
                        c = s[j];
                        if(c>='0' && c<='9')
                            b+=c;
                        else if(c>='a' && c<='z')
                          b+=c;
                        else
                          break;
                        j+=1;

                    }
                    if(b.length() == 1 || b.length() == 2) //byte
                    {
                       i.type = HEX_TOKEN;
                       i.ln = ln;
                       i.content = b;
                       tokenlist.push_back(i);
                    }
                    else if(b.length() >= 3 && b.length()<=8)//int32
                    {
                       i.type = HEX_TOKEN;
                       i.ln = ln;
                       i.content = b;
                       tokenlist.push_back(i);
                    }
                    else if(b.length()>8 &&  b.length() <= 16)//int64
                    {
                       i.type = HEX_TOKEN;
                       i.ln = ln;
                       i.content = b;
                       tokenlist.push_back(i);
                    }
                    else
                    {
                        line_num = ln;
                        lexErr("SyntaxError","Invalid Syntax");
                    }
                    k = j;
                    continue;
                }
                size_t j = k+1;
                string t;
                t+=c;
                bool decimal = false;
                bool exp = false;

                bool expSign = false;
                while(j<srcLen)
                {
                    if(!isdigit(s[j]) && s[j]!='.' && s[j]!='e' && s[j]!='-' && s[j]!='+')
                    {
                        j = j-1;
                        break;
                    }
                    if(s[j]=='.')
                    {
                        if(decimal || exp)
                        {
                            j = j-1;
                            break;
                        }
                        decimal = true;
                    }
                    else if(s[j]=='e')
                    {
                        if(exp)
                        {
                            j = j-1;
                            break;
                        }
                        exp = true;
                    }
                    else if(s[j]=='+' || s[j]=='-')
                    {
                        if(expSign || !exp)
                        {
                            j = j-1;
                            break;
                        }
                        expSign = true;
                    }
                    t+=s[j];
                    j+=1;
                }
                if(t[t.size()-1]=='e')
                {
                    t = t.substr(0,t.length()-1);
                    j-=1;
                }
                Token i;
                i.content = t;
                if(!decimal && !exp)
                i.type = NUM_TOKEN;
                else
                i.type = FLOAT_TOKEN;
                i.ln = ln;
                tokenlist.push_back(i);
            k = j;
            }
            else if(c=='>' || c=='<')
            {
                if(k==srcLen-1)
                {
                    line_num = ln;
                    lexErr("SyntaxError","Invalid Syntax");
                }
                if(s[k+1]=='=')
                {
                    Token i;
                    i.type = TokenType::OP_TOKEN;
                    i.content+=c;
                    i.content+="=";
                    i.ln = ln;
                    tokenlist.push_back(i);
                    k = k+1;
                }
                else if(c=='<' && s[k+1]=='<')
                {
                    Token i;
                    i.type=TokenType::OP_TOKEN;
                    i.content ="<<";
                    tokenlist.push_back(i);
                    k+=1;
                }
                else if(c=='>' && s[k+1]=='>')
                {
                    Token i;
                    i.type=TokenType::OP_TOKEN;
                    i.content =">>";
                    tokenlist.push_back(i);
                    k+=1;
                }
                else
                {
                    Token i;
                    i.type = TokenType::OP_TOKEN;
                    i.content+=c;
                    i.ln = ln;
                    tokenlist.push_back(i);

                }
            }
            else if(c=='.')
            {
            if(k==srcLen-1)
            {
                    line_num = ln;
                    lexErr("SyntaxError","Invalid Syntax");
            }
                Token i;
                i.type = TokenType::OP_TOKEN;
                i.content+=c;
                i.ln = ln;
                tokenlist.push_back(i);
            
            }
            else if(c=='+' || c=='-')
            {
                if(k!=srcLen-1)
                {
                    if(s[k+1]=='=')
                    {
                    Token i;
                    i.ln = ln;
                    i.content += c;
                    i.content+="=";
                    i.type = TokenType::OP_TOKEN;
                    tokenlist.push_back(i);
                    k+=2;
                    continue;
                    }
                }
                Token i;
                i.ln = ln;
                i.content += c;
                i.type = TokenType::OP_TOKEN;
                tokenlist.push_back(i);
            }
            else if(c=='/' || c=='*' || c=='%' || c=='^' || c=='&' || c=='|' || c=='~')
            {

                if(k!=srcLen-1)
                {
                    if(s[k+1]=='=')
                    {
                    Token i;
                    i.ln = ln;
                    i.content += c;
                    i.content+="=";
                    i.type = TokenType::OP_TOKEN;
                    tokenlist.push_back(i);
                    // = "TokenType::OP_TOKEN";
                    k+=2;
                    continue;
                    }
                }
                Token i;
                i.ln = ln;
                i.content += c;
                i.type = TokenType::OP_TOKEN;
                tokenlist.push_back(i);
                // = "TokenType::OP_TOKEN";


            }
            else if(c=='(')
            {
                Token i;
                i.content += c;
                i.type = TokenType::LParen_TOKEN;
                i.ln = ln;
                tokenlist.push_back(i);
            }
            else if(c==')')
            {
                Token i;
                i.content += c;
                i.type = TokenType::RParen_TOKEN;
                            i.ln = ln;
                tokenlist.push_back(i);
            }
            else if(c=='!')
            {
                if(k!=srcLen-1)
                {
                    if(s[k+1]=='=')
                    {
                        Token i;
                        i.content = "!=";
                        i.type = TokenType::OP_TOKEN;
                        i.ln = ln;
                        k+=1;
                        tokenlist.push_back(i);
                    }
                    else
                    {
                        Token i;
                        i.content = "!";
                        i.type = TokenType::OP_TOKEN;
                        i.ln = ln;
                        tokenlist.push_back(i);
                    }
                }
                else
                {
                    Token i;
                    i.content = "!";
                    i.type = TokenType::OP_TOKEN;
                    i.ln = ln;
                    tokenlist.push_back(i);
                }
            }
            else if(c=='=')
            {
                if(s[k+1]=='=')
                {
                Token i;
                i.content = "==";
                i.type = TokenType::OP_TOKEN;
                i.ln = ln;
                k+=1;
                tokenlist.push_back(i);
                }
                else
                {
                Token i;
                i.content = "=";
                i.type = TokenType::OP_TOKEN;
                i.ln = ln;
                tokenlist.push_back(i);
                }
            }

            else if(c=='[')
            {
                Token i;
                i.content = "[";
                i.type = BEGIN_LIST_TOKEN;
                i.ln = ln;
                tokenlist.push_back(i);
            }
            else if(c==']')
            {

                    Token i;
                    i.content = "]";
                    i.type = END_LIST_TOKEN;
                            i.ln = ln;
                    tokenlist.push_back(i);
            }
            else if(c=='{')
            {

                Token i;
                i.content = "{";
                i.type = L_CURLY_BRACKET_TOKEN;
                i.ln = ln;
                tokenlist.push_back(i);
            }
            else if(c=='}')
            {
                Token i;
                i.content = "}";
                i.type = R_CURLY_BRACKET_TOKEN;
                i.ln = ln;
                tokenlist.push_back(i);
            }
            else if(isalpha(c) || c=='_')
            {
                size_t j = k+1;
                string t;
                t+=c;
                while(j<srcLen)
                {
                    if((j!=srcLen-1 && s[j]==':' && s[j+1]==':'))
                    {
                        t+="::";
                        j+=2;
                    }
                    else if(!isalpha(s[j]) && !isdigit(s[j]) && s[j]!='_')
                    {
                        j = j-1;
                        break;
                    }
                    else
                    {
                        t+=s[j];
                        j+=1;
                    }
                }

                Token i;
                if(isKeyword(t))
                {
                    if(t=="if" && k!=0 && tokenlist.size()!=0)
                    {
                        if(tokenlist[tokenlist.size()-1].type == KEYWORD_TOKEN && tokenlist[tokenlist.size()-1].content=="else")
                        {
                            tokenlist[tokenlist.size()-1].content+=" if";
                            k = j+1;
                            continue;
                        }
                    }
                    i.type = KEYWORD_TOKEN;
                    i.content = t;
                    i.ln = ln;
                }
                else if(t=="or" || t=="and" || t=="is")
                {
                    i.content = t;
                    i.type = TokenType::OP_TOKEN;
                    i.ln = ln;
                }
                else if(t=="true" || t=="false")
                {
                    i.content = t;
                    i.type = BOOL_TOKEN;
                    i.ln = ln;
                }
                else
                {
                    i.type = ID_TOKEN;
                    i.content = t;
                    i.ln = ln;
                }
                tokenlist.push_back(i);
                k = j;
            }
            else if(c==',')
            {
                Token i;
                i.type = COMMA_TOKEN;
                i.content = ",";
                i.ln = ln;
                tokenlist.push_back(i);
            }
            else if(c==':')
            {
                Token i;
                i.type = COLON_TOKEN;
                i.content = ":";
                i.ln = ln;
                tokenlist.push_back(i);
            }
            else if(c=='\n' )
            {
                Token i;
                i.content = "\n";
                i.type = NEWLINE_TOKEN;;
                i.ln = -1;
                tokenlist.push_back(i);
                ln+=1;
            }
            else if(c=='\t' )
            {
                Token i; 
                i.content = "\t";
                i.type = TAB_TOKEN;;
                i.ln = ln;
                tokenlist.push_back(i);
            }
            else if(c==' ' )
            {
                Token i;
                i.content = " ";
                i.type = SPACE_TOKEN;;
                i.ln = ln;
                tokenlist.push_back(i);
            }
            else if(c=='\r')
            {

            }
            else
            {
                //error;
                    if(c<0)
                    {
                        printf("Error: The given file is non ascii\n");
                        exit(0);
                    }
                    line_num = ln;
                    lexErr("SyntaxError","Invalid Syntax");
            }
            k+=1;
        }
        return tokenlist;
    }
};
#endif // LEXER_H_
