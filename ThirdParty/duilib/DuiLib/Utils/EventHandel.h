#ifndef __UIEVENTHANDEL_H__
#define __UIEVENTHANDEL_H__

#pragma once

#include <functional>

namespace DuiLib 
{
	// typedef void(*EventHandel)(TNotifyUI& msg);

	class CEventHandelData;

	class DUILIB_API CEventHandel
	{
	public:
		CEventHandel();
		~CEventHandel();
		CEventHandel(CEventHandel& other) = delete;
		CEventHandel& operator = (CEventHandel& other) = delete;
		void RegistEvent(LPCTSTR lpszType, CControlUI* pControlUI, std::function<void (TNotifyUI& msg)> hendel);
		void UnregistEvent(LPCTSTR lpszType, CControlUI* pControlUI);
		bool Handel(TNotifyUI& msg);
		
	private:
		CEventHandelData* m_pData;
	};

}

#endif // __UIEVENTHANDEL_H__
