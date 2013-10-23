/* 
	CPage abstract class.
	(C) ultrashot 2011-2013
*/
#pragma once

#include "PageManager.hpp"

class CPage 
{
protected:
	SHELL_PAGE _page;
	SHELL_PAGEMANAGER _pageManager;
public:
	CPage()
	{
	}

	CPage(wchar_t *name)
	{
	}

	virtual ~CPage()
	{
	}

	virtual void Create() = NULL;
	virtual void Release() = NULL;
};
