#pragma once
#include <functional>

class ScopeTailed
{
public:
    ScopeTailed(std::function<void(void)> fun) : tailFun_(fun) { } 
    ~ScopeTailed(){ if (enabled_) { tailFun_(); } }
    void setDisable(){ enabled_ = false; } private:

private:
	std::function<void(void)> tailFun_;
	bool enabled_ = true;
};