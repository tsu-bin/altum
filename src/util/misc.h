//
// Created by Bin Xu on 2021/5/23.
//

#ifndef ALTUM_MISC_H
#define ALTUM_MISC_H

#include <sstream>
#include <thread>

template<class T>
class ShowType; //ShowType<decltype(x)>{};

std::string get_tid()
{
	std::stringstream ss;
	ss << std::hex << std::this_thread::get_id();
	return ss.str();
}

#endif //ALTUM_MISC_H
