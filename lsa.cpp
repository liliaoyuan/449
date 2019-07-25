#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
	std::string wires_name[100];
	std::string evl_token;
	int n_wires = 0;
	int n_component;
	char wire_length[100] = { 1 };
	

	if (argc < 2)
	{
		std::cerr << "You should provide a file name." << std::endl;
		return -1;
	}

	std::ifstream input_file(argv[1]);
	if (!input_file)
	{
		std::cerr << "I can't read " << argv[1] << "." << std::endl;
		return -1;
	}

	std::string output_file_name = std::string(argv[1]) + ".syntax";
	std::ofstream output_file(output_file_name);
	if (!output_file)
	{
		std::cerr << "I can't write " << argv[1] << ".tokens ." << std::endl;
		return -1;
	}

	std::string line;
	for (int line_no = 1; std::getline(input_file, line); ++line_no)
	{
		for (size_t i = 0; i < line.size();)
		{
			if (line[i] == '/')
			{
				++i;
				if ((i == line.size()) || (line[i] != '/'))
				{
					std::cerr << "LINE " << line_no
						<< ": a single / is not allowed" << std::endl;
					return -1;
				}
				break; // skip the rest of the line by exiting the loop
			}

			if ((line[i] == ' ') || (line[i] == '\t')
				|| (line[i] == '\r') || (line[i] == '\n'))
			{
				++i; // skip this space character
				continue; // skip the rest of the iteration
			}

			if ((line[i] == '(') || (line[i] == ')')
				|| (line[i] == '[') || (line[i] == ']')
				|| (line[i] == ':') || (line[i] == ';')
				|| (line[i] == ','))
			{
				//output_file << "SINGLE " << line[i] << std::endl;
				++i; // we consumed this character
				continue; // skip the rest of the iteration
			}

			if ((line[i] >= '0') && (line[i] <= '9'))       // find number
			{
				size_t number_begin = i;
				i = i + 1;
				for (i; i < line.size(); ++i)
				{
					if (!((line[i] >= '0') && (line[i] <= '9')))
						/*if((line[i]!= '0') || (line[i]!= '1') || (line[i] != '2')
						|| (line[i] != '3') || (line[i] != '4') || (line[i] != '5')
						|| (line[i] != '6') || (line[i] != '7') || (line[i] != '8')
						|| (line[i] != '9'))*/
					{
						break; // [number_begin, i) is the range for the token
					}
				}
				//output_file << "NUMBER "<< line.substr(number_begin, i - number_begin) << std::endl;
				continue;
			}
				

			// NAME
			if (((line[i] >= 'a') && (line[i] <= 'z'))       // a to z
				|| ((line[i] >= 'A') && (line[i] <= 'Z'))    // A to Z
				|| (line[i] == '_'))
			{
				size_t name_begin = i;
				for (++i; i < line.size(); ++i)
				{
					if (!(((line[i] >= 'a') && (line[i] <= 'z'))
						|| ((line[i] >= 'A') && (line[i] <= 'Z'))
						|| ((line[i] >= '0') && (line[i] <= '9'))
						|| (line[i] == '_') || (line[i] == '$')))
					{
						break; // [name_begin, i) is the range for the token
					}
				}
				evl_token = line.substr(name_begin, i - name_begin);
				if (evl_token == "module")
				{
					for (++i; i < line.size(); ++i) { if (line[i] == ';') { break; } }
					cur_token = line.substr(name_begin, i - name_begin);
					output_file <<cur_token<<std::endl;
					
				}
				//if (evl_token == "wire") {}
				if (evl_token == "endmodule") { output_file << evl_token << std::endl; }
			   
			
			}
			else
			{
				std::cerr << "LINE " << line_no
					<< ": invalid character" << std::endl;
				return -1;
			}
		}
	}

	return 0;
}
