#include "RusExceptinon.h"
#include <sstream>

RusException::RusException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{}

const char* RusException::what() const noexcept
{
	std::ostringstream oss;

	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* RusException::GetType() const noexcept
{
	return "Rus exception";
}

const std::string& RusException::GetFile() const noexcept
{
	return file;
}

std::string RusException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File]" << file << std::endl
		<< "[Line]" << line;
	return oss.str();
}