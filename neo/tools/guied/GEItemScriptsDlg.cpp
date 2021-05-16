/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company.
Copyright (C) 2014-2016 Robert Beckebans
Copyright (C) 2014-2016 Kot in Action Creative Artel

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#include <afxwin.h>
#include <afxcmn.h>
#pragma hdrstop
#include "precompiled.h"

#include "sys/win32/rc/guied_resource.h"

#include "GEApp.h"
#include "GEPropertyPage.h"

bool GEItescriptsDlg_Init ( HWND hwnd, rvGEWorkspace *newWorkspace)
{
	idWindow*			window;
	rvGEWindowWrapper*	wrapper;
	HWND				script;

	rvGEWorkspace *workspace = 0;
	if (newWorkspace == 0)
	{
		workspace = gApp.GetActiveWorkspace();
	}
	else
	{
		workspace = newWorkspace;
	}
	
	bool bSel = false;

	if (workspace)
	{
		if (workspace->GetSelectionMgr().Num() > 0)
		{
			window = workspace->GetSelectionMgr()[0];

			if (window)
			{
				bSel = true;
				// Get the window wrapper of the script window
				wrapper = rvGEWindowWrapper::GetWrapper(window);
				assert(wrapper);

				// Get the edit box used to edit the script
				script = GetDlgItem(hwnd, IDC_GUIED_SCRIPT);

				UINT tabsize = 16;
				SendMessage(script, EM_SETTABSTOPS, 1, (LPARAM)&tabsize);

				TEXTMETRIC tm;
				HDC dc;
				dc = GetDC(script);
				GetTextMetrics(dc, &tm);
				ReleaseDC(script, dc);

				LOGFONT lf;
				ZeroMemory(&lf, sizeof(lf));
				lf.lfHeight = tm.tmHeight;
				strcpy(lf.lfFaceName, "Courier New");

				SendMessage(script, WM_SETFONT, (WPARAM)CreateFontIndirect(&lf), 0);

				SendMessage(script, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELONG(10, 10));

				int i;

				for (i = 0; i < wrapper->GetVariableDict().GetNumKeyVals(); i++)
				{
					const idKeyValue* key = wrapper->GetVariableDict().GetKeyVal(i);

					SendMessage(script, EM_SETSEL, -1, -1);
					SendMessage(script, EM_REPLACESEL, FALSE, (LPARAM)key->GetKey().c_str());
					SendMessage(script, EM_SETSEL, -1, -1);
					SendMessage(script, EM_REPLACESEL, FALSE, (LPARAM)"\t");
					SendMessage(script, EM_SETSEL, -1, -1);
					SendMessage(script, EM_REPLACESEL, FALSE, (LPARAM)key->GetValue().c_str());
					SendMessage(script, EM_SETSEL, -1, -1);
					SendMessage(script, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
				}

				if (i)
				{
					SendMessage(script, EM_SETSEL, -1, -1);
					SendMessage(script, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
				}

				for (i = 0; i < wrapper->GetScriptDict().GetNumKeyVals(); i++)
				{
					const idKeyValue* key = wrapper->GetScriptDict().GetKeyVal(i);

					SendMessage(script, EM_SETSEL, -1, -1);
					SendMessage(script, EM_REPLACESEL, FALSE, (LPARAM)va("%s\r\n", key->GetKey().c_str()));
					SendMessage(script, EM_SETSEL, -1, -1);
					SendMessage(script, EM_REPLACESEL, FALSE, (LPARAM)key->GetValue().c_str());
					SendMessage(script, EM_SETSEL, -1, -1);
					SendMessage(script, EM_REPLACESEL, FALSE, (LPARAM)"\r\n\r\n");
				}

				SendMessage(script, EM_SETSEL, 0, 0);
				SendMessage(script, EM_SCROLLCARET, 0, 0);
			}
		}
	}


	if (!bSel)
	{
		script = GetDlgItem(hwnd, IDC_GUIED_SCRIPT);
		SendMessage(script, WM_SETTEXT, 0, (LPARAM)"\0");
		SendMessage(script, EM_SETSEL, 0, 0);
		SendMessage(script, EM_SCROLLCARET, 0, 0);
	}

	return true;
}

void GEItemScriptsDlg_Clear(HWND hwnd)
{
	HWND script = GetDlgItem(hwnd, IDC_GUIED_SCRIPT);
	SendMessage(script, WM_SETTEXT, 0, (LPARAM)"\0");
	SendMessage(script, EM_SETSEL, 0, 0);
	SendMessage(script, EM_SCROLLCARET, 0, 0);
}

bool GEItescriptsDlg_Apply(HWND hwnd, rvGEWorkspace *newWorkspace)
{
	idWindow*			window;
	rvGEWindowWrapper*	wrapper;
	HWND				script;

	rvGEWorkspace *pCurr = 0;
	if (newWorkspace != 0)
	{
		pCurr = newWorkspace;
	}
	else
	{
		pCurr = gApp.GetActiveWorkspace();
	}

	if (pCurr)
	{

		if (pCurr->GetSelectionMgr().Num() > 0)
		{
			window = pCurr->GetSelectionMgr()[0];

			if (window)
			{
				pCurr->SetModified(true);
				// Get the window wrapper of the script window
				wrapper = rvGEWindowWrapper::GetWrapper(window);
				assert(wrapper);

				// Get the edit box used to edit the script
				script = GetDlgItem(hwnd, IDC_GUIED_SCRIPT);

				GETTEXTLENGTHEX textLen;
				int				chars;
				textLen.flags = GTL_DEFAULT | GTL_USECRLF;
				textLen.codepage = CP_ACP;
				chars = SendMessage(script, EM_GETTEXTLENGTHEX, (WPARAM)&textLen, 0);

				char* text = new char[chars + 1];

				GETTEXTEX getText;
				getText.cb = chars + 1;
				getText.codepage = CP_ACP;
				getText.flags = GT_DEFAULT | GT_USECRLF;
				getText.lpDefaultChar = NULL;
				getText.lpUsedDefChar = NULL;
				SendMessage(script, EM_GETTEXTEX, (WPARAM)&getText, (LPARAM)text);

				idStr parse = text;
				delete[] text;

				try
				{
					idParser src(parse, parse.Length(), "", LEXFL_ALLOWMULTICHARLITERALS | LEXFL_NOSTRINGCONCAT | LEXFL_ALLOWBACKSLASHSTRINGCONCAT);
					idToken token;

					wrapper->GetVariableDict().Clear();
					wrapper->GetScriptDict().Clear();

					while (src.ReadToken(&token))
					{
						idStr scriptName;
						idStr out;

						if (!token.Icmp("definevec4"))
						{
							idToken token2;
							idStr	result;

							if (!src.ReadToken(&token2))
							{
								src.Error("expected define name");
								return false;
							}

							idWinVec4				var;
							idUserInterfaceLocal	ui;
							idWindow				tempwin(&ui);
							idStr					out;
							int						i;

							src.SetMarker();
							for (i = 0; i < 3; i++)
							{
								tempwin.ParseExpression(&src, &var);
								src.ExpectTokenString(",");
							}

							tempwin.ParseExpression(&src, &var);
							src.GetStringFromMarker(out, true);

							wrapper->GetVariableDict().Set(token + "\t\"" + token2 + "\"", out);

							continue;
						}
						else if (!token.Icmp("definefloat") || !token.Icmp("float"))
						{
							idToken token2;
							idStr	result;

							if (!src.ReadToken(&token2))
							{
								src.Error("expected define name");
								return false;
							}

							idWinFloat				var;
							idUserInterfaceLocal	ui;
							idWindow				tempwin(&ui);
							idStr					out;

							src.SetMarker();
							tempwin.ParseExpression(&src, &var);
							src.GetStringFromMarker(out, true);

							wrapper->GetVariableDict().Set(token + "\t\"" + token2 + "\"", out);

							continue;
						}

						// If the token is a scriptdef then just parse out the
						// braced section and add it to the list.  Right now only
						// one scriptdef per window is supported
						else if (!token.Icmp("scriptdef"))
						{
							scriptName = "scriptDef";
						}
						else if (!token.Icmp("ontime"))
						{
							if (!src.ReadToken(&token))
							{
								src.Error("expected time");
								return false;
							}

							scriptName = "onTime ";
							scriptName.Append(token);
						}
						else if (!token.Icmp("onevent"))
						{
							/*if ( !src.ReadToken ( &token ) )
							{
							src.Error ( "expected time" );
							return false;
							}*/

							scriptName = "onEvent ";
							//scriptName.Append ( token );
						}
						else if (!token.Icmp("onNamedEvent"))
						{
							if (!src.ReadToken(&token))
							{
								src.Error("expected event name");
								return false;
							}

							scriptName = "onNamedEvent ";
							scriptName.Append(token);
						}
						else
						{

							int i;

							for (i = 0; i < idWindow::SCRIPT_COUNT; i++)
							{
								if (idStr::Icmp(idWindow::ScriptNames[i], token) == 0)
								{
									scriptName = idWindow::ScriptNames[i];
									break;
								}
							}

							if (i >= idWindow::SCRIPT_COUNT)
							{
								src.Error("expected script name");
								return false;
							}
						}

						if (!src.ParseBracedSectionExact(out, 1))
						{
							return false;
						}

						wrapper->GetScriptDict().Set(scriptName, out);
					}
				}
				catch (idException &e)
				{
					MessageBox(hwnd, e.GetError(), "Script Error", MB_OK | MB_ICONERROR);
					return false;
				}
			}
		}
	}

	return true;
}

INT_PTR CALLBACK GEItescriptsDlg_WndProc ( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			SetWindowLongPtr ( hwnd, GWLP_USERDATA, lParam );
			GEItescriptsDlg_Init ( hwnd , 0);

			gApp.GetOptions().GetWindowPlacement ( "scripts", hwnd );

			// Let it fall through so the scripts window gets resized.

		case WM_SIZE:
		{
			RECT rClient;
			GetClientRect ( hwnd, &rClient );
			MoveWindow ( GetDlgItem ( hwnd, IDC_GUIED_SCRIPT ),
						 rClient.left, rClient.top,
						 rClient.right - rClient.left,
						 rClient.bottom - rClient.top,
						 FALSE );
			break;
		}
		case WM_ERASEBKGND:
			return TRUE;

		case WM_DESTROY:
		case WM_CLOSE:
			SendMessage(gApp.GetMDIFrame(), WM_COMMAND, MAKELONG(ID_GUIED_WINDOW_SHOWSCRIPTS, 0), 0);
			break;
	}

	return FALSE;
}

/*
================
GEItemScriptsDlg_DoModal

Starts the item properties dialog
================
*/
HWND GEItemScriptsDlg_DoModal(HWND parent, idWindow* window)
{
	LoadLibrary ( "Riched20.dll" );

	HWND hWnd = CreateDialog(gApp.GetInstance(), MAKEINTRESOURCE(IDD_GUIED_SCRIPTS), parent, GEItescriptsDlg_WndProc);
	
	//HWND script = GetDlgItem(hWnd, IDC_GUIED_SCRIPT);
	//SetWindowLongPtr(script, GWLP_WNDPROC, (LONG_PTR)GEScriptEdit_WndProc);

	gApp.GetOptions().GetWindowPlacement("scripts", hWnd);

	ShowWindow(hWnd, gApp.GetOptions().GetScriptsVisible() ? SW_SHOW : SW_HIDE);

	return hWnd;
}
