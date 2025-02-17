/*
 *
 *  ic_optimizer.cpp
 *  中间代码优化器
 *  Creator:Sichao Chen
 *  Create time:2022/3/8
 *
*/
#include"ic_optimizer.h"
#include<fstream>
#include<iostream>

//中间代码输出信息
extern map<ic_op,ic_output> ic_outputs;

Ic_optimizer::Ic_optimizer()
{

}

/*
初始化，设置优化选项
*/
void Ic_optimizer::init()
{
    
}

/*
局部优化
*/
void Ic_optimizer::local_optimize()
{

}

/*
数据流分析
*/
void Ic_optimizer::data_flow_analysis()
{

}

/*
全局优化
*/
void Ic_optimizer::global_optimize()
{

}

/*
代码优化

Parameters
----------
intermediate_codes:需要优化的中间代码序列

Return
------
返回最终的流图
*/
struct ic_flow_graph * Ic_optimizer::optimize(list<struct quaternion> * intermediate_codes)
{
    //建立中间代码流图
    intermediate_codes_flow_graph_=new struct ic_flow_graph(intermediate_codes);
    //进行局部优化
    local_optimize();
    //进行数据流分析
    data_flow_analysis();
    //进行全局优化
    global_optimize();
    //返回优化结果
    return intermediate_codes_flow_graph_;
}

/*
代码优化并将最终优化的中间代码流图输出到文件中

Parameters
----------
intermediate_codes:需要优化的中间代码序列
filename：要输出的文件名

Return
------
返回最终的流图
*/
struct ic_flow_graph * Ic_optimizer::optimize_then_output(list<struct quaternion> * intermediate_codes,const char * filename)
{
	ofstream outFile;
    struct ic_flow_graph * res;

    //进行优化
    res=optimize(intermediate_codes);

    if(intermediate_codes)
    {
        //把中间代码流图输出到文件中
        outFile.open(filename,ios::out);
	    if (!outFile)
        {
            cout<<"Open file "<<filename<<" error!"<<endl;
	    }
        else
        {
            //先输出全局定义
            for(auto i:res->global_defines)
            {
                outFile<<(ic_outputs[i.op](i))<<endl;
            }
            //再输出函数流图
            for(auto i:res->func_flow_graphs)
            {
                for(auto j:i->basic_blocks)
                {
                    outFile<<"=========================================BASIC_BLOCK:"<<(j)<<"========================================="<<endl;
                    for(auto k:j->ic_sequence)
                    {
                        outFile<<(ic_outputs[k.intermediate_code.op](k.intermediate_code))<<endl;
                    }
                    if(j->jump_next)
                    {
                        outFile<<"=========================================MAY_JUMP_TO:"<<(j->jump_next)<<"========================================="<<endl;
                    }
                    else
                    {
                        outFile<<"============================================================================================================"<<endl;
                    }
                }
            }
            outFile.close();
        }
    }

    return res;
}