/*
 *
 *  arm_asm_generator.cpp
 *  ARM的汇编代码生成
 *  Creator:Sichao Chen
 *  Create time:2021/12/04
 *
*/
#include "arm_asm_generator.h"
#include<iostream>

//ARM内存信息
string arm_memory_info="little_ending";

Arm_asm_generator::Arm_asm_generator()
{

}

Arm_asm_generator::~Arm_asm_generator()
{
    
}

//初始化汇编指令生成器
bool Arm_asm_generator::init_instruction_generator(map<reg_index,string> regs_info)
{
    instruction_generator_=new Arm_instruction_generator(regs_info);
    instruction_generator_->set_mediator(this);
    return instruction_generator_->is_init_successful();
}

/*
组件通知该汇编代码生成器有事件发生

Parameters
----------
sender发生事件的组件
event:发生的事件

Return
------
返回处理完事件之后返回的响应数据
*/
// #include<iostream>
// using namespace std;
struct event Arm_asm_generator::notify(Asm_generator_component *sender, struct event event) const
{
    struct event res;
    
    if(sender==instruction_generator_)
    {
        //cout<<"instruction_generator_ send "<<(int)event.type<<endl;
        switch(event.type)
        {
            case event_type::NEXT_IC:
                res=intermediate_code_manager_->handler(event);
                break;
            case event_type::GET_REG_FOR_INT_CONST:
            case event_type::GET_REG_FOR_READING_VAR:
            case event_type::GET_REG_FOR_WRITING_VAR:
            case event_type::GET_SP_REG:
            case event_type::GET_R0_REG:
            case event_type::GET_R1_REG:
            case event_type::GET_S0_REG:
            case event_type::IS_CPU_REG:
            case event_type::READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS:
            case event_type::READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS:
            case event_type::CLEAR_FLAG:
            case event_type::RETURN_VAR:
            case event_type::GET_ADDR_REG:
            case event_type::START_INSTRUCTION:
            case event_type::END_INSTRUCTION:
            case event_type::FUNC_RET:
            case event_type::SAVE_REGS_WHEN_CALLING_FUNC:
            case event_type::SAVE_REGS_WHEN_CALLING_ABI_FUNC:
            case event_type::PLACE_ARGUMENT_IN_REGS_WHEN_CALLING_FUNC:
            case event_type::CHECK_CONST_INT_OWN_VALUE_REG:
            case event_type::CHECK_VAR_OWN_VALUE_REG:
            case event_type::CHANGE_FLAGS_FOR_VAR:
            case event_type::CHECK_VAR_ATTACHED_TO_FLAG:
            case event_type::GET_VAR_CARED_FLAG:
            case event_type::GET_CONST_INT_S_VALUE_REG:
            case event_type::ALLOCATE_IDLE_CPU_REG:
            case event_type::ATTACH_CONST_INT_TO_REG:
                res=register_manager_->handler(event);
                break;
            case event_type::FUNC_DEFINE:
                memory_manager_->handler(event);
                register_manager_->handler(event);
                break;
            case event_type::READY_TO_PUSH_LOCAL_VARS:
            case event_type::READY_TO_POP_LOCAL_VARS:
            case event_type::READY_TO_POP_F_PARAM_CPU_REGS:
            case event_type::READY_TO_POP_F_PARAM_VFP_REGS:
            case event_type::READY_TO_POP_CONTEXT_RECOVERED_CPU_REGS:
            case event_type::READY_TO_POP_CONTEXT_RECOVERED_VFP_REGS:
            case event_type::GET_VAR_STACK_POS_FROM_SP:
            case event_type::PUSH_VAR_TO_STACK:
            case event_type::READY_TO_POP_TEMP_VARS:
            case event_type::PUSH_ARGUMENT_TO_STACK_WHEN_CALLING_FUNC:
            case event_type::CHECK_TEMP_VAR_IN_STACK:
                res=memory_manager_->handler(event);
                break;
            case event_type::READY_TO_PUSH_F_PARAM_CPU_REGS:
            case event_type::READY_TO_PUSH_F_PARAM_VFP_REGS:
                res=register_manager_->handler(event);
                memory_manager_->handler(event);
                break;
            case event_type::CALL_FUNC:
            case event_type::CALL_ABI_FUNC:
            // case event_type::PUSH_TEMP_VAR_FROM_REG_TO_STACK:
            case event_type::MOVE_DATA_BETWEEN_CPU_REGS:
            case event_type::ASSIGN_VAR:
                instruction_generator_->handler(event);
                break;
            case event_type::RET_FROM_CALLED_FUNC:
            case event_type::RET_FROM_CALLED_ABI_FUNC:
            case event_type::END_BASIC_BLOCK_WITHOUT_FLAG:
                register_manager_->handler(event);
                res=memory_manager_->handler(event);
                break;
            default:
                break;
        }
    }
    else if(sender==register_manager_)
    {
        //cout<<"register_manager_ send "<<(int)event.type<<endl;
        switch(event.type)
        {
            case event_type::STORE_VAR_TO_MEM:
            case event_type::LOAD_VAR_TO_REG:
            case event_type::WRITE_CONST_INT_TO_REG:
            case event_type::WRITE_CONST_FLOAT_TO_REG:
            case event_type::WRITE_ADDR_TO_REG:
            case event_type::PUSH_TEMP_VAR_FROM_REG_TO_STACK:
            case event_type::MOVE_DATA_BETWEEN_CPU_REGS:
                instruction_generator_->handler(event);
                break;
            case event_type::READY_TO_PUSH_CONTEXT_SAVED_CPU_REGS:
            case event_type::READY_TO_PUSH_CONTEXT_SAVED_VFP_REGS:
                memory_manager_->handler(event);
                break;
            case event_type::START_INSTRUCTION:
            case event_type::END_INSTRUCTION:
                register_manager_->handler(event);
                break;
            default:
                break;
        }
    }
    else if(sender==memory_manager_)
    {
        //cout<<"memory_manager_ send "<<(int)event.type<<endl;
        switch(event.type)
        {
            case event_type::GET_REG_BYTE_SIZE:
            case event_type::START_INSTRUCTION:
            case event_type::END_INSTRUCTION:
            case event_type::GET_LR_REG:
            case event_type::GET_PC_REG:
                res=register_manager_->handler(event);
                break;
            case event_type::POP_STACK:
                instruction_generator_->handler(event);
                break;
            default:
                break;
        }
    }
    else if(sender==intermediate_code_manager_)
    {
        //cout<<"intermediate_code_manager_ send "<<(int)event.type<<endl;
        switch(event.type)
        {
            case event_type::START_BASIC_BLOCK:
            case event_type::START_FUNC:
                instruction_generator_->handler(event);
                break;
            case event_type::END_BASIC_BLOCK:
                register_manager_->handler(event);
                memory_manager_->handler(event);
                break;
            case event_type::END_FUNC:
                register_manager_->handler(event);
                break;
            default:
                break;
        }
    }
    return res;
}

