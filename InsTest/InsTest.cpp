#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "string.h"
#include "vector"
#include "iterator"
#include "algorithm"
#include "llvm/IR/Module.h"

#define NUM 8 //特征数量

using namespace llvm;

namespace
{
//判断vector的某一元素是否存在
bool is_element_in_vector(std::vector<std::string> v, std::string element)
{
	std::vector<std::string>::iterator it;
	it = find(v.begin(), v.end(), element);
	if (it != v.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

std::vector<std::vector<std::string>> createIns()
{
	std::vector<std::vector<std::string>> insGroup(NUM, std::vector<std::string>(0));

	//指令类别
	//整型二元操作Cint"add""sub""mul""udiv""sdiv""urem""srem""shl""lshr""ashr"
	std::string ins_1[] = {"add", "sub", "mul", "udiv", "sdiv", "urem", "srem", "shl", "lshr", "ashr"};
	//浮点型二元操作Cfloat"fadd""fsub""fmul""fdiv""frem"
	std::string ins_2[] = {"fadd", "fsub", "fmul", "fdiv", "frem"};
	//比较指令Ccmp"icmp""fcmp"
	std::string ins_3[] = {"icmp", "fcmp"};
	//逻辑操作Cbit"and""or""xor"
	std::string ins_4[] = {"and", "or", "xor"};
	//转换操作Cconv"trunc""zext""sext""fptrunc""fpext""fptoui""fptosi""uitofp""sitofp""inttoptr""ptrtoint""bitcast""addrspacecast"
	std::string ins_5[] = {"trunc", "zext", "sext", "fptrunc", "fpext", "fptoui", "fptosi", "uitofp", "sitofp", "inttoptr", "ptrtoint", "bitcast", "addrspacecast"};
	//地址相关的操作Cmem""atomicrmw""fence""getelementptr"
	std::string ins_6[] = {"atomicrmw", "fence", "getelementptr"};
	//函数调用指令call"call""ret""br""switch""indirectbr""invoke""callbr""resume""catchswitch""catchret" "cleanupret""unreachable""catchpad""va_arg""landingpad" "cleanuppad"
	std::string ins_7[] = {"call", "ret", "br", "switch", "indirectbr", "invoke", "callbr", "resume", "catchswitch", "catchret", "cleanupret", "unreachable", "catchpad", "va_arg", "landingpad", "cleanuppad"};
	//内存读数指令load"load""alloca""store"
	std::string ins_8[] = {"load", "alloca", "store"};
	//不在这8类中的指令："shufflevector""extractelement""insertelement"cmpxchg""extractvalue""insertvalue""phi""select"

	//循环写入特征向量
	for (int i = 0; i < sizeof(ins_1) / 32; i++)
	{
		insGroup[0].push_back(ins_1[i]);
	}
	for (int i = 0; i < sizeof(ins_2) / 32; i++)
	{
		insGroup[1].push_back(ins_2[i]);
	}
	for (int i = 0; i < sizeof(ins_3) / 32; i++)
	{
		insGroup[2].push_back(ins_3[i]);
	}
	for (int i = 0; i < sizeof(ins_4) / 32; i++)
	{
		insGroup[3].push_back(ins_4[i]);
	}
	for (int i = 0; i < sizeof(ins_5) / 32; i++)
	{
		insGroup[4].push_back(ins_5[i]);
	}
	for (int i = 0; i < sizeof(ins_6) / 32; i++)
	{
		insGroup[5].push_back(ins_6[i]);
	}
	for (int i = 0; i < sizeof(ins_7) / 32; i++)
	{
		insGroup[6].push_back(ins_7[i]);
	}
	for (int i = 0; i < sizeof(ins_8) / 32; i++)
	{
		insGroup[7].push_back(ins_8[i]);
	}

	return insGroup;
}

//判断特征属于哪一个类型
int instype(std::string ins, std::vector<std::vector<std::string>> insGroup)
{
	int type = -1;

	for (int i = 0; i < NUM; i++)
	{
		if (is_element_in_vector(insGroup[i], ins))
			type = i;
	}
	return type;
}

struct InsTest : public ModulePass
{
	static char ID;
	InsTest() : ModulePass(ID) {}

	bool runOnModule(Module &M) override
	{
		std::map<std::string, int> opcode_map;
		// errs() << "函数名：";
		//		errs() << F.getName() << '\n';
		for (Module::iterator fI = M.begin(); fI != M.end(); ++fI)
		{
			Function &fun = *fI;
			// for (Function::iterator bb = F.begin(), e = F.end(); bb != e; bb++)
			for (Function::iterator bI = fun.begin(); bI != fun.end(); ++bI)
			{
				BasicBlock &block = *bI;
				for (BasicBlock::iterator i = bI->begin(), i2 = bI->end(); i != i2; i++)
				{
					//errs() << instype(i->getOpcodeName()) << '\n';
					std::vector<std::vector<std::string>> insGroup(NUM, std::vector<std::string>(0));
					insGroup = createIns();
					//循环输出8元组特征
					for (int m = 0; m < 8; m++)
					{
						if (instype(i->getOpcodeName(), insGroup) == m)
							errs() << 1 << ' ';
						else
							errs() << 0 << ' ';
					}
					errs() << '\n';
				}
			}

			std::map<std::string, int>::iterator p_start = opcode_map.begin();
			std::map<std::string, int>::iterator p_final = opcode_map.end();

			// while(p_start != p_final)
			// {
			// 	outs()<< p_start->first <<":::"<<p_start->second<<"\n";
			// 	p_start++;
			// }
			opcode_map.clear();
			return false;
		}
	}
};
} // namespace
char InsTest::ID = 0;
static RegisterPass<InsTest> X("InsTest", "collect instrutions type information!", false, false);
