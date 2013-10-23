#pragma once
#ifndef XNAMESSAGEBOX_HPP
#define XNAMESSAGEBOX_HPP

#include "XNAFrameworkCore.h"

typedef enum
{
    XNA_RESULT_NONE	= 0,
    XNA_RESULT_LEFT = 1,
    XNA_RESULT_RIGHT = 2
}XNA_RESULT;

namespace Xna
{
    class Window
    {
    public:
        static XNA_RESULT MessageBox(HWND hWnd, wchar_t *text, wchar_t *caption, int flags)
        {
            int icon = None;
            wchar_t *btn1 = L"Ok";
            wchar_t *btn2 = NULL;
            if (flags & MB_OKCANCEL)
            {
                btn2 = L"Cancel";
            }
            else if (flags & MB_YESNO)
            {
                btn1 = L"Yes";
                btn2 = L"No";
            }
            else if (flags & MB_RETRYCANCEL)
            {
                btn1 = L"Retry";
                btn2 = L"Cancel";
            }
            if (flags & MB_ICONQUESTION)
                icon = Alert;
            else if (flags & MB_ICONEXCLAMATION)
                icon = Warning;

            GS_ActionDialogResult result;
            XNA_RESULT funcResult = XNA_RESULT_NONE;
            HANDLE mb = CreateMessageBoxDialog(caption, text, btn1, btn2, Alert);

            GetDialogResult(mb, &result);
            if (result.disReason == ButtonPressed)
            {
                if (result.iButtonPressed == 0)
                {
                    funcResult = XNA_RESULT_LEFT;
                }
                else if (result.iButtonPressed == 1)
                {
                    funcResult = XNA_RESULT_RIGHT;
                }
            }
            ReleaseDialog(mb);
            return funcResult;
        }
    };
}

#endif