/*
ARM汇编代码生成器的初始化

Return
------
如果初始化成功返回true，否则返回false
*/
bool Arm_asm_generator::init()
{
    //ARM可用的寄存器的名字以及描述
    set<struct reg> arm_regs={
        reg("r0",0,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r1",1,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r2",2,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r3",3,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r4",4,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r5",5,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r6",6,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r7",7,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r8",8,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r9",9,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r10",10,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r11",11,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("r12",12,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("sp",13,reg_attr::STACK_POINTER,false,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("lr",14,reg_attr::RETURN_ADDR,true,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("pc",15,reg_attr::PC,false,reg_state::NOT_USED,32,reg_processor::CPU),
        reg("s0",16,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s1",17,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s2",18,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s3",19,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s4",20,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s5",21,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s6",22,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s7",23,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s8",24,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s9",25,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s10",26,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s11",27,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s12",28,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s13",29,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s14",30,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s15",31,reg_attr::ARGUMENT,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s16",32,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s17",33,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s18",34,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s19",35,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s20",36,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s21",37,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s22",38,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s23",39,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s24",40,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s25",41,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s26",42,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s27",43,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s28",44,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s29",45,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s30",46,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
        reg("s31",47,reg_attr::TEMP,true,reg_state::NOT_USED,32,reg_processor::VFP),
    };
    return Asm_generator::init(arm_regs,flag_reg("cpsr",4,flag_in_flag_reg::NEGATIVE,flag_in_flag_reg::ZERO,flag_in_flag_reg::CARRY,flag_in_flag_reg::OVERFLOW),arm_memory_info);
}
