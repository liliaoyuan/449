#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
	std::string 
	struct evl_token {
		enum token_type { NAME, NUMBER, SINGLE };
		token_type type;
		std::string str;
		int line_no;
	}; // struct evl_token
	typedef std::list<evl_token> evl_tokens;
	struct evl_statement{
		enum statement_type { MODULE, WIRE, COMPONENT, ENDMODULE };
		statement_type type; 
		evl_token tokens;
	}; // struct evl_statement
	typedef std::list<evl_statement> evl_statements;
	evl_token tokens;
	std::string evl_file = argv[1];
	if (!extract_tokens_tokens_from_file(evl_file, tokens))
	{
		return -1;
	}
	display_tokens(tokens);
	if (!store_tokens_to_file(evl_file + ".token", tokens))
	{
		return -1;
	}
}
evl_statements statements;
if (!group_tokens_into_statements(statements, tokens))
{
	return -1;
}
display_statements(statemenmts);

return 0;
}
bool group_tokens_into_statements(evl_statements &statements,evl_tokens &tokens)
{
	for (; !tokens.empty();)
	{
		evl_token token = tokens.front();
		if (token.type != evl_token::NAME)
		{
			std::cerr << "Need a NAME token but found '" << token.str << "'on line " << token.line_no << std::endl;
			return false;
		}
		if (token.str == "module") 
		{
			evl_statement module;
			module.type = evl_statement::MODULE;
			// Thinking of a function to replace the loop?
			for (; !tokens.empty();)
			{
				module.tokens.push_back(tokens.front());
				tokens.pop_front(); // consume one token per iteration
				if (module.tokens.back().str == ";")
					break; // exit if the ending ";" is found
			}
			if (module.tokens.back().str != ";")
			{
				std::cerr << "Look for ¡¯;¡¯ but reach the end of file" << std::endl; 
				return false;
			}// MODULE statement
			statements.push_back(module);
		}
		else if (token.str == "endmodule")
		{
			evl_statement endmodule;
			endmodule.type = evl_statement::ENDMODULE;
			endmodule.tokens.push_back(token);
			tokens.pop_front();
			statements.push_back(endmodule);
		}
		else if (token.str == "wire")
		{
			struct evl_wire 
			{ 
				std::string name; int width;
			}; // struct evl_wire 
			typedef std::vector<evl_wire> evl_wires; 
		}
		else 
		{
			struct evl_statement
			{
				std::string name;
				int width;
			};
			typedef std::vector<evl_components> evl_components;
		}
	}
}
bool move_tokens_to_statement£¨evl_tokens $tokens &statement_tokens, evl_tokens &tokens£©
{
	assert(statement_tokens.empty());
	evl_tokens::iterator next_sc = std::find_if(tokens.begin(), tokens.end(), token_is_semicolon);
if (next_sc == tokens.end())
{
	std::cerr << "Look for ¡¯;¡¯ but reach the end of file" << std::endl;
	return false;
}
evl_tokens::iterator after_sc = next_sc; ++after_sc;
statement_tokens.splice(statement_tokens.begin(),
	tokens, tokens.begin(), after_sc);
return true;
}
bool process_component_statement(evl_components &comps, evl_statement &s, evl_wires &wires)
{
	assert(s.type == evl_statement::COMPONENT);

	enum state_type { INIT, TYPE, NAME, PINS, PIN_NAME, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE, PINS_DONE, DONE };
	state_type state = INIT;
	evl_component comp;
	evl_pin pin;//should be outside th for loop, otherwise compiling error
	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front())//consumming the list
	{
		evl_token t = s.tokens.front();

		if (state == INIT)
		{
			if (t.type == evl_token::NAME)
			{
				comp.type = t.str;
				comp.name = "NONE";
				state = TYPE;
			}
			else
			{
				std::cerr << "need NAME type but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == TYPE)
		{
			if (t.type == evl_token::NAME)
			{
				comp.name = t.str;
				state = NAME;
			}
			else if (t.str == "(")
			{
				state = PINS;
			}
			else
			{
				std::cerr << "need NAME or (  but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == NAME)
		{
			if (t.str == "(")
			{
				state = PINS;
			}
			else
			{
				std::cerr << "need '('  but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS)
		{
			if (t.type == evl_token::NUMBER)
			{
				pin.bus_msb = atoi(t.str.c_str());
				state = BUS_MSB;
			}
			else
			{
				std::cerr << "need NUMBER but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS_MSB)
		{
			if (t.str == ":")
			{
				state = BUS_COLON;
			}
			else if (t.str == "]")
			{
				state = BUS_DONE;
			}
			else
			{
				std::cerr << "need ':' or ']' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS_COLON)
		{
			if (t.type == evl_token::NUMBER)
			{
				pin.bus_lsb = atoi(t.str.c_str());
				state = BUS_LSB;
			}
			else
			{
				std::cerr << "need '0'  but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS_LSB)
		{
			if (t.str == "]")
			{
				state = BUS_DONE;
			}
			else
			{
				std::cerr << "need ']'  but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == BUS_DONE)
		{
			if (t.str == ")")
			{
				comp.pins.push_back(pin);
				state = PINS_DONE;
			}
			else if (t.str == ",")
			{
				comp.pins.push_back(pin);
				state = PINS;
			}
			else
			{
				std::cerr << "need ')' or ','  but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == PINS)
		{
			if (t.type == evl_token::NAME)
			{
				pin.name = t.str;
				pin.bus_msb = -1;
				pin.bus_lsb = -1;
				state = PIN_NAME;
			}
			else
			{
				std::cerr << "need NAME  but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == PIN_NAME)
		{
			if (t.str == ",")
			{
				comp.pins.push_back(pin);
				state = PINS;
			}
			else if (t.str == ")")
			{
				comp.pins.push_back(pin);
				state = PINS_DONE;
			}
			else if (t.str == "[")
			{
				state = BUS;
			}
			else
			{
				std::cerr << "need ',' or ')' or '[' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}

		else if (state == PINS_DONE)
		{
			if (t.str == ";")
			{
				state = DONE;
				comps.push_back(comp);
			}
			else
			{
				std::cerr << "need ';' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}
		else
		{
			assert(false);
		}
	}
	if (!s.tokens.empty() || (state != DONE))
	{
		std::cerr << "statement was illegally ended\n" << std::endl;
		return false;
	}
	return true;
}