
#pragma once
#include "patcher.hpp"
#include "fileIO.hpp"
#include "utils.hpp"

namespace binPatcher
{

class Hooks : public Utils
{
public:
	Hooks(bool hook_format = false, string filename = "");
	void parse_hooks();

	bool fast_Compile_Hooks = false;
	bool indiv_Compile_Hooks = false;
	string filename_out;


private:
	int compile_Hook(string current_file, string outPath, string fileName);
	void build_O(string outPath, string fileName);
	void parse_build(int offset, string alone_Filename);
	void apply_Hook(string current_file, int offset, FileIO& f_out);
	void align_hook(int align_sizeL,string filename, string command);

	int align_sizeL;
	int Bytes_to_write;
	string _fast_hooks = make+" _fast_hooks";

};

} //namespace binPatcher