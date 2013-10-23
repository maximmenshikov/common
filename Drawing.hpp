#pragma once

#define DelObject(a) if (a) { DeleteObject(a); a = NULL; }
#define DelDC(a) if (a) { DeleteDC(a); a = NULL; }

namespace System
{
	class Drawing
	{
	public:
		static void DrawBitmap(HDC hdcDest, HBITMAP hbmSrc, int x, int y, int width, int height, int useAlphaBlend)
		{
			HDC hDC = CreateCompatibleDC(NULL);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hDC, hbmSrc);
			if (useAlphaBlend)
			{
				static BLENDFUNCTION bf;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 0xff;
				bf.AlphaFormat = AC_SRC_ALPHA;
				AlphaBlend (hdcDest, x, y, width, height, hDC, 0, 0, width, height, bf);
			}
			else
			{
				BitBlt(hdcDest, 
					x, y, 
					width, height, 
					hDC, 
					0, 0, 
					SRCCOPY);
			}
			SelectObject(hDC, hbmOld);
			DeleteDC(hDC);
		}

		static void DrawBitmap(HDC hdcDest, HDC hdcSrc, int x, int y, int width, int height, int useAlphaBlend)
		{
			if (useAlphaBlend)
			{
				static BLENDFUNCTION bf;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 0xff;
				bf.AlphaFormat = AC_SRC_ALPHA;
				AlphaBlend (hdcDest, x, y, width, height, hdcSrc, 0, 0, width, height, bf);
			}
			else
			{
				BitBlt(hdcDest, 
					x, y, 
					width, height, 
					hdcSrc, 
					0, 0, 
					SRCCOPY);
			}
		}

		static void DrawString(HDC hDC, int x, int y, int x2, int y2, wchar_t *str, int bitmask, int fontsize, int color, wchar_t *fontstr, int weight)
		{
			LOGFONT lf;
			RECT rect = {x, y, x2, y2};
			ZeroMemory(&lf, sizeof(lf));
			lf.lfWeight = weight;
			lf.lfHeight = -fontsize;
			lf.lfOutPrecision = OUT_RASTER_PRECIS;
			lf.lfQuality = ANTIALIASED_QUALITY;
			lf.lfPitchAndFamily = VARIABLE_PITCH | FF_ROMAN;
			wcscpy(lf.lfFaceName, fontstr);

			HFONT hFont = CreateFontIndirect(&lf);
			if (hFont)
			{
				COLORREF oldColor = SetTextColor(hDC, color);
				int oldBkMode = SetBkMode(hDC, TRANSPARENT);
				HFONT oldFont = (HFONT)SelectObject(hDC, hFont);
				if (oldFont)
				{
					DrawText(hDC, str, -1, &rect, bitmask);
					SelectObject(hDC, oldFont);
				}
				SetBkMode(hDC, oldBkMode);
				SetTextColor(hDC, oldColor);
				DeleteObject(hFont);
			}
		}

		static void DrawString(HDC hDC, int x, int y, int x2, int y2, wchar_t *str, int bitmask)
		{
			RECT rect = {x, y, x2, y2};
			DrawText(hDC, str, -1, &rect, bitmask);
		}

	};
}