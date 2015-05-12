#pragma once

#include <string>

/**
 * \brief Request或Response中头的键-值对
 */ 
struct header_t {
	std::string name;
	std::string value;
};
