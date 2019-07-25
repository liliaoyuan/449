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