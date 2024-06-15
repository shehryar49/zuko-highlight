#include <iostream>
#include <string>
#include <fstream>
#include "lexer.h"
using namespace std;
string operatorToHtmlAscii(string op)
{
    string html;
    for(auto e: op)
    {
        html += "&#"+ to_string((int)e) + ";";
    }
    return html;
}
int main(int argc,const char* argv[])
{
  if(argc != 2)
  {
    cerr<<"usage: ./highlight <filename>"<<endl;
    return 1;
  }
  ifstream fin(argv[1],ios::in);
  if(!fin)
  {
    cerr<<"Error opening file"<<endl;
    return 1;
  }
  fin.seekg(0, std::ios::end);
  size_t size = fin.tellg();
  std::string src(size, ' ');
  fin.seekg(0);
  fin.read(&src[0], size); 
  //cout<<src;
  Lexer lex;
  vector<Token> tokens = lex.generateTokens(argv[1],src);
  string html = "<link rel=\"stylesheet\" href=\"theme.css\">\n<pre class=\"zukocode\">";
  for(auto tok: tokens)
  {
    if(tok.type == STRING_TOKEN)
      html += "<span class=\"string\">&quot;"+tok.content+"&quot;</span>";
    else if(tok.type == NUM_TOKEN)
      html += "<span class=\"num\">"+tok.content+"</span>";
    else if(tok.type == HEX_TOKEN)
      html += "<span class=\"hex\">0x"+tok.content+"</span>";
    else if(tok.type == COMMENT_TOKEN)
      html += "<span class=\"comment\">"+tok.content+"</span>";
    else if(tok.type == OP_TOKEN)
      html += "<span class=\"operator\">"+operatorToHtmlAscii(tok.content)+"</span>";
    else if(tok.type == MACRO_TOKEN)
      html += "<span class=\"macro\">"+tok.content+"</span>";
    else if(tok.type == FLOAT_TOKEN)
      html += "<span class=\"float\">"+tok.content+"</span>";
    else if(tok.type == KEYWORD1_TOKEN)
      html += "<span class=\"keyword1\">"+tok.content+"</span>";
    else if(tok.type == KEYWORD2_TOKEN)
      html += "<span class=\"keyword2\">"+tok.content+"</span>";
    else if(tok.type == BOOL_TOKEN)
      html += "<span class=\"bool\">"+tok.content+"</span>";
    else if(tok.type == ID_TOKEN)
      html += "<span class=\"id\">"+tok.content+"</span>";
    else
      html += tok.content;      
  }
  html+="</pre>";
  ofstream out("output.html",ios::out);
  out<<html;
  return 0;
}