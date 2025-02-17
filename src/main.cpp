#include<iostream>
#include<stdio.h>
#include<list>
#include<string>
#include "symbol_table.h"
#include "pre_processor.h"
#include "lexical_analyzer.h"
#include "syntax_directed_translator.h"
#include "arm_asm_generator.h"
#include "ic_optimizer.h"

using namespace std;

#define SYSY_INNER_HEADER_FILE_NAME "./lib/include/sylib.h"         //SysY语言内置的头文件名
#define SYMBOL_TABLE_OUTPUT_FILE_SUFFIX ".sym_tbl"             //符号表输出文件后缀
#define PRE_PROCESS_OUTPUT_FILE_SUFFIX ".pre"                   //程序经过预处理之后的输出文件后缀
#define TOKENS_OUTPUT_FILE_SUFFIX ".tok"                         //单词tokens输出文件后缀
#define PRODUCTIONS_OUTPUT_FILE_SUFFIX ".pro"               //语法分析得到的产生式输出的文件后缀
#define INTERMEDIATE_CODES_OUTPUT_FILE_SUFFIX ".ic"               //语义翻译得到的中间代码输出的文件后缀
#define FLOW_GRAPH_OUTPUT_FILE_SUFFIX ".fg"                     //中间代码优化之后获得的数据流图的文件后缀
#define ASM_CODES_OUTPUT_FILE_SUFFIX ".s"                   //目标汇编代码输出的文件

#define GRAMMER_FILE "./grammer/SysY.gra"                                      //书写语法的文件
#define ACTION_GOTO_FILE "action_goto"                                  //保存语法分析表action和goto的文件

#define DEBUG_FLAG "-debug"
#define GENERATE_ASM_FLAG "-S"
#define SPECIFY_TARGET_FLAG "-o"
#define OPTIMIZATION_FLAG "-O1"

bool lrc(string source_program_filename,string target_filename,bool debug=false)
{
    bool res=false;
    Symbol_table * symbol_table;
    Pre_processor pre;
    Lexical_analyzer lex;
    Syntax_directed_translator translator;
    Arm_asm_generator asm_generator;
    Ic_optimizer ic_optimizer;
    list<struct token * > * tokens;
    list<struct quaternion> * intermediate_codes;
    struct ic_flow_graph * intermediate_codes_flow_graph;

    //建立符号表
    symbol_table=Symbol_table::get_instance();

    //添加语言内置的头文件
    pre.add_inner_header_files(1,SYSY_INNER_HEADER_FILE_NAME);

    //先对源程序进行预处理
    if(!pre.pre_process(source_program_filename.c_str(),(target_filename+PRE_PROCESS_OUTPUT_FILE_SUFFIX).c_str()))
    {
        cout<<"Pre process error!\n"<<endl;
        goto out;
    }

    //建立词法分析器
    if(!lex.init((target_filename+PRE_PROCESS_OUTPUT_FILE_SUFFIX).c_str()))
    {
        cout<<"Lexical analyzer init error!"<<endl;
        goto out;
    }

    //进行词法分析
    if(debug)
    {
        tokens=lex.tokens_scan_then_output((target_filename+TOKENS_OUTPUT_FILE_SUFFIX).c_str());
    }
    else
    {
        tokens=lex.tokens_scan();
    }

    //输出符号表内容到文件
    //symbol_table->outptu_symbols((target_filename+SYMBOL_TABLE_OUTPUT_FILE_SUFFIX).c_str());

    //建立语法分析器
    if(!translator.init(GRAMMER_FILE,ACTION_GOTO_FILE,tokens,false))
    {
        cout<<"Syntax directed translator init error!"<<endl;
        goto out;
    }

    //进行语义翻译,生成中间代码
    if(debug)
    {
        if(!(intermediate_codes=translator.translate_then_output((target_filename+INTERMEDIATE_CODES_OUTPUT_FILE_SUFFIX).c_str())))
        {
            cout<<"Generate intermedia codes error!"<<endl;
            goto out;
        }
    }
    else
    {
        if(!(intermediate_codes=translator.translate()))
        {
            cout<<"Generate intermedia codes error!"<<endl;
            goto out;
        }
    }

    //初始化中间代码优化器
    ic_optimizer.init();

    //中间代码优化
    if(debug)
    {
        intermediate_codes_flow_graph=ic_optimizer.optimize_then_output(intermediate_codes,(target_filename+FLOW_GRAPH_OUTPUT_FILE_SUFFIX).c_str());
    }
    else
    {
        intermediate_codes_flow_graph=ic_optimizer.optimize(intermediate_codes);
    }

    //初始化汇编生成器
    if(!(asm_generator.init()))
    {
        cout<<"Asm generator init error!"<<endl;
        goto out;
    }

    //输出最终的汇编文件
    asm_generator.generate_asm_then_output(intermediate_codes_flow_graph,(target_filename+ASM_CODES_OUTPUT_FILE_SUFFIX).c_str());
    delete intermediate_codes_flow_graph;
    res=true;

out:
    //释放内存并退出
    Symbol_table::delete_instance();
    
    return res;
}

int main(int argc,char * argv[])
{
    string source_program_filename;
    string target_filename;
    string debug_info;
    bool debug;

    switch(argc)
    {
        case 3:
            source_program_filename=argv[1];
            target_filename=argv[2];
            debug=false;
            break;
        case 4:
            source_program_filename=argv[2];
            target_filename=argv[3];
            debug_info=argv[1];
            if(debug_info==DEBUG_FLAG)
            {
                debug=true;
            }
            else
            {
                cout<<"No such command!"<<endl;
                return -1;
            }
            break;
        default:
            cout<<"No such command!"<<endl;
            return -1;
            break;
    }

    return lrc(source_program_filename,target_filename,debug)?0:-1;
}
